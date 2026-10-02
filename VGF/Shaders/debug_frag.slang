layout(binding = 1) Sampler2D colorSampler;
layout(binding = 3) Sampler2D metallicRoughnessSampler;
layout(binding = 4) Sampler2D emissiveSampler;
layout(binding = 5) Sampler2D occulsionSampler;
layout(binding = 6) Sampler2D normalSampler;

[shader("fragment")]
float4 main(float3 fragColor) : SV_Target
{
    return float4(fragColor,1.f);
}