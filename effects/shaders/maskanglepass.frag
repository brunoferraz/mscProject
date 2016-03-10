#version 400
out vec4 out_Angle;

in float depth;
in vec3 normal;
in vec4 vert;

void main(void)
{
//    out_Color = vec4(depth, depth, depth, 1.0);
//    out_Angle   = vec4(1.0, 0.0, 0.0, 1.0);
//    vec2 coord = vec2(gl_FragCoord.x/viewportSize.x, gl_FragCoord.y/viewportSize.y);


    //ANGLE
    vec3 eyeDirection = -normalize(vert.xyz);
    float angle = dot(eyeDirection, normal);

    out_Angle = vec4(vec3(angle),1.0);
}
