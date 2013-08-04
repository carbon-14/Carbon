
const float Roughness       = 0.05;
const float SqrRoughness    = Roughness * Roughness;
const float RoughnessA      = 1.0 - 0.5 * SqrRoughness / ( SqrRoughness + 0.33 );
const float RoughnessB      = 0.45 * SqrRoughness / ( SqrRoughness + 0.09 );

const float FresnelCoeff    = 0.5;
