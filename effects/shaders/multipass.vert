#version 400
in vec4 in_Position;
in vec3 in_Normal;

in vec2 in_coordText_0;
in vec2 in_coordText_1;
in vec2 in_coordText_2;
in vec2 in_coordText_3;
in vec2 in_coordText_4;
in vec2 in_coordText_5;
in vec2 in_coordText_6;
in vec2 in_coordText_7;
in vec2 in_coordText_8;
in vec2 in_coordText_9;

in vec2 in_TexCoords;
in vec4 in_Color;
//in vec4 nColor;

out vec4 color;
out vec3 normal;
out vec4 vert;
out vec2 texCoords_0;
out vec2 texCoords_1;
out vec2 texCoords_2;
out vec2 texCoords_3;

out float depth;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float numImages;

//uniform vec4 default_color;

// if attribute in_Color exists or not
//uniform bool has_color;
//uniform bool firstPass;
//uniform bool lastPass;

//uniform sampler2D imageTexture_0;
//uniform sampler2D imageTexture_1;
//uniform sampler2D imageTexture_2;
//uniform sampler2D imageTexture_3;
uniform sampler2D imageTexture_4;
uniform sampler2D imageTexture_5;
uniform sampler2D imageTexture_6;
uniform sampler2D imageTexture_7;
uniform sampler2D imageTexture_8;
uniform sampler2D imageTexture_9;

float near = 0.1;
float far = 10000.0;

//vec4 sampleColor (in sampler2D texture, vec2 coords)
//{
//    vec4 c = vec4(0.0);
//    if (coords.x > 0 && coords.y > 0)
//        c = texture2D(texture, coords);
//    return c;
//}

void main(void)
{
        mat4 modelViewMatrix = viewMatrix * modelMatrix;

        mat4 normalMatrix = transpose(inverse(modelViewMatrix));
        normal = normalize(vec3(normalMatrix * vec4(in_Normal,0.0)).xyz);

        vert = modelViewMatrix * in_Position;

        depth = in_Position.z;

        gl_Position = (projectionMatrix * modelViewMatrix) * in_Position;
//        color = vec4(0.0);


        texCoords_0 = in_coordText_0.xy;
        texCoords_1 = in_coordText_1.xy;
        texCoords_2 = in_coordText_2.xy;
        texCoords_3 = in_coordText_3.xy;

//        color += sampleColor(imageTexture_0, in_coordText_0);
//        color += sampleColor(imageTexture_1, in_coordText_1);
//        color += sampleColor(imageTexture_2, in_coordText_2);
//        color += sampleColor(imageTexture_3, in_coordText_3);
//        color += sampleColor(imageTexture_4, in_coordText_4);
//        color += sampleColor(imageTexture_5, in_coordText_5);
//        color += sampleColor(imageTexture_6, in_coordText_6);
//        color += sampleColor(imageTexture_7, in_coordText_7);
//        color += sampleColor(imageTexture_8, in_coordText_8);
//        color += sampleColor(imageTexture_9, in_coordText_9);



}
