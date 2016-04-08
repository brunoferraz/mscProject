#version 400

in vec4 in_Position;
in vec3 in_Normal;
uniform float near;
uniform float far;

out float depth;
out float depthNonNorm;
out vec3 normal;


uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
    vec4 proj = viewMatrix * modelMatrix * in_Position;
    depth = -(proj.z - near) / (far - near);
    depthNonNorm = proj.z;

    mat4 modelViewMatrix = viewMatrix * modelMatrix;
    mat4 normalMatrix = transpose(inverse(modelViewMatrix));
    normal = normalize(vec3(normalMatrix * vec4(in_Normal,0.0)).xyz);

    gl_Position = projectionMatrix * proj;
}
