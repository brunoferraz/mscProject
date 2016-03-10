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

    float angle = dot(eyeDirection, normal);

//    texture2D(in_depthmap, coord.xy);
    out_Angle = vec4(vec3(angle),1.0);
    out_Depth   = texture2D(in_depthmap, coord);
    out_Border  = vec4(0.0, 0.0, 1.0, 1.0);
}
