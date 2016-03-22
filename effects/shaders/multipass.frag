#version 400
in vec3 normalFrag;
in vec4 vert;

in vec2 texCoordsFrag_0;
in vec2 texCoordsFrag_1;
in vec2 texCoordsFrag_2;
in vec2 texCoordsFrag_3;
in vec2 texCoordsFrag_4;

in float depth;

flat in int goodtriangle;
flat in int goodTex_0;
flat in int goodTex_1;
flat in int goodTex_2;
flat in int goodTex_3;
flat in int goodTex_4;

out vec4 out_Color;

uniform mat4 lightViewMatrix;

uniform float numImages;
uniform bool firstPass;
uniform bool lastPass;
uniform bool multiPass;
uniform vec2 viewportSize;

uniform sampler2D mask_0;
uniform sampler2D mask_1;
uniform sampler2D mask_2;
uniform sampler2D mask_3;
uniform sampler2D mask_4;

uniform sampler2D imageTexture_0;
uniform sampler2D imageTexture_1;
uniform sampler2D imageTexture_2;
uniform sampler2D imageTexture_3;
uniform sampler2D imageTexture_4;

uniform float distWeight_0;
uniform float distWeight_1;
uniform float distWeight_2;
uniform float distWeight_3;
uniform float distWeight_4;

uniform float angleWeight_0;

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
    vec4 mask;
      nColor = prevPassColor;
//    if(goodtriangle == 1){
      if(goodTex_0 == 1){
        mask = texture2D(mask_0, texCoordsFrag_0);
        nColor.rgb += sampleColor(imageTexture_0, texCoordsFrag_0).rgb * mask.r;
        nColor.a += mask.r;
        nColor *= distWeight_0;
//        nColor.rgb += nColor.rgb *  distWeight_0;
//        nColor.a += distWeight_0;
      }
      if(goodTex_1 == 1){
        mask = texture2D(mask_1, texCoordsFrag_1);
        nColor.rgb += sampleColor(imageTexture_1, texCoordsFrag_1).rgb * mask.r;
        nColor.a += mask.r;
        nColor *= distWeight_1;
//        nColor.rgb += nColor.rgb *  distWeight_1;
//        nColor.a += distWeight_1;
      }
      if(goodTex_2 == 1){
          mask = texture2D(mask_2, texCoordsFrag_2);
          nColor.rgb += sampleColor(imageTexture_2, texCoordsFrag_2).rgb * mask.r;
          nColor.a += mask.r;
          nColor *= distWeight_2;
//          nColor.rgb += nColor.rgb *  distWeight_2;
//          nColor.a += distWeight_2;
      }
      if(goodTex_3 == 1){
          mask = texture2D(mask_3, texCoordsFrag_3);
          nColor.rgb += sampleColor(imageTexture_3, texCoordsFrag_3).rgb * mask.r;
          nColor.a += mask.r;
          nColor *= distWeight_3;
//          nColor.rgb += nColor.rgb *  distWeight_3;
//          nColor.a += distWeight_3;
      }
      if(goodTex_4 == 1){
          mask = texture2D(mask_4, texCoordsFrag_4);
          nColor.rgb += sampleColor(imageTexture_4, texCoordsFrag_4).rgb * mask.r;
          nColor.a += mask.r;
          nColor *= distWeight_4;
//          nColor.rgb += nColor.rgb *  distWeight_4;
//          nColor.a += distWeight_4;
      }

//        prevPassColor.rgb += colorFrag.rgb * colorFrag.a;
//    }

    if(lastPass){
        nColor /= nColor.a;

        vec3 lightDirection = (lightViewMatrix * vec4(0.0, 0.0, 1.0, 0.0)).xyz;
        lightDirection = normalize(lightDirection);

        vec3 lightReflection = reflect(-lightDirection, normal);
        vec3 eyeDirection = -normalize(vert.xyz);
        float shininess = 100.0;

        vec4 ambientLight = nColor * 0.5;
        vec4 diffuseLight = nColor * 0.4 * max(dot(lightDirection, normal),0.0);
        vec4 specularLight = vec4(1.0) *  max(pow(dot(lightReflection, eyeDirection), shininess),0.0);

        nColor = vec4(ambientLight.xyz + diffuseLight.xyz + specularLight.xyz, 1.0);
    }
    out_Color = nColor;
//    out_Color = vec4(0.5);

}
