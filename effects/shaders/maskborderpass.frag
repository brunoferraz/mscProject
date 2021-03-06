#version 400
out vec4 out_Border;

in float depth;
in vec3 normal;
in vec4 vert;

uniform vec2 viewportSize;
uniform sampler2D depthMap;



void main(void)
{
    vec2 coord = vec2(gl_FragCoord.x/viewportSize.x, gl_FragCoord.y/viewportSize.y);

    float stepX = (1 * 1)/viewportSize.x;
    float stepY = (1 * 1)/viewportSize.y;

    float c1 = texture2D(depthMap, vec2(coord.x-stepX,  coord.y-stepY)).x;
    float c2 = texture2D(depthMap, vec2(coord.x,        coord.y -stepY)).x;
    float c3 = texture2D(depthMap, vec2(coord.x+stepX,  coord.y -stepY)).x;
    float c4 = texture2D(depthMap, vec2(coord.x-stepX,  coord.y)).x;
    float c5 = texture2D(depthMap, vec2(coord.x,        coord.y)).x;
    float c6 = texture2D(depthMap, vec2(coord.x +stepX, coord.y)).x;
    float c7 = texture2D(depthMap, vec2(coord.x -stepX, coord.y+ stepY)).x;
    float c8 = texture2D(depthMap, vec2(coord.x,        coord.y + stepY)).x;
    float c9 = texture2D(depthMap, vec2(coord.x +stepX, coord.y + stepY)).x;

//    SOBEL
//    float f1 = 1.0;
//    float f2 = 2.0;
//    float sX = (c1 * -f1) + (c4 * -f2) + (c7 * -f1) + (c3 * f1) + (c6 * f2) + (c9 * f1);
//    float sY = (c1 * +f1) + (c2 * +f2) + (c3 * +f1) + (c7 * -f1) + (c8 * -f2) + (c9 *-f1);
//    float sXY = sqrt(pow(sX, 2) + pow(sY, 2));
//    float threshold = 0.002;
//    float border = 0;
//    if(sXY > threshold) border = 1;


//  LAPLACIANO
//    float f1 = 1.0;
//    float f2 = -4.0;
//    float sXY = (c1 * 0) + (c2 * f1) + (c3 * 0) + (c4 * f1) + (c5 * f2) + (c6 *f1) + (c7 * 0) + (c8 * f1) + (c9 * 0);
//    float threshold = 0.00005;
//    float border = 0;
//    if(sXY > threshold) border = 1;

//    LAPLACIANO II
    float f1 = 1.0;
    float f2 = -8.0;
    float sXY = (c1 * f1) + (c2 * f1) + (c3 * f1) +
                (c4 * f1) + (c5 * f2) + (c6 *f1) +
                (c7 * f1) + (c8 * f1) + (c9 * f1);
    float threshold = 0.00005;
    float border = 0;
    if(sXY > threshold) border = 1;
    vec4 finalColor = vec4(border);

//  COMPOSTO
//    float f1 = 1.0;
//    float f2 = -8.0;
//    float sXY = (c1 * f1)     + (c2 * f1 * 2) + (c3 * f1) +
//                (c4 * f1 * 2) + (c5 * f2 * 4) + (c6 * f1 * 2) +
//                (c7 * f1)     + (c8 * f1 * 2) + (c9 * f1);
//    sXY /= 16;
//    float threshold = 0.00000000001;
//    float border = 0;
//    if(sXY > threshold) border = 1;

//  COMPOSTO II
//    float f1 = 1.0;
//    float f2 = 4.0;
//    float sXY = (c1 * f1 * 0)   + (c2 * f1 * 2) + (c3 * f1 * 0) +
//                (c4 * f1 * 2)   + (c5 * f2 * 4) + (c6 * f1 * 2) +
//                (c7 * f1 * 0)   + (c8 * f1 * 2) + (c9 * f1 * 0);
//    sXY /= 16;
//    float threshold = 0.22;
//    float border = 0;
//    if(sXY > threshold) border = 1;

//    border  = sXY;

//  GAUSSIANO
//    float sXY = (c1)+ (c2 * 2) + (c3) +
//                (c4 * 2) + (c5 * 4) + (c6 * 2) +
//                (c7)+ (c8 * 2) + (c9);
//    sXY /= 16;
////    float threshold = 0.00005;
////    float border = 0;
////    if(sXY > threshold) border = 1;
//    float border = sXY;

//LAPLACIANO COM KERNEL DE TAMANHO VARIAVEL

//    vec2 center = gl_FragCoord.xy;
//    vec4 finalColor = vec4(1.0);
//    int step = 1;
//    ivec2 coord[25];
//    vec4 l[25];
//    int size = 3;
//    int counter = 0;
//    for(int j = -step * size; j <= step * size; j+= step)
//    {
//        for(int i = -step * size; i <= step * size; i+= step)
//        {
//            coord[counter] = ivec2(center.x + step, center.y + step);
//            counter++;
//        }
//    }
//    for (int i = 0; i < 25; ++i)
//    {
//        if (coord[i].x >= 0 && coord[i].y >= 0 && coord[i].x < viewportSize.x && coord[i].y < viewportSize.y)
//        {
//            l[i] = texelFetch(depthMap, coord[i], 0);
//        }
//        else
//        {
//            l[i] = vec4(1.0);
//        }
//    }
//    float k[9] = float[9](  1, 2, 1,
//                            2, 4, 2,
//                            1, 2, 1);
//    float k5[25] = float[25](2, 4,  5,  4,  2,
//                             4, 9,  12, 9,  4,
//                             5, 12, 15, 12, 5,
//                             4, 9,  12, 9,  4,
//                             2, 4,  5,  4,  2);

//    for (int i = 0; i < 25; ++i)
//    {
//        finalColor += l[i] * k5[i];
//    }
//    finalColor /= 115;


    out_Border = finalColor;
//    out_Border = vec4(1.0, 0.0, 0.0, 1.0);
}
