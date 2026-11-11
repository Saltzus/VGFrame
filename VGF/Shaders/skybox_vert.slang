cbuffer MatrixBuffer
{
    float4x4 view;
    float4x4 proj;
};

struct VertexIn
{
    float3 position;
};

struct VertexOut
{
    float4 sv_position : SV_Position;
    float3 viewDir;
};

[shader("vertex")]
VertexOut main(VertexIn vertexIn)
{
    VertexOut outVertex;
    outVertex.viewDir = vertexIn.position;

    float3x3 onlyRotation = (float3x3)view;
    float4 viewPosition = float4(mul(onlyRotation, vertexIn.position), 1.f);
    float4 newPosition = mul(proj, viewPosition);

    outVertex.sv_position = float4(newPosition.x, newPosition.y, newPosition.w, newPosition.w);

    return outVertex;
}