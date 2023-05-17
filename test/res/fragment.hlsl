
struct VSOutput
{
	float4 position : SV_POSITION;
	[[util::location(0)]] float3 colour : COLOR0;
	[[util::location(1)]] float2 uv : TEXCOORD0;
};

[[util::binding(1, 0)]] Texture2D<float4> texture0;
[[util::binding(1, 0)]] SamplerState sampler;

float4 main(VSOutput input) : SV_TARGET
{
    return texture0.Sample(sampler, input.uv) * float4(input.colour, 1.0);
};
