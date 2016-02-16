#version 400
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 color[3];
in vec3 normal[3];
in vec2 texCoords[3];

out vec4 colorFrag;
out vec3 normalFrag;

void main() {
    if( texCoords[0].x > 0 &&
        texCoords[0].y > 0 &&
        texCoords[1].x > 0 &&
        texCoords[1].y > 0 &&
        texCoords[2].x > 0 &&
        texCoords[2].y > 0){

        colorFrag = color[0];
        gl_Position = gl_in[0].gl_Position;
        colorFrag = color[0];
        normalFrag = normal[0];
        EmitVertex();

        gl_Position = gl_in[1].gl_Position;
        colorFrag = color[1];
        normalFrag = normal[1];
        EmitVertex();

        gl_Position = gl_in[2].gl_Position;
        colorFrag = color[2];
        normalFrag = normal[2];
        EmitVertex();

        EndPrimitive();
    }
}
