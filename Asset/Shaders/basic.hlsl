#include "cbuffer.h"
#include "vsoutput.hs"

v2p VSMain(a2v input) {
    v2p output;

	float4 temp = mul(m_viewMatrix, mul(m_worldMatrix, mul(objectMatrix, float4(input.Position.xyz, 1.0f))));
	output.vPosInView = temp;
	output.Position = mul(m_projectionMatrix, temp);
	float3 vN = mul(m_viewMatrix, mul(m_worldMatrix, mul(objectMatrix, float4(input.Normal, 0.0f)))).xyz;

	output.vNorm = vN;

	output.TextureUV.x = input.TextureUV.x;
	output.TextureUV.y = 1.0f - input.TextureUV.y;

	return output;
}

SamplerState samp0 : register(s0);
Texture2D colorMap : register(t0);
//Texture2D bumpGlossMap: register(t1);

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
	float atten = 1.0f;
	switch (attenType)
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
			float s = attenParams[0];
			float o = attenParams[1];
			float kl = attenParams[2];
			float kc = attenParams[3];
			atten = clamp(s / (kl * dist + kc * s) + o, 0.0f, 1.0f);
			break;
		}
		case 3: // inverse square
		{
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

float3 Illumination(v2p input, Light l)
{
    // vertex normal
    float3 N = normalize(input.vNorm);
    float3 L;// = (viewMatrix * worldMatrix * l.lightPosition).xyz - v.xyz; // vector from vertex to light
	float3 lightDir = normalize(mul(m_viewMatrix, mul(m_worldMatrix, l.m_lightDirection)).xyz);

	if (l.m_lightPosition.w == 0.0f)
		L = -lightDir;
	else
		L = mul(m_viewMatrix, mul(m_worldMatrix, l.m_lightPosition)).xyz - input.vPosInView.xyz;
    
	float distance = length(L);
    L = normalize(L);
    float lightToSurfAngle = acos(dot(L, -lightDir));

	// angle attenuation
	float attenParams[5];
	attenParams[0] = l.m_lightAngleAttenCurveParams_0;
	attenParams[1] = l.m_lightAngleAttenCurveParams_1;
	attenParams[2] = l.m_lightAngleAttenCurveParams_2;
	attenParams[3] = l.m_lightAngleAttenCurveParams_3;
	attenParams[4] = l.m_lightAngleAttenCurveParams_4;

    float atten = ImplementAttenCurve(lightToSurfAngle, l.m_lightAngleAttenCurveType, attenParams);

    // distance attenuation
	attenParams[0] = l.m_lightDistAttenCurveParams_0;
	attenParams[1] = l.m_lightDistAttenCurveParams_1;
	attenParams[2] = l.m_lightDistAttenCurveParams_2;
	attenParams[3] = l.m_lightDistAttenCurveParams_3;
	attenParams[4] = l.m_lightDistAttenCurveParams_4;
    atten *= ImplementAttenCurve(distance, l.m_lightDistAttenCurveType, attenParams);

    float3 R = normalize(2.0f * dot(L, N) * N - L);
    float3 V = normalize(-input.vPosInView.xyz);

    if (usingDiffuseMap)
        return l.m_lightIntensity * atten * l.m_lightColor.rgb * (colorMap.Sample(samp0, input.TextureUV).rgb * clamp(dot(N, L), 0.0f, 1.0f) + specularColor.rgb * pow(clamp(dot(R, V), 0.0f, 1.0f), specularPower)); 
    else
        return l.m_lightIntensity * atten * l.m_lightColor.rgb * (diffuseColor.rgb * clamp(dot(N, L), 0.0f, 1.0f) + specularColor.rgb * pow(clamp(dot(R,V), 0.0f, 1.0f), specularPower)); 
}

float3 projectOnPlane(float3 p, float3 center_of_plane, float3 normal_of_plane)
{
    return p - dot(p - center_of_plane, normal_of_plane) * normal_of_plane;
}

bool isAbovePlane(float3 p, float3 center_of_plane, float3 normal_of_plane)
{
    return dot(p - center_of_plane, normal_of_plane) > 0.0f;
}

float3 linePlaneIntersect(float3 line_start, float3 line_dir, float3 center_of_plane, float3 normal_of_plane)
{
    return line_start + line_dir * (dot(center_of_plane - line_start, normal_of_plane) / dot(line_dir, normal_of_plane));
}

float3 apply_areaLight(v2p input, Light light)
{
    float3 N = normalize(input.vNorm);
    float3 right = normalize(mul(m_viewMatrix, mul(m_worldMatrix, float4(1.0f, 0.0f, 0.0f, 0.0f))).xyz);
    float3 pnormal = normalize(mul(m_viewMatrix, mul(m_worldMatrix, light.m_lightDirection)).xyz);
    float3 ppos = mul(m_viewMatrix, mul(m_worldMatrix, light.m_lightPosition)).xyz;
    float3 up = normalize(cross(pnormal, right));
    right = normalize(cross(up, pnormal));

    //width and height of the area light:
    float width = light.m_lightSize.x;
    float height = light.m_lightSize.y;

    //project onto plane and calculate direction from center to the projection.
    float3 projection = projectOnPlane(input.vPosInView.xyz, ppos, pnormal);// projection in plane
    float3 dir = projection - ppos;

    //calculate distance from area:
    float2 diagonal = float2(dot(dir,right), dot(dir,up));
    float2 nearest2D = float2(clamp(diagonal.x, -width, width), clamp(diagonal.y, -height, height));
    float3 nearestPointInside = ppos + right * nearest2D.x + up * nearest2D.y;

    float3 L = nearestPointInside - input.vPosInView.xyz;

    float lightToSurfDist = length(L);
    L = normalize(L);

    // distance attenuation
    float atten_params[5];
    atten_params[0] = light.m_lightDistAttenCurveParams_0;
    atten_params[1] = light.m_lightDistAttenCurveParams_1;
    atten_params[2] = light.m_lightDistAttenCurveParams_2;
    atten_params[3] = light.m_lightDistAttenCurveParams_3;
    atten_params[4] = light.m_lightDistAttenCurveParams_4;
    float atten = ImplementAttenCurve(lightToSurfDist, light.m_lightDistAttenCurveType, atten_params);

    float3 linearColor = 0.0f;

    float pnDotL = dot(pnormal, -L);

    if (pnDotL > 0.0f && isAbovePlane(input.vPosInView.xyz, ppos, pnormal)) //looking at the plane
    {
        //shoot a ray to calculate specular:
        float3 V = normalize(-input.vPosInView.xyz);
        float3 R = normalize(2.0f * dot(V, N) *  N - V);
        float3 R2 = normalize(2.0f * dot(L, N) *  N - L);
        float3 E = linePlaneIntersect(input.vPosInView.xyz, R, ppos, pnormal);

        float specAngle = clamp(dot(-R, pnormal), 0.0f, 1.0f);
        float3 dirSpec = E - ppos;
        float2 dirSpec2D = float2(dot(dirSpec, right), dot(dirSpec, up));
        float2 nearestSpec2D = float2(clamp(dirSpec2D.x, -width, width), clamp(dirSpec2D.y, -height, height));
        float specFactor = 1.0f - clamp(length(nearestSpec2D - dirSpec2D), 0.0f, 1.0f);

        if (usingDiffuseMap)
        {
            linearColor = ambientColor.rgb + light.m_lightIntensity * atten * light.m_lightColor.rgb * (colorMap.Sample(samp0, input.TextureUV).rgb * dot(N, L) * pnDotL + specularColor.rgb * pow(clamp(dot(R2, V), 0.0f, 1.0f), specularPower) * specFactor * specAngle); 
        }
        else
        {
            linearColor = ambientColor.rgb + light.m_lightIntensity * atten * light.m_lightColor.rgb * (diffuseColor.rgb * dot(N, L) * pnDotL + specularColor.rgb * pow(clamp(dot(R2, V), 0.0f, 1.0f), specularPower) * specFactor * specAngle); 
        }
    }

    return linearColor;
}

float4 PSMain(v2p input) : SV_TARGET
{
	float3 linearColor = 0.0f;
	for (uint i = 0; i < numLights; ++i)
	{
		if (m_lights[i].m_lightSize.x > 0.0f && m_lights[i].m_lightSize.y > 0.0f)
		{
			linearColor += apply_areaLight(input, m_lights[i]);
		}
		else 
		{
			linearColor += Illumination(input, m_lights[i]);
		}
	}
	return float4(pow(clamp(linearColor, 0.0f, 1.0f), 1.0f/2.2f), 1.0f);
}

