#version 420

#define PAL_SECAM
#define NTSC

in Data
{
    vec4 color;
} DataIn;

out vec4 outColor;

layout(binding=0) uniform sampler2D carbonTex;

void main()
{
    vec2 uv = DataIn.color.xy;
    vec3 color = texture2D( carbonTex, uv ).rgb;

    outColor = vec4( color, 1.0 );
}
