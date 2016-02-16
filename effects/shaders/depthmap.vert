#version 400

in vec4 in_Position;
uniform float near;
uniform float far;

out float depth;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
    vec4 proj = viewMatrix * modelMatrix * in_Position;
    depth = -(proj.z - near) / (far - near);


    gl_Position = projectionMatrix * proj;
}
