
struct VSOutput
{
	float4 position : SV_POSITION;
	[[vk::location(0)]] float3 colour : COLOR0;
	[[vk::location(1)]] float2 uv : TEXCOORD0;
};

[[vk::binding(1, 0)]] Texture2D<float4> texture0;
[[vk::binding(1, 0)]] SamplerState sampler;

float4 main(VSOutput input) : SV_TARGET
{
    return texture0.Sample(sampler, input.uv) * float4(input.colour, 1.0);
};
