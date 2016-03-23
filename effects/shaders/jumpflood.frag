#version 400
out vec4 out_Border;

in float depth;
in vec3 normal;
in vec4 vert;

uniform vec2 viewportSize;
uniform sampler2D map;


uniform float lengthStep;
uniform bool lastPass;
uniform bool firstPass;

float threshold = 0.0020;

void main(void)
{
    vec2 coord = vec2(gl_FragCoord.x/viewportSize.x, gl_FragCoord.y/viewportSize.y);

//    float stepX = (50 * 1)/viewportSize.x;
//    float stepY = (50 * 1)/viewportSize.y;
    vec2 a = vec2(viewportSize.x / lengthStep, viewportSize.y / lengthStep);

//    float stepX = 1.0 / lengthStep;
//    float stepY = 1.0 / lengthStep;
      float stepX = (a.x - viewportSize.x)/viewportSize.x;
      float stepY = (a.y - viewportSize.y)/viewportSize.y;

//    vec4 c1 = texture2D(map, vec2(coord.x -stepX, coord.y-stepY));
//    vec4 c2 = texture2D(map, vec2(coord.x, coord.y -stepY));
//    vec4 c3 = texture2D(map, vec2(coord.x +stepX, coord.y -stepY));
//    vec4 c4 = texture2D(map, vec2(coord.x -stepX, coord.y));
//    vec4 c5 = texture2D(map, vec2(coord.x, coord.y));
//    vec4 c6 = texture2D(map, vec2(coord.x + stepX, coord.y));
//    vec4 c7 = texture2D(map, vec2(coord.x - stepX, coord.y+ stepY));
//    vec4 c8 = texture2D(map, vec2(coord.x, coord.y + stepY));
//    vec4 c9 = texture2D(map, vec2(coord.x + stepX, coord.y + stepY));
    float size = 9;
    vec4 l[9];
    l[0] = texture2D(map, vec2(coord.x -stepX, coord.y-stepY));
    l[1] = texture2D(map, vec2(coord.x, coord.y -stepY));
    l[2] = texture2D(map, vec2(coord.x +stepX, coord.y -stepY));
    l[3] = texture2D(map, vec2(coord.x -stepX, coord.y));
    l[4] = texture2D(map, vec2(coord.x, coord.y));
    l[5] = texture2D(map, vec2(coord.x + stepX, coord.y));
    l[6] = texture2D(map, vec2(coord.x - stepX, coord.y+ stepY));
    l[7] = texture2D(map, vec2(coord.x, coord.y + stepY));
    l[8] = texture2D(map, vec2(coord.x + stepX, coord.y + stepY));

    vec4 finalColor = vec4(0.0, 0.0, sqrt(pow(viewportSize.x,2)+ pow(viewportSize.y,2)), 1.0);
    if(firstPass){
        if(l[4].x == 1.0){
            finalColor = vec4(coord.x, coord.y, 0.0, 1.0);
        }
    }else{
        vec4 bestVec = l[4];
        if(l[4].z != 0 && l[4].x == 0.0 && l[4].y==0.0){
            bestVec = vec4(0.0, 0.0, sqrt(pow(viewportSize.x,2)+ pow(viewportSize.y,2)), 1.0);
            for(int i = 0; i < size; i++)
            {
                if(l[i].z < bestVec.z) bestVec = l[i];
            }
            float distance = sqrt(pow((coord.x - bestVec.x),2) + pow((coord.y - bestVec.y),2));
            bestVec.z = distance;
        }
        finalColor = bestVec;
    }
    if(lastPass)
    {
        finalColor = vec4(vec3(finalColor.z), 1.0);
    }

    out_Border = finalColor;
}
