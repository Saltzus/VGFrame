layout(binding = 0) cbuffer MatrixBuffer
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};

struct VertexIn
{
    float3 position;
    float3 color;

    float4x4 model;
    uint textureId;
};

struct VertexOut
{
    float4 sv_position : SV_Position;
    float3 color;
};

[shader("vertex")]
VertexOut main(VertexIn vertexIn)
{
    VertexOut outVertex;
    outVertex.sv_position = mul(proj, mul(view, mul(vertexIn.model, float4(vertexIn.position, 1.f))));
    outVertex.color = vertexIn.color;

    return outVertex;
}