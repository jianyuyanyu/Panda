////////////////////////////////////////////////////////////////////////////////
// Filename: basic.ps 
////////////////////////////////////////////////////////////////////////////////
#version 150


/////////////////////
// CONSTANTS       //
/////////////////////
// per frame
#define MAX_LIGHTS 10
uniform int numLights;
uniform struct Light
{
    vec4    lightPosition;
    vec4    lightColor;
    vec4    lightDirection;
    vec2    lightSize;
    float   lightIntensity;
    int     lightDistAttenCurveType;
    float   lightDistAttenCurveParams[5];
    int     lightAngleAttenCurveType;
    float   lightAngleAttenCurveParams[5];
} allLights[MAX_LIGHTS];

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

vec3 Illumination(Light l)
{
    // vertex normal
    vec3 N = normalize(normal.xyz);

    vec3 L = (viewMatrix * worldMatrix * l.lightPosition).xyz - v.xyz; // vector from vertex to light
    float distance = length(L);
    L = normalize(L);
    vec3 lightDir = normalize((viewMatrix * worldMatrix * l.lightDirection).xyz);
    float lightToSurfAngle = acos(dot(L, -lightDir));

    //angle attenuation
    float atten = ImplementAttenCurve(lightToSurfAngle, l.lightAngleAttenCurveType, l.lightAngleAttenCurveParams);

    // distance attenuation
    atten *= ImplementAttenCurve(distance, l.lightDistAttenCurveType, l.lightDistAttenCurveParams);

    vec3 R = normalize(2.0f * clamp(dot(L, N), 0.0f, 1.0f) * N - L);
    vec3 V = -normalize(v.xyz);

    if (usingDiffuseMap)
        return l.lightIntensity * atten * l.lightColor.rgb * (texture(diffuseMap, uv).rgb * clamp(dot(N, L), 0.0f, 1.0f) + specularColor.rgb * pow(clamp(dot(R, V), 0.0f, 1.0f), specularPower)); 
    else
        return l.lightIntensity * atten * l.lightColor.rgb * (diffuseColor.rgb * clamp(dot(N, L), 0.0f, 1.0f) + specularColor.rgb * pow(clamp(dot(R,V), 0.0f, 1.0f), specularPower)); 
}

vec3 ProjectOnPlane(vec3 point, vec3 centerOfPlane, vec3 normalOfPlane)
{
    return point - dot(point - centerOfPlane, normalOfPlane) * normalOfPlane;
}

bool IsAbovePlane(vec3 point, vec3 centerOfPlane, vec3 normalOfPlane)
{
    return dot(point - centerOfPlane, normalOfPlane) > 0.0f;
}

vec3 LinePlaneIntersect(vec3 lineStart, vec3 lineDir, vec3 centerOfPlane, vec3 normalOfPlane)
{
    return lineStart + lineDir * (dot(centerOfPlane - lineStart, normalOfPlane) / dot(lineDir, normalOfPlane));
}

