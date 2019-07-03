////////////////////////////////////////////////////////////////////////////////
// Filename: basic.ps 
////////////////////////////////////////////////////////////////////////////////
#version 150


/////////////////////
// CONSTANTS       //
/////////////////////
// per frame
uniform vec4    lightPosition;
uniform vec4    lightColor;
uniform vec3    lightDirection;
uniform float   lightIntensity;
uniform int     lightDistAttenCurveType;
uniform float   lightDistAttenCurveParams[5];
uniform int     lightAngleAttenCurveType;
uniform float   lightAngleAttenCurveParams[5];

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

// per drawcall
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float specularPower;

uniform bool usingDiffuseMap;
uniform bool usingNormalMap;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

/////////////////////
// INPUT VARIABLES //
/////////////////////
in vec4 normal;
in vec4 v; 
in vec2 uv;

//////////////////////
// OUTPUT VARIABLES //
//////////////////////
out vec4 outputColor;

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float LinearInterpolate(float t, float b, float e)
{
    if (t < b)
        return 1.0f;
    else if (t > e)
        return 0.0f;
    else
        return (e - t) / (e - b);
}

float ImplementAttenCurve(float dist, int attenType, float attenParams[5])
{
    float atten;
    switch(attenType)
    {
        case 0: // linear
        {
            float b = attenParams[0];
            float e = attenParams[1];
            atten = LinearInterpolate(dist, b, e);
            break;
        }
        case 1: // smooth
        {
            float b = attenParams[0];
            float e = attenParams[1];
            float at = LinearInterpolate(dist, b, e);
            atten = 3 * at * at - 2 * at * at * at;
            break;
        }
        case 2: // inverse
        {
            // float scale; float offset; float kl; float kc;
            float s = attenParams[0];
            float o = attenParams[1];
            float kl = attenParams[2];
            float kc = attenParams[3];
            atten = clamp(s / (kl * dist + kc * s) + o, 0.0f, 1.0f);
            break;
        }
        case 3: // inverse square
        {
            // float scale; float offset; float kq; float kl; float kc;
            float s = attenParams[0];
            float o = attenParams[1];
            float kq = attenParams[2];
            float kl = attenParams[3];
            float kc = attenParams[4];
            atten = clamp(s * s / (kq * dist * dist + kl * s * dist + kc * s * s) + o, 0.0f, 1.0f);
            break;
        }
    }

    return atten;
}

void main(void)
{
    // vertex normal
    vec3 N = normalize(normal.xyz);

    vec3 L = (viewMatrix * worldMatrix * lightPosition).xyz - v.xyz; // vector from vertex to light
    float distance = length(L);
    L = normalize(L);
    vec3 lightDir = normalize((viewMatrix * worldMatrix * vec4(lightDirection, 0.0f)).xyz);
    float lightToSurfAngle = acos(dot(L, lightDir));

    //angle attenuation
    float atten = ImplementAttenCurve(lightToSurfAngle, lightAngleAttenCurveType, lightAngleAttenCurveParams);

    // distance attenuation
    atten *= ImplementAttenCurve(distance, lightDistAttenCurveType, lightDistAttenCurveParams);

    vec3 R = normalize(2.0f * clamp(dot(L, N), 0.0f, 1.0f) * N - L);
    vec3 V = -normalize(v.xyz);

    if (usingDiffuseMap)
        outputColor = vec4(ambientColor.rgb + lightIntensity * atten * lightColor.rgb * (texture(diffuseMap, uv).rgb * clamp(dot(N, L), 0.0f, 1.0f) + specularColor.rgb * pow(clamp(dot(R, V), 0.0f, 1.0f), specularPower)), 1.0f); 
    else
        outputColor = vec4(ambientColor.rgb + lightIntensity * atten * lightColor.rgb * (diffuseColor.rgb * clamp(dot(N, L), 0.0f, 1.0f) + specularColor.rgb * pow(clamp(dot(R,V), 0.0f, 1.0f), specularPower)), 1.0f); 
}

