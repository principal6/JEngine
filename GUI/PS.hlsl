#include "iHeader.hlsli"

SamplerState AtlasSampler : register(s0);
Texture2D Atlas : register(t91);

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	if (Input.ColorTex.a >= 0)
	{
		return Input.ColorTex;
	}
	return Atlas.Sample(AtlasSampler, Input.ColorTex.xy);
}