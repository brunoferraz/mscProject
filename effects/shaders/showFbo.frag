#version 130

in vec4 color;
in vec3 normal;
in vec4 vert;

in vec2 texCoords;
in float depth;

out vec4 out_Color;

uniform mat4 lightViewMatrix;
uniform sampler2D imageTexture;
uniform vec2 in_Viewport;

void main(void)
{
    vec2 texCoord = vec2(gl_FragCoord.xy/in_Viewport);
    vec3 result = texture2D(imageTexture, texCoord).rgb;
//    gl_FragColor = vec4(result,1.0);
//    out_Color = vec4(1.0);
    out_Color = vec4(result,1.0);
}
