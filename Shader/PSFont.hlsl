#include "iFont.hlsli"

SamplerState Sampler : register(s0);
Texture2D Texture : register(t0);

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	float4 Sampled = Texture.Sample(Sampler, Input.TexCoord);

	Sampled.rgb = Input.Color.rgb;
	Sampled.a *= Input.Color.a;

	return Sampled;
}
