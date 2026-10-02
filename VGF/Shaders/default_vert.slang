layout(binding = 0) cbuffer MatrixBuffer
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};

layout(binding = 3) cbuffer AnimationBuffer
{
    float4x4 jointMatrices[128];
};

struct VertexIn
{
    float3 position;
    float3 normal;
    float3 color;
    float2 texCoord;
    float4 jointIndices;
    float4 jointWeights;

    float4x4 model;
    uint textureId;
};

struct VertexOut
{
    float4 sv_position : SV_Position;
    float3 color;
    float2 texCoord;
    float3 position;
    float3 normal;
    float3 textureId;
};

static const float4x4 IDENTITY = float4x4
(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
);

[shader("vertex")]
VertexOut main(VertexIn vertexIn)
{
    float weightSum = vertexIn.jointWeights.x + vertexIn.jointWeights.y + vertexIn.jointWeights.z + vertexIn.jointWeights.w;
    float4x4 skinMatrix;
    if (weightSum < 0.001)
    {
        skinMatrix = IDENTITY;
    }
    else
    {
        skinMatrix =
            vertexIn.jointWeights.x * jointMatrices[int(vertexIn.jointIndices.x)] +
            vertexIn.jointWeights.y * jointMatrices[int(vertexIn.jointIndices.y)] +
            vertexIn.jointWeights.z * jointMatrices[int(vertexIn.jointIndices.z)] +
            vertexIn.jointWeights.w * jointMatrices[int(vertexIn.jointIndices.w)];

        if (skinMatrix[3][3] < 0.001) skinMatrix = IDENTITY;
    }

    float4 skinnedPosition = mul(skinMatrix, float4(vertexIn.position, 1.0));
    float4 worldPosition = mul(vertexIn.model, skinnedPosition);

    VertexOut outVertex;
    outVertex.sv_position = mul(proj, mul(view, worldPosition));
    outVertex.color = vertexIn.color;
    outVertex.texCoord = float2(vertexIn.texCoord.x, 1.0 - vertexIn.texCoord.y);
    outVertex.position = worldPosition.xyz / worldPosition.w;
    outVertex.normal = normalize(mul(mul(vertexIn.model, skinMatrix), float4(vertexIn.normal, 0.0)).xyz);
    outVertex.textureId = vertexIn.textureId;

    return outVertex;
}