#pragma target 3.0
#include "noiseSimplex.cginc"

cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;

	float4 Color;
	float3 LightPos;
	float3 Shuffle;
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
	float  noise : FLOAT;
	float2 tex   : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT) 0;

	output.Pos = mul(input.Pos, World);
	
	float2 pos = input.Pos.xz + Shuffle.xy;

	output.noise = snoise(pos);
	output.Pos += float4(0.0f, -output.noise * 3, 0.0f, 0.0f);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	return Color * (1.0f + input.noise) / 2;
}

