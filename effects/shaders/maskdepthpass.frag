#version 400
out vec4 out_Depth;

in float depth;
in vec3 normal;
in vec4 vert;

uniform float minDepth;
uniform float maxDepth;

uniform sampler2D depthMap;
uniform vec2 viewportSize;

void main(void)
{
//    out_Color = vec4(depth, depth, depth, 1.0);
//    out_Angle   = vec4(1.0, 0.0, 0.0, 1.0);
    vec2 coord = vec2(gl_FragCoord.x/viewportSize.x, gl_FragCoord.y/viewportSize.y);
    float val =  texture2D(depthMap, coord).x;
    float valnorm = (val - minDepth)/(maxDepth - minDepth);

    out_Depth = vec4(vec3(valnorm),1.0);
}
