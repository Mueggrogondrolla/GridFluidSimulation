



StructuredBuffer<float> densities : register(t0);

cbuffer PerObjectConstantBuffer : register(b0)
{
	matrix worldViewProjection;
	float time;
};

struct VS_INPUT
{
	float2 vPos : POSITION0;
	//float2 vTexUV : TEXCOORD0;
	float4 vColor :	COLOR0;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	//float2 texUV : TEXCOORD0;
	float4 color :	COLOR0;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.position = mul(float4(input.vPos, 1, 1), worldViewProjection);
	//output.texUV = input.vTexUV;

	//output.color = float4(densities[0], densities[0], densities[0], densities[0]);
	//output.color = float4(input.vColor.r * sin(time) / 2 + 0.5, input.vColor.g * cos(time) / 2 + 0.5, input.vColor.b, input.vColor.a);
	output.color = float4(input.vColor.r, input.vColor.g, input.vColor.b, input.vColor.a);

	return output;
}
