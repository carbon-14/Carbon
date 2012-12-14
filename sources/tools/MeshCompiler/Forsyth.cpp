#include "Forsyth.h"

//
// www.martin.st/thesis/efficient_triangle_reordering.pdf
// Thx guy !
//

#include <stdint.h>

// Set these to adjust the performance and result quality
#define VERTEX_CACHE_SIZE 8
#define CACHE_FUNCTION_LENGHT 32

// The size of these data types affect the memory usage
typedef uint16_t ScoreType;
#define SCORE_SCALING 7281

typedef uint8_t AdjacencyType;
#define MAX_ADJACENCY UINT8_MAX

typedef int32_t VertexIndexType;
typedef int8_t  CachePosType;
typedef int32_t TriangleIndexType;
typedef int32_t ArrayIndexType;

// The size of the precalculated tables
#define CACHE_SCORE_TABLE_SIZE 32
#define VALENCE_SCORE_TABLE_SIZE 32
#if CACHE_SCORE_TABLE_SIZE < VERTEX_CACHE_SCORE_SIZE
#error Vertex score table too small
#endif

// Precalculated tables
static ScoreType cachePositionScore[CACHE_SCORE_TABLE_SIZE];
static ScoreType valenceScore[VALENCE_SCORE_TABLE_SIZE];

#define ISADDED(x)  (triangleAdded[(x) >> 3] &  ( 1 << ( x & 7 ) ) )
#define SETADDED(x) (triangleAdded[(x) >> 3] |= ( 1 << ( x & 7 ) ) )

// Score function constants
#define CACHE_DECAY_POWER   1.5f
#define LAST_TRI_SCORE      0.75f
#define VALENCE_BOOST_SCALE 2.0f
#define VALENCE_BOOST_POWER 0.5f

// Preacalculate the tables.
void initForsyth()
{
    for ( size_t i=0; i<CACHE_SCORE_TABLE_SIZE; ++i )
    {
        float score = 0;
        if ( i < 3 )
        {
            // This vertex was used in the last triangle,
            // so it has a fixed score, which ever of the three
            // it's in. Otherwise, you cacn get every different
            // answers depending on wether you add
            // the triangle 1, 2, 3 or 3, 1, 2 - which is silly.
            score = LAST_TRI_SCORE;
        }
        else
        {
            // Points for being hight in the cache.
            const float scaler = 1.0f / ( CACHE_FUNCTION_LENGHT - 3 );
            score = 1.0f - ( i - 3 ) * scaler;
            score = powf( score, CACHE_DECAY_POWER );
        }
        cachePositionScore[i] = (ScoreType)( SCORE_SCALING * score );
    }

    for ( size_t i=0; i<VALENCE_SCORE_TABLE_SIZE; ++i )
    {
        // Bonus points for having a low number of tris still to
        // use the vert, so we get rid of lone verts quickly.
        float valenceBoost = powf( (float)i, -VALENCE_BOOST_POWER );
        float score = VALENCE_BOOST_SCALE * valenceBoost;
        valenceScore[i] = (ScoreType)( SCORE_SCALING * score );
    }
}

// Calculate the score for a vertex.
ScoreType findVertexScore( int numActiveTris, int cachePosition )
{
    if ( numActiveTris == 0 )
    {
        // No triangles need this vertex !
        return 0;
    }

    ScoreType score = 0;
    if ( cachePosition < 0)
    {
        // Vertex is not in LRU cache _ no score.
    }
    else
    {
        score = cachePositionScore[ cachePosition ];
    }

    if ( numActiveTris < VALENCE_SCORE_TABLE_SIZE )
    {
        score += valenceScore[ numActiveTris ];
    }

    return score;
}

