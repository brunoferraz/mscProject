#version 400
out vec4 out_Border;

in float depth;
in vec3 normal;
in vec4 vert;

uniform vec2 viewportSize;
uniform sampler2D depthMap;

float threshold = 0.002;

void main(void)
{
    vec2 coord = vec2(gl_FragCoord.x/viewportSize.x, gl_FragCoord.y/viewportSize.y);

    float stepX = (1 * 1)/viewportSize.x;
    float stepY = (1 * 1)/viewportSize.y;

    float c1 = texture2D(depthMap, vec2(coord.x-stepX,  coord.y-stepY)).x;
    float c2 = texture2D(depthMap, vec2(coord.x,        coord.y -stepY)).x;
    float c3 = texture2D(depthMap, vec2(coord.x+stepX,  coord.y -stepY)).x;
    float c4 = texture2D(depthMap, vec2(coord.x-stepX,  coord.y)).x;
    float c5 = texture2D(depthMap, vec2(coord.x,        coord.y)).x;
    float c6 = texture2D(depthMap, vec2(coord.x +stepX, coord.y)).x;
    float c7 = texture2D(depthMap, vec2(coord.x -stepX, coord.y+ stepY)).x;
    float c8 = texture2D(depthMap, vec2(coord.x,        coord.y + stepY)).x;
    float c9 = texture2D(depthMap, vec2(coord.x +stepX, coord.y + stepY)).x;

    float f1 = 1.0;
    float f2 = 2.0;

    float sX = (c1 * -f1) + (c4 * -f2) + (c7 * -f1) + (c3 * f1) + (c6 * f2) + (c9 * f1);
    float sY = (c1 * +f1) + (c2 * +f2) + (c3 * +f1) + (c7 * -f1) + (c8 * -f2) + (c9 *-f1);
    float sXY = sqrt(pow(sX,2) + pow(sY,2));

    float border = 0;
    if(sXY > threshold) border = 1;

    out_Border = vec4(border);
}
