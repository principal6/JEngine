struct VS_INPUT
{
	float4 Position : POSITION;
	float4 ColorTex : COLORTEX;
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float4 ColorTex : COLROTEX;
};