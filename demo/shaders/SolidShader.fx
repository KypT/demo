#pragma target 3.0

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;

	float4 color;
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
	float2 tex   : TEXCOORD;
};

PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;

	output.Pos = input.Pos;
	output.Pos = mul(output.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

    return output;
}

float4 PS( PS_INPUT input) : SV_Target
{
	return color;
}
