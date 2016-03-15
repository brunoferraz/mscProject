#version 400
out vec4 out_Angle;

in float depth;
in vec3 normal;
in vec4 vert;

uniform vec2 viewportSize;
uniform sampler2D depthMap;

void main(void)
{
    vec2 coord = vec2(gl_FragCoord.x/viewportSize.x, gl_FragCoord.y/viewportSize.y);

    float stepX = (5 * 1)/viewportSize.x;
    float stepY = (5 * 1)/viewportSize.y;

    vec4 c1 = texture2D(depthMap, vec2(coord.x-stepX, coord.y-stepY));
    vec4 c2 = texture2D(depthMap, vec2(coord.x, coord.y -stepY));
    vec4 c3 = texture2D(depthMap, vec2(coord.x+stepX, coord.y -stepY));
    vec4 c4 = texture2D(depthMap, vec2(coord.x-stepX, coord.y));
    vec4 c5 = texture2D(depthMap, vec2(coord.x, coord.y));
    vec4 c6 = texture2D(depthMap, vec2(coord.x + stepX, coord.y));
    vec4 c7 = texture2D(depthMap, vec2(coord.x - stepX, coord.y+ stepY));
    vec4 c8 = texture2D(depthMap, vec2(coord.x, coord.y + stepY));
    vec4 c9 = texture2D(depthMap, vec2(coord.x + stepX, coord.y + stepY));

    float f1 = 1.0;
    float f2 = 10.0;

    vec4 sX = (c1 * -f1) + (c4 * -f2) + (c7 * -f1) + (c3 * f1) + (c6 * f2) + (c9 * f1);
    vec4 sY = (c1 * +f1) + (c2 * +f2) + (c3 * +f1) + (c7 * -f1) + (c8 * -f2) + (c9 *-f1);
    vec4 border = sX + sY;


    out_Angle = border;
}
