#version 400
out vec4 out_Color;

in float depth;
in vec3 normal;
in vec4 vert;
uniform vec2 viewportSize;

uniform sampler2D angleMask;
uniform sampler2D depthMask;

void main(void)
{
    vec2 coord = vec2(gl_FragCoord.x/viewportSize.x, gl_FragCoord.y/viewportSize.y);
    vec4 a = texture2D(angleMask, coord);
    vec4 d = texture2D(depthMask, coord);

    float r = 1.0;
    float g = 1.0;
    float b = 1.0;

    r*= a.x;
    g*= a.y;
    b*= a.z;

    r*= d.x;
    g*= d.y;
    b*= d.z;

    out_Color = vec4(r, g, b, 1.0);
}
