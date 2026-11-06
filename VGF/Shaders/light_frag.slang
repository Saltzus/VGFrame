struct VertexOut
{
    float4 sv_position : SV_Position;
    float3 color;
    float2 uv;
};

[shader("fragment")]
float4 main(VertexOut fragIn) : SV_Target
{
    float dist = length(fragIn.uv - 0.5);

    float radius = 0.25;
    float feather = 0.3;

    float circle = smoothstep(radius, radius + feather, dist);
    circle = 1.0 - circle;

    return float4(fragIn.color, circle);
}