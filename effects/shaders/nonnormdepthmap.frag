#version 400
out vec4 out_Color;
in float depth;

void main(void)
{
    out_Color = vec4(depth, depth, depth, 1.0);
}
