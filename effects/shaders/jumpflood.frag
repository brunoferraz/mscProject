#version 400
out vec4 out_Border;

in float depth;
in vec3 normal;
in vec4 vert;

uniform ivec2 viewportSize;
uniform sampler2D map;


uniform float step;
uniform bool lastPass;
uniform bool firstPass;

void main(void)
{
    vec2 center = gl_FragCoord.xy;
    vec4 finalColor = vec4(0.0);

    if(firstPass)
    {
         if(texelFetch(map, ivec2(center.xy), 0).w == 1.0)
        {
            finalColor = vec4(center.x, center.y, 0.0, 1.0);
        }
    }
    else
    {
//    vec4 c1 = texture2D(map, vec2(coord.x -step, coord.y-step));
//    vec4 c2 = texture2D(map, vec2(coord.x, coord.y -step));
//    vec4 c3 = texture2D(map, vec2(coord.x +step, coord.y -step));
//    vec4 c4 = texture2D(map, vec2(coord.x -step, coord.y));
//    vec4 c5 = texture2D(map, vec2(coord.x, coord.y));
//    vec4 c6 = texture2D(map, vec2(coord.x + step, coord.y));
//    vec4 c7 = texture2D(map, vec2(coord.x - step, coord.y+ step));
//    vec4 c8 = texture2D(map, vec2(coord.x, coord.y + step));
//    vec4 c9 = texture2D(map, vec2(coord.x + step, coord.y + step));
        float size = 9;
        vec4 l[9];
        ivec2 coord[9];
        coord[0] = ivec2(center.x - step, center.y - step);
        coord[1] = ivec2(center.x       , center.y - step);
        coord[2] = ivec2(center.x + step, center.y - step);
        coord[3] = ivec2(center.x - step, center.y);
        coord[4] = ivec2(center.x       , center.y);
        coord[5] = ivec2(center.x + step, center.y);
        coord[6] = ivec2(center.x - step, center.y + step);
        coord[7] = ivec2(center.x       , center.y + step);
        coord[8] = ivec2(center.x + step, center.y + step);

        for (int i = 0; i < 9; ++i)
        {
            if (coord[i].x >= 0 && coord[i].y >= 0 && coord[i].x < viewportSize.x && coord[i].y < viewportSize.y)
            {
                l[i] = texelFetch(map, coord[i], 0);
            }
            else
            {
                l[i] = vec4(0.0);
            }
        }

//    l[0] = texelFetch(map, vec2(coord.x - step, coord.y-step), 0);
//    l[1] = texelFetch(map, vec2(coord.x, coord.y -step),0);
//    l[2] = texelFetch(map, vec2(coord.x +step, coord.y -step),0);
//    l[3] = texelFetch(map, vec2(coord.x -step, coord.y),0);
//    l[4] = texelFetch(map, vec2(coord.x, coord.y),0);
//    l[5] = texelFetch(map, vec2(coord.x + step, coord.y),0);
//    l[6] = texelFetch(map, vec2(coord.x - step, coord.y+ step),0);
//    l[7] = texelFetch(map, vec2(coord.x, coord.y + step),0);
//    l[8] = texelFetch(map, vec2(coord.x + step, coord.y + step),0);

        float bestDist = sqrt(pow(viewportSize.x,2)+ pow(viewportSize.y,2));
        int bestJump = -1;
        for(int i = 0; i < size; i++)
        {
            if (l[i].w == 1.0)
            {
                float distance = sqrt(pow((center.x - l[i].x),2) + pow((center.y - l[i].y),2));
                if (distance < bestDist)
                {
                    bestDist = distance;
                    bestJump = i;
                }
            }
        }
        if (bestJump != -1)
        {
            finalColor = vec4(l[bestJump].xy, bestDist, 1.0);
        }
    }
    if(lastPass)
    {
            finalColor.z /= sqrt(pow(viewportSize.x,2)+ pow(viewportSize.y,2));
            finalColor.z = clamp(finalColor.z,0, 1);
            finalColor = vec4(vec3(finalColor.z), 1.0);

    }

    out_Border = finalColor;
}
