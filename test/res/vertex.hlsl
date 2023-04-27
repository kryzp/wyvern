
struct UniformBufferObject
{
	float4x4 model;
	float4x4 view;
	float4x4 proj;
};

cbuffer ubo : register(b0, space0) { UniformBufferObject ubo; }

struct VSInput
{
    [[vk::location(0)]] float3 position : POSITION0;
    [[vk::location(1)]] float2 uv : TEXCOORD0;
    [[vk::location(2)]] float3 colour : COLOR0;
};

struct VSOutput
{
	float4 position : SV_POSITION;
	[[vk::location(0)]] float3 colour : COLOR0;
	[[vk::location(1)]] float2 uv : TEXCOORD0;
};

VSOutput main(VSInput input, uint vertex_idx : SV_VertexID)
{
	VSOutput output = (VSOutput)0;
	output.position = mul(ubo.proj, mul(ubo.view, mul(ubo.model, float4(input.position.xyz, 1.0))));
    output.colour = input.colour;
	output.uv = input.uv;
	return output;
}
