#pragma target 3.0
#include "noiseSimplex.cginc"

cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;

	float4 Color;
	float3 LightPos;
}

struct VS_INPUT
{
	float4 Pos   : POSITION;
	float4 Norm  : NORMAL;
	float2 tex   : TEXCOORD;
};

struct PS_INPUT
{
	float4 Pos   : SV_POSITION;
	float4 color : COLOR;
	float  noise : FLOAT;
	float2 tex   : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT) 0;
	output.noise = snoise(input.Pos);
	float len = length(input.Pos);

	if (len >= 10)
	{
		input.Pos.y -= (len - 10) / 7;
		input.Pos *= 2;
	}

	output.Pos = mul(input.Pos, World);
	output.Pos += float4(0.0f, output.noise * len / 2, 0.0f, 0.0f);

	float3 lightDir = LightPos - output.Pos.xyz;
	len = sqrt(lightDir.x * lightDir.x + lightDir.y * lightDir.y + lightDir.z * lightDir.z);
	lightDir = normalize(lightDir);

	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	float lightStrength = 20;
	output.color = Color * 0.1 + Color * lightStrength / len * (1.0f - mul(input.Norm.xyz, lightDir));

	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	return input.color * (1.0f + input.noise) / 2;
}

