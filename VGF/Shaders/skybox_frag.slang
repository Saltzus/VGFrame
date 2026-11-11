struct VertexOut
{
    float4 sv_position : SV_Position;
    float3 viewDir;
};

SamplerCube samplerCube;

[shader("fragment")]
float4 main(VertexOut fragIn) : SV_Target
{
    return samplerCube.Sample(normalize(fragIn.viewDir));
}