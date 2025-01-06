struct FSInput {
  [[vk::location(0)]] float2 UV : TEXCOORD0;
};

struct FSOutput {
  [[vk::location(0)]] float4 Color : SV_Target;
};

Texture2D uTexture: register(t0);
SamplerState uSampler: register(s0);

FSOutput main(FSInput input) {
	float4 sampled = uTexture.Sample(uSampler, input.UV);

	FSOutput output;
	output.Color    = float4(sampled.r, sampled.r, sampled.r, sampled.g);
	// output.Color    = float4(input.UV, 0.0f, 1.0f);
	return output;
}