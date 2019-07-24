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
	// uint temp = 4;
	// if (temp == 0)
	// 	return float3(l.m_lightDistAttenCurveParams_0, l.m_lightDistAttenCurveParams_0, l.m_lightDistAttenCurveParams_0);
	// else if (temp == 1)
	// 	return float3(l.m_lightDistAttenCurveParams_1, l.m_lightDistAttenCurveParams_1, l.m_lightDistAttenCurveParams_1);
	// else if (temp == 2)
	// 	return float3(l.m_lightDistAttenCurveParams_2, l.m_lightDistAttenCurveParams_2, l.m_lightDistAttenCurveParams_2);
	// else if (temp == 3)
	// 	return float3(l.m_lightDistAttenCurveParams_3, l.m_lightDistAttenCurveParams_3, l.m_lightDistAttenCurveParams_3);
	// else if (temp == 4)
	// 	return float3(l.m_lightDistAttenCurveParams_4, l.m_lightDistAttenCurveParams_4, l.m_lightDistAttenCurveParams_4);

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

float4 PSMain(v2p input) : SV_TARGET
{
	float3 linearColor = 0.0f;
	for (uint i = 0; i < numLights; ++i)
		linearColor += Illumination(input, m_lights[i]);
	linearColor += ambientColor.rgb;
	// return m_lights[2].m_lightColor;
	// return normalize(input.vPosInView);
	// if (usingDiffuseMap)
	// 	return float4(1.0f, 0.0f, 0.0f, 1.0f);
	// else
	// 	return float4(m_lights[0].m_lightPosition.w, m_lights[0].m_lightPosition.w, m_lights[0].m_lightPosition.w, 1.0f);
	return float4(linearColor, 1.0f);
}

