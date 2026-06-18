Sampler2D colorSampler;
Sampler2D metallicRoughnessSampler;
Sampler2D emissiveSampler;
Sampler2D occulsionSampler;
Sampler2D normalSampler;

struct FragmentIn
{
    float3 fragColor;
    float2 fragTexCoord;
};

struct FragmentOut
{
    float4 outColor;
};

[shader("fragment")]
FragmentOut main(FragmentIn fragmentIn)
{
    float3 color = fragmentIn.fragColor;

    fragmentIn.fragTexCoord;

    FragmentOut output;
    output.outColor = float4(color, 1.0);

    return output;
}