void ReorderForsyth( std::vector< size_t >& indices, const size_t nVertices )
{
    size_t nTriangles = indices.size() / 3;

    // The tables need not be inited every time this function
    // is use. Either call initForsyth from the calling process,
    // or just replace the score tables with precalculated values.
    initForsyth();

    std::vector<AdjacencyType> numActiveTris( nVertices, 0 );

    // First scan over vertex data, count the total number of
    // occurences of each vertex.
    for ( size_t i=0; i<indices.size(); ++i )
    {
        AdjacencyType& adj = numActiveTris[indices[i]];
        if ( adj == MAX_ADJACENCY )
        {
            // Unsupported mesh.
            // vertex shared by too many triangles.
            return;
        }
        ++adj;
    }

    std::vector< ArrayIndexType >       offsets(nVertices);
    std::vector< ScoreType >            lastScore(nVertices);
    std::vector< CachePosType >         cacheTag(nVertices);
    std::vector< uint8_t >              triangleAdded( ( nTriangles + 7 ) / 8, 0 );
    std::vector< ScoreType >            triangleScore( nTriangles, 0 );
    std::vector< TriangleIndexType >    triangleIndices( indices.size(), 0 );

    // Count the triangle array offset for each vertex,
    // initialize the rest of the data.
    int sum = 0;
    for ( size_t i=0; i<nVertices; ++i )
    {
        offsets[i] = sum;
        sum += numActiveTris[i];
        numActiveTris[i] = 0;
        cacheTag[i] = -1;
    }

    // Fill the vertex data structures with indices to the triangles
    // using each vertex.
    for ( size_t i=0; i<nTriangles; ++i )
    {
        for ( size_t j=0; j<3; ++j )
        {
            size_t v = indices[ 3 * i + j ];
            triangleIndices[ offsets[v] + numActiveTris[v] ] = i;
            ++numActiveTris[v];
        }
    }

    // Initialize the score for all vertices.
    for ( size_t i=0; i<nVertices; ++i )
    {
        lastScore[i] = findVertexScore( numActiveTris[i], cacheTag[i] );
        for ( size_t j=0; j<numActiveTris[i]; ++j )
        {
            triangleScore[ triangleIndices[ offsets[i] + j ] ] += lastScore[i];
        }
    }

    // Find the best triangle.
    int bestTriangle = -1;
    int bestScore = -1;

    for ( size_t i=0; i<nTriangles; ++i )
    {
        if ( triangleScore[i] > bestScore )
        {
            bestScore = triangleScore[i];
            bestTriangle = i;
        }
    }

    std::vector< TriangleIndexType > outTriangles( nTriangles );
    size_t outPos = 0;

    // Initialize the cache.
    int cache[ VERTEX_CACHE_SIZE + 3 ];
    for ( size_t i=0; i<(VERTEX_CACHE_SIZE+3); ++i )
    {
        cache[i] = -1;
    }

    size_t scanPos = 0;

    // Output the currently best triangle, as long as there
    // are triangles left to output.
    while ( bestTriangle >= 0 )
    {
        // Mark the triangle as added.
        SETADDED( bestTriangle );
        // Output this triangle.
        outTriangles[outPos++] = bestTriangle;
        for ( size_t i=0; i<3; ++i )
        {
            // Update thi svertex
            size_t v = indices[ 3 * bestTriangle + i ];

            // Check the current cache position, if it
            // is in the cache
            int endpos = cacheTag[v];
            if ( endpos < 0 )
            {
                endpos = VERTEX_CACHE_SIZE + i;
            }

            // Move all cache entries from the previous position
            // in the cache to the new target position (i) one
            // step backwards.
            for ( int j=endpos; j>i; --j )
            {
                cache[j] = cache[j-1];
                // If this cache slot contains a real
                // vertex, updata its cache tag.
                if ( cache[j] >= 0 )
                {
                    ++cacheTag[ cache[j] ];
                }
            }

            // Insert the current vertex into its new target
            // slot.
            cache[i] = v;
            cacheTag[v] = i;

            // Find the current triangle in the list of active
            // triangles and remove it (moving the last
            // triangle in the list to the slot of this triangle).
            for ( size_t j=0; j<numActiveTris[v]; ++j )
            {
                if ( triangleIndices[ offsets[v] + j ] == bestTriangle )
                {
                    triangleIndices[ offsets[v] + j ] = triangleIndices[ offsets[v] + numActiveTris[v] - 1 ];
                    break;
                }
            }

            // Shorten the list.
            --numActiveTris[v];
        }

        // Update the scores of all triangles in the cache
        for ( size_t i=0; i<(VERTEX_CACHE_SIZE+3); ++i )
        {
            int v = cache[i];

            if ( v < 0 )
                break;

            // This vertex has been pushed outside of the
            // actual cache
            if ( i >= VERTEX_CACHE_SIZE )
            {
                cacheTag[v] = -1;
                cache[i] = -1;
            }

            ScoreType newScore = findVertexScore( numActiveTris[v], cacheTag[v] );

            ScoreType diff = newScore - lastScore[v];
            for ( size_t j=0; j<numActiveTris[v]; ++j )
            {
                triangleScore[ triangleIndices[ offsets[v] + j ] ] += diff;
                lastScore[v] = newScore;
            }
        }

        // Find the besttriangle referenced by vertices in the
        // cache
        bestTriangle = -1;
        bestScore = -1;
        for ( size_t i=0; i<VERTEX_CACHE_SIZE; ++i )
        {
            if ( cache[i] < 0 )
                break;

            int v = cache[i];
            for ( size_t j=0; j<numActiveTris[v]; ++j )
            {
                int t = triangleIndices[ offsets[v] + j ];
                if ( triangleScore[t] > bestScore )
                {
                    bestTriangle = t;
                    bestScore = triangleScore[t];
                }
            }
        }

        // If no active triangle was found at all, continue
        // scanning the whole list of triangles
        if ( bestTriangle < 0 )
        {
            for (; scanPos < nTriangles; ++scanPos )
            {
                if ( !ISADDED(scanPos) )
                {
                    bestTriangle = scanPos;
                    break;
                }
            }
        }
    }

    std::vector< size_t > outIndices( indices.size() );

    outPos = 0;
    for ( size_t i=0; i<nTriangles; ++i )
    {
        size_t t = outTriangles[i];
        for ( size_t j=0; j<3; ++j )
        {
            outIndices[outPos++] = indices[ 3 * t + j ];
        }
    }

    indices = outIndices;
}
