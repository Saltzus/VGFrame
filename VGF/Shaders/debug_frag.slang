Sampler2D colorSampler;
Sampler2D metallicRoughnessSampler;
Sampler2D emissiveSampler;
Sampler2D occulsionSampler;
Sampler2D normalSampler;

[shader("fragment")]
float4 main(float3 fragColor) : SV_Target
{
    return float4(fragColor,1.f);
}