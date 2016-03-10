#version 400
out vec4 out_Color;

in float depth;
in vec3 normal;
in vec4 vert;
uniform vec2 viewportSize;

uniform sampler2D angleMask;

void main(void)
{
    vec2 coord = vec2(gl_FragCoord.x/viewportSize.x, gl_FragCoord.y/viewportSize.y);
    out_Color = texture2D(angleMask, coord);
}
