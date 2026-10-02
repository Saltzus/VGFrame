layout(binding = 0) cbuffer TextBuffer
{
    float4x4 model;
    float4x4 proj;
    float4 color;
    float2 uvScale;
};

struct VertexIn
{
    float3 position;
    float3 normal;
    float3 color;
    float2 texCoord;

    float4x4 model;
    uint textureId;
};

struct VertexOut
{
    float4 sv_position : SV_Position;
    float4 color;
    float2 texCoord;
};

[shader("vertex")]
VertexOut main(VertexIn vertexIn)
{
    VertexOut outVertex;
    outVertex.sv_position = mul(proj, mul(model, float4(vertexIn.position, 1.f)));
    outVertex.color = color;
    outVertex.texCoord = vertexIn.texCoord;

    return outVertex;
}