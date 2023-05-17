#pragma target 3.0

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
	float4 Pos  : SV_POSITION;
	float4 color : COLOR;
	float2 tex   : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT) 0;

	output.Pos = input.Pos;
	output.Pos = mul(output.Pos, World);
	
	float3 lightDir = LightPos - output.Pos.xyz;
	float length = sqrt(lightDir.x * lightDir.x + lightDir.y * lightDir.y + lightDir.z * lightDir.z);
	lightDir = normalize(lightDir);

	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	float lightStrength = 15;

	output.color = Color * lightStrength / length * mul(input.Norm.xyz, lightDir);

	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	return input.color;
}
