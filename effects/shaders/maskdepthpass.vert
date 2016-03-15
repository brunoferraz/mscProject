#version 400

in vec4 in_Position;
in vec3 in_Normal;

out float depth;
out vec3 normal;
out vec4 vert;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
    mat4 modelViewMatrix = viewMatrix * modelMatrix;

    mat4 normalMatrix = transpose(inverse(modelViewMatrix));
    normal = normalize(vec3(normalMatrix * vec4(in_Normal,0.0)).xyz);
    vert = modelViewMatrix * in_Position;

    vec4 proj = viewMatrix * modelMatrix * in_Position;
//    depth = -(proj.z - near) / (far - near);


    gl_Position = projectionMatrix * proj;
}
