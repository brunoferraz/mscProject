#version 400
in vec4 colorFrag;
out vec4 out_Color;
flat in int goodtriangle;
void main(void)
{
    vec4 ncolor = vec4(0.0);
//    if(goodtriangle ==1)
//    {
        ncolor = colorFrag;
//    }

    out_Color = ncolor;
}
