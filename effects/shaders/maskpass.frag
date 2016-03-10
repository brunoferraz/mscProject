#version 400
out vec4 out_Angle;
out vec4 out_Depth;
out vec4 out_Border;

in float depth;
in vec3 normal;
in vec4 vert;

uniform vec2 viewportSize;
uniform sampler2D in_depthmap;

void main(void)
{
//    out_Color = vec4(depth, depth, depth, 1.0);
//    out_Angle   = vec4(1.0, 0.0, 0.0, 1.0);
    vec2 coord = vec2(gl_FragCoord.x/viewportSize.x, gl_FragCoord.y/viewportSize.y);
    vec3 eyeDirection = -normalize(vert.xyz);

    //ANGLE
    float angle = dot(eyeDirection, normal);

    //BORDER
    float stepX = 1/viewportSize.x;
    float stepY = 1/viewportSize.y;


    vec4 c1 = texture2D(in_depthmap, vec2(coord.x-stepX, coord.y-stepY));
    vec4 c2 = texture2D(in_depthmap, vec2(coord.x, coord.y -stepY));
    vec4 c3 = texture2D(in_depthmap, vec2(coord.x+stepX, coord.y -stepY));
    vec4 c4 = texture2D(in_depthmap, vec2(coord.x-stepX, coord.y));
    vec4 c5 = texture2D(in_depthmap, vec2(coord.x, coord.y));
    vec4 c6 = texture2D(in_depthmap, vec2(coord.x + stepX, coord.y));
    vec4 c7 = texture2D(in_depthmap, vec2(coord.x - stepX, coord.y+ stepY));
    vec4 c8 = texture2D(in_depthmap, vec2(coord.x, coord.y + stepY));
    vec4 c9 = texture2D(in_depthmap, vec2(coord.x + stepX, coord.y + stepY));

    vec4 sX = (c1 * -1) + (c4 * -2) + (c7 * -1) + (c3 * 1) + (c6 * 2) + (c9 * 1);
    vec4 sY = (c1 * +1) + (c2 * +2) + (c3 * +1) + (c7 * -1) + (c8 * -2) + (c9 *-1);
    vec4 border = sX + sY;



    out_Angle = vec4(vec3(angle),1.0);
    out_Depth   = texture2D(in_depthmap, coord);
    out_Border  = border;
}
