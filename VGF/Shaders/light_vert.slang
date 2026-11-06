cbuffer MatrixBuffer
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};

struct VertexIn
{
    float3 position;
    float2 texCoord;
};

cbuffer LightColor
{
    float3 color;
}

struct VertexOut
{
    float4 sv_position : SV_Position;
    float3 color;
    float2 uv;
};

[shader("vertex")]
VertexOut main(VertexIn vertexIn)
{
    float4 billboard = mul(view, mul(model, float4(0.f, 0.f, 0.f, 1.f)));
    float4 newPos = mul(proj, (billboard + float4(vertexIn.position,0.f)));

    VertexOut outVertex;
    outVertex.sv_position = newPos;
    outVertex.color = color;
    outVertex.uv = vertexIn.texCoord;

    return outVertex;
}