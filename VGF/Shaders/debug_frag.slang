[shader("fragment")]
float4 main(float3 fragColor) : SV_Target
{
    return float4(fragColor,1.f);
}