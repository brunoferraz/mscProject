#version 400
out vec4 out_Color;
out vec4 out_ColorNon;

in float depth;
in float depthNonNorm;

void main(void)
{
    out_Color = vec4(vec3(depth), 1.0);
    out_ColorNon = vec4(vec3(depthNonNorm), 1.0);
}
