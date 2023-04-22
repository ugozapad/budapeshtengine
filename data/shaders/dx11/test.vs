cbuffer GlobalData : register(b0)
{
	float4x4 g_modelMatrix;
	float4x4 g_viewMatrix;
	float4x4 g_projectionMatrix;
	float4x4 g_modelViewProjection;
}

struct VS_INPUT
{
	float3 position : POSITION;
	float2 texcoord0 : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texcoord0 : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input) 
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(float4(input.position, 1.0f), g_modelMatrix);
	output.position = mul(output.position, g_viewMatrix);
	output.position = mul(output.position, g_projectionMatrix);
	output.texcoord0 = input.texcoord0;
	return output;
}