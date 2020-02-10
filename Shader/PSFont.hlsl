#include "iFont.hlsli"

cbuffer cbColor : register(b3)
{
	float4 g_Color;
}

SamplerState Sampler : register(s0);
Texture2D Texture : register(t0);

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	float4 Sampled = Texture.Sample(Sampler, Input.TexCoord);

	Sampled.rgb = g_Color.rgb;
	Sampled.a *= g_Color.a;

	return Sampled;
}
