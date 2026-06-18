cbuffer MatrixBuffer
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};

struct VertexIn
{
    float3 inPosition;
    float3 inNormal;
    float3 inColor;
    float2 inTexCoord;

    float4x4 model;
    uint textureId;
};

struct VertexOut
{
    float4 sv_position : SV_Position;
    float3 fragColor;
    float2 fragTexCoord;
};

[shader("vertex")]
VertexOut main(VertexIn vertexIn)
{
    VertexOut outVertex;
    outVertex.fragColor = vertexIn.inColor;
    outVertex.fragTexCoord = vertexIn.inTexCoord;

    outVertex.sv_position = mul(proj, mul(view, mul(vertexIn.model, float4(vertexIn.inPosition, 1.0))));

    return outVertex;
}