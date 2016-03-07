#version 400
in vec3 normalFrag;
in vec4 vert;

in vec2 texCoordsFrag_0;
in vec2 texCoordsFrag_1;
in vec2 texCoordsFrag_2;
in vec2 texCoordsFrag_3;

in float depth;

flat in int goodtriangle;
flat in int goodTex_0;
flat in int goodTex_1;
flat in int goodTex_2;
flat in int goodTex_3;

out vec4 out_Color;

uniform mat4 lightViewMatrix;

uniform float numImages;
uniform bool firstPass;
uniform bool lastPass;
uniform bool multiPass;
uniform vec2 viewportSize;

uniform sampler2D imageTexture_0;
uniform sampler2D imageTexture_1;
uniform sampler2D imageTexture_2;
uniform sampler2D imageTexture_3;

uniform sampler2D prevPassTexture;

vec4 sampleColor (in sampler2D texture, vec2 coords)
{
    vec4 c = vec4(0.0);
    if (coords.x > 0 && coords.y > 0)
        c = texture2D(texture, coords);
    return c;
}

void main(void)
{
    vec3 normal = normalFrag;
    vec2 prevPassCoord  = vec2(gl_FragCoord.x/ viewportSize.x, gl_FragCoord.y/ viewportSize.y);
    vec4 prevPassColor  = texture2D(prevPassTexture, prevPassCoord);
    vec4 nColor = vec4(0);

      nColor = prevPassColor;
//    if(goodtriangle == 1){
      if(goodTex_0 == 1){
        nColor += sampleColor(imageTexture_0, texCoordsFrag_0);
      }
      if(goodTex_1 == 1){
        nColor += sampleColor(imageTexture_1, texCoordsFrag_1);
      }
      if(goodTex_2 == 1){
        nColor += sampleColor(imageTexture_2, texCoordsFrag_2);
      }
      if(goodTex_3 == 1){
        nColor += sampleColor(imageTexture_3, texCoordsFrag_3);
      }
//        prevPassColor.rgb += colorFrag.rgb * colorFrag.a;
//    }

    if(lastPass){
        nColor /= nColor.a;

//        vec3 lightDirection = (lightViewMatrix * vec4(0.0, 0.0, 1.0, 0.0)).xyz;
//        lightDirection = normalize(lightDirection);

//        vec3 lightReflection = reflect(-lightDirection, normal);
//        vec3 eyeDirection = -normalize(vert.xyz);
//        float shininess = 100.0;

//        vec4 ambientLight = nColor * 0.5;
//        vec4 diffuseLight = nColor * 0.4 * max(dot(lightDirection, normal),0.0);
//        vec4 specularLight = vec4(1.0) *  max(pow(dot(lightReflection, eyeDirection), shininess),0.0);

//        nColor = vec4(ambientLight.xyz + diffuseLight.xyz + specularLight.xyz, 1.0);
    }
    out_Color = nColor;

}
