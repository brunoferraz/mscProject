#version 400

in vec4 colorFrag;
in vec3 normalFrag;
in vec4 vert;

in vec2 texCoordsFrag;

in float depth;

flat in int goodtriangle;

out vec4 out_Color;

uniform mat4 lightViewMatrix;

uniform float numImages;
uniform bool firstPass;
uniform bool lastPass;
uniform bool multiPass;
uniform vec2 viewportSize;

uniform sampler2D lastPassTexture;

void main(void)
{
    vec3 normal = normalFrag;
    vec2 coord  = vec2(gl_FragCoord.x/ viewportSize.x, gl_FragCoord.y/ viewportSize.y);
    vec4 ncolor  = texture2D(lastPassTexture, coord);

    if(multiPass && goodtriangle == 1){
        ncolor.rgb += colorFrag.rgb * colorFrag.a;
        ncolor.a   += colorFrag.a;
    }

    if(lastPass){
        if(ncolor.a != 0){
            ncolor /= ncolor.a;
            ncolor.a = 1.0;
        }

        vec3 lightDirection = (lightViewMatrix * vec4(0.0, 0.0, 1.0, 0.0)).xyz;
        lightDirection = normalize(lightDirection);

        vec3 lightReflection = reflect(-lightDirection, normal);
        vec3 eyeDirection = -normalize(vert.xyz);
        float shininess = 100.0;

        vec4 ambientLight = ncolor * 0.5;
        vec4 diffuseLight = ncolor * 0.4 * max(dot(lightDirection, normal),0.0);
        vec4 specularLight = vec4(1.0) *  max(pow(dot(lightReflection, eyeDirection), shininess),0.0);

        ncolor = vec4(ambientLight.xyz + diffuseLight.xyz + specularLight.xyz, 1.0);
    }
    out_Color = ncolor;
//    out_Color = vec4(1.0, 0.0, 0.0, 1.0);
}
