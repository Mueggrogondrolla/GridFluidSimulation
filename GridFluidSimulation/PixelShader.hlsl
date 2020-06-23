





Texture2D spriteTex : register(t0);
SamplerState spriteSampler : register(s0);

struct PS_INPUT
{
	float4 position : SV_POSITION;
	//float2 texUV : TEXCOORD0;
	float4 color :	COLOR0;
};

struct PS_OUTPUT
{
	float4 color : SV_TARGET;
};

PS_OUTPUT main(PS_INPUT pixel)
{
	PS_OUTPUT output;

	output.color = pixel.color;
	//output.color = saturate(spriteTex.Sample(spriteSampler, pixel.texUV));

	return output;
}