#include "iHeader.hlsli"

SamplerState Sampler : register(s0);
Texture2D Image : register(t90);
Texture2D Atlas : register(t91);

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	if (Input.ColorTex.a >= 0)
	{
		return Input.ColorTex;
	}
	else if (Input.ColorTex.a < -1)
	{	
		return Image.Sample(Sampler, Input.ColorTex.xy);
	}
	return Atlas.Sample(Sampler, Input.ColorTex.xy);
}