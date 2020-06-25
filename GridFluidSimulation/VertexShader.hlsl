



StructuredBuffer<float> densities : register(t0);

cbuffer PerObjectConstantBuffer : register(b0)
{
	matrix worldViewProjection;
	float time;
	float fogColorR;
	float fogColorG;
	float fogColorB;
};

struct VS_INPUT
{
	float2 vPos : POSITION0;
	float3 vTexUV : TEXCOORD0;
	//float4 vColor :	COLOR0;
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

	int index = input.vTexUV.z;

	output.position = mul(float4(input.vPos.x + input.vTexUV.x, input.vPos.y + input.vTexUV.y, 1, 1), worldViewProjection);
	//output.texUV = input.vTexUV;

	//output.color = float4(input.vColor.r * sin(time) / 2 + 0.5, input.vColor.g * cos(time) / 2 + 0.5, input.vColor.b, input.vColor.a);

	float density = densities[index];

	float blurFactor = 0;

	density *= 1;
	//density += (densities[index - 1] + densities[index + 1] + densities[index - columns] + densities[index + columns]) * blurFactor;

	output.color = float4(fogColorR, fogColorG, fogColorB, density);

	return output;
}
