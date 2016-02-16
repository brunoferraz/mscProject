#version 400
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 colorVert[3];
in vec3 normalVert[3];

out vec4 colorFrag;
out vec3 normalFrag;

void main() {
    colorFrag = colorVert[0];

    gl_Position = gl_in[0].gl_Position;
    colorFrag = colorVert[0];
    normalFrag = normalVert[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    colorFrag = colorVert[1];
    normalFrag = normalVert[1];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    colorFrag = colorVert[2];
    normalFrag = normalVert[2];
    EmitVertex();

    EndPrimitive();
}
