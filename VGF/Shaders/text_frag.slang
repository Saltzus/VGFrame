layout(binding = 1) Sampler2D colorSampler;
layout(binding = 3) Sampler2D metallicRoughnessSampler;
layout(binding = 4) Sampler2D emissiveSampler;
layout(binding = 5) Sampler2D occulsionSampler;
layout(binding = 6) Sampler2D normalSampler;

[shader("fragment")]
float4 main(float4 fragColor, float2 fragTexCoord) : SV_Target
{
    float4 color = colorSampler.Sample(float2(fragTexCoord.x, 1 - fragTexCoord.y));
    if (color.a < 0.1) discard;
    return float4(fragColor.xyz, fragColor.w * color.r);
}