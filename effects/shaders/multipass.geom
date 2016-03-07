#version 400
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 color[3];
in vec3 normal[3];
in vec2 texCoords_0[3];
in vec2 texCoords_1[3];
in vec2 texCoords_2[3];
in vec2 texCoords_3[3];

flat out int goodtriangle;
flat out int goodTex_0;
flat out int goodTex_1;
flat out int goodTex_2;
flat out int goodTex_3;

out vec4 colorFrag;
out vec3 normalFrag;
out vec2 texCoordsFrag_0;
out vec2 texCoordsFrag_1;
out vec2 texCoordsFrag_2;
out vec2 texCoordsFrag_3;

void main() {

    goodtriangle = 1;
    goodTex_0 = 1;
    goodTex_1 = 1;
    goodTex_2 = 1;
    goodTex_3 = 1;

    for(int i = 0; i < 3; i ++)
    {
        if(texCoords_0[i].x <= 0 || texCoords_0[i].y <= 0)
        {
            goodtriangle = 0;
            goodTex_0 = 0;
        };
        if(texCoords_1[i].x <= 0 || texCoords_1[i].y <= 0)
        {
            goodtriangle = 0;
            goodTex_1 = 0;
        };
        if(texCoords_2[i].x <= 0 || texCoords_2[i].y <= 0){
            goodtriangle = 0;
            goodTex_2 = 0;
        };
        if(texCoords_3[i].x <= 0 || texCoords_3[i].y <= 0){
            goodtriangle = 0;
            goodTex_3 = 0;
        };
    }

        gl_Position = gl_in[0].gl_Position;
        normalFrag = normal[0];
        texCoordsFrag_0 = texCoords_0[0];
        texCoordsFrag_1 = texCoords_1[0];
        texCoordsFrag_2 = texCoords_2[0];
        texCoordsFrag_3 = texCoords_3[0];
        EmitVertex();

        gl_Position = gl_in[1].gl_Position;
        normalFrag = normal[1];
        texCoordsFrag_0 = texCoords_0[1];
        texCoordsFrag_1 = texCoords_1[1];
        texCoordsFrag_2 = texCoords_2[1];
        texCoordsFrag_3 = texCoords_3[1];
        EmitVertex();

        gl_Position = gl_in[2].gl_Position;
        normalFrag = normal[2];
        texCoordsFrag_0 = texCoords_0[2];
        texCoordsFrag_1 = texCoords_1[2];
        texCoordsFrag_2 = texCoords_2[2];
        texCoordsFrag_3 = texCoords_3[2];
        EmitVertex();

        EndPrimitive();
}