vec3 IlluminationAreaLight(Light light)
{
    vec3 N = normalize(normal.xyz);
    vec3 right = normalize((viewMatrix * worldMatrix * vec4(1.0f, 0.0f, 0.0f, 0.0f)).xyz);  // light local space right axis to view space
    vec3 pnormal = normalize((viewMatrix * worldMatrix * light.lightDirection).xyz); // light local space direction to view space light direction
    vec3 ppos = (viewMatrix * worldMatrix * light.lightPosition).xyz; // light position in view space
    vec3 up = normalize(cross(pnormal, right)); // up vector in view space
    right = normalize(cross(up, pnormal)); // right vector in view space

    // width and height of the area light
    float width = light.lightSize.x;
    float height = light.lightSize.y;

    // project current vertex onto the light plane
    vec3 projection = ProjectOnPlane(v.xyz, ppos, pnormal);
    vec3 dir = projection - ppos;

    // calculate distance from area.
    vec2 diagonal = vec2(dot(dir, right), dot(dir,up));
    vec2 nearest2D = vec2(clamp(diagonal.x, -width, width), clamp(diagonal.y, -height, height));
    vec3 nearestPointInside = ppos + right * nearest2D.x + up * nearest2D.y;

    vec3 L = nearestPointInside - v.xyz;

    float lightToSurfDist = length(L);
    L = normalize(L);

    // distance attenuation
    float atten = ImplementAttenCurve(lightToSurfDist, light.lightDistAttenCurveType, light.lightDistAttenCurveParams);

    vec3 linearColor = vec3(0.0f);

    float pnDotL = dot(pnormal, -L);

    if (pnDotL > 0.0f && IsAbovePlane(v.xyz, ppos, pnormal))
    {
        // shoot a ray to calculate secpular
        vec3 V = normalize(-v.xyz);
        vec3 R = normalize(2.0f * dot(V, N) * N - V);
        vec3 R2 = normalize(2.0f * dot(L, N) * N - L);
        vec3 E = LinePlaneIntersect(v.xyz, R, ppos, pnormal);

        float specAngle = clamp(dot(-R, pnormal), 0.0f, 1.0f);
        vec3 dirSpec = E - ppos;
        vec2 dirSpec2D = vec2(dot(dirSpec, right), dot(dirSpec, up));
        vec2 nearestSpec2D = vec2(clamp(dirSpec2D.x, -width, width), clamp(dirSpec2D.y, -height, height));
        float specFactor = 1.0f - clamp(length(nearestSpec2D - dirSpec2D), 0.0f, 1.0f);

        if (usingDiffuseMap)
        {
            linearColor = ambientColor.rgb + light.lightIntensity * atten * light.lightColor.rgb * (texture(diffuseMap, uv).rgb * dot(N, L) * pnDotL + specularColor.rgb * pow(clamp(dot(R2, V), 0.0f, 1.0f), specularPower) * specFactor * specAngle);
        }
        else 
        {
            linearColor = ambientColor.rgb + light.lightIntensity * atten * light.lightColor.rgb * (diffuseColor.rgb * dot(N, L) * pnDotL + specularColor.rgb * pow(clamp(dot(R2, V), 0.0f, 1.0f), specularPower) * specFactor * specAngle); 
        }
    }
    //return vec3(lightToSurfDist / 10.0f);
    return linearColor;
}

vec3 ApplyPointLight(Light l)
{
	// change light position to view space
	vec3 lightPosInViewSpace = (viewMatrix * worldMatrix * l.lightPosition).xyz; // light position in view space
	
	// ambient 
	vec3 currentAmbient = ambientColor.rgb;
	
	// diffuse
    float distance = length(v.xyz - lightPosInViewSpace);
    float kq = l.lightDistAttenCurveParams[2];
    float kl = l.lightDistAttenCurveParams[3];
    float kc = l.lightDistAttenCurveParams[4];
    float atten = clamp(1.0 / (kq * distance * distance + kl * distance + kc), 0.0f, 1.0f);
	vec3 N = normalize(normal.xyz);
	vec3 L = normalize(lightPosInViewSpace - v.xyz);
	vec3 currentDiffuse = l.lightIntensity * atten * l.lightColor.rgb * diffuseColor.rgb * clamp(dot(N, L), 0.0f, 1.0f);
	
	// specular
	vec3 R = normalize(2.0f * clamp(dot(L, N), 0.0f, 1.0f) * N - L);
    vec3 V = -normalize(v.xyz);
	vec3 currentSpecular = l.lightIntensity * atten * l.lightColor.rgb * specularColor.rgb * pow(clamp(dot(R,V), 0.0f, 1.0f), specularPower);
	
	return currentAmbient + currentDiffuse + currentSpecular;
}

void main(void)
{
    vec3 ill = vec3(0);
    for (int i = 0; i < numLights; ++i)
    {
        if (allLights[i].lightSize.x > 0.0f && allLights[i].lightSize.y > 0.0f)
        {
            ill += IlluminationAreaLight(allLights[i]);
        }
        else 
        {
            ill += Illumination(allLights[i]);
        }
    }
	
	vec3 irr = ApplyPointLight(allLights[0]);

    // gamma correction
    outputColor = vec4(clamp(pow(irr, vec3(1.0f / 2.2f)), 0.0f, 1.0f), 1.0f);
}

