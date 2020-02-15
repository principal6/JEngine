#include "iHeader.hlsli"

cbuffer cbSpace : register(b0)
{
	float4x4	g_Projection;
	float4		g_Offset;
}

VS_OUTPUT main(VS_INPUT Input)
{
	VS_OUTPUT Output;
	Output.Position = mul(Input.Position + g_Offset, g_Projection);
	Output.ColorTex = Input.ColorTex;
	return Output;
}