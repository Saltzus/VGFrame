
layout(binding = 1) cbuffer PBRBuffer
{
    float3 cameraPosition;
    float metallicFactor;
    float4 baseColorFactor;
    float3 emissiveFactor;
    float roughnessFactor;
    float occlusionStrength;
};

struct DirectionalLight
{
    float3 direction;
    float3 color;
};

struct PointLight
{
    float3 position;
    float3 color;
};

layout(binding = 2) cbuffer LightBuffer
{
    float3 ambientLightColor;
    float ambientLightIntensity;

    DirectionalLight directionalLights[30];
    PointLight pointLights[30];
    float directionLightAmount;
    float pointLightAmount;
};

layout(binding = 4) Sampler2D colorSampler;
layout(binding = 5) Sampler2D metallicRoughnessSampler;
layout(binding = 6) Sampler2D emissiveSampler;
layout(binding = 7) Sampler2D occulsionSampler;
layout(binding = 8) Sampler2D normalSampler;

static const float near = 0.01;
static const float far = 100;

[shader("fragment")]
float4 main(float4 position : SV_Position)
{
    // Raw depth from the depth buffer (0 .. 1)
    float depth = position.z;

    // Convert back to NDC z (-1 .. 1)
    float z_ndc = depth * 2.0 - 1.0;

    float linearDepth = (2.0 * near * far) / (far + near - z_ndc * (far - near));

    // Normalize to 0 .. 1 (0 = near) (1 = far)
    float depth01 = (linearDepth - near) / (far - near);
    depth01 = clamp(depth01, 0.0, 1.0);

    return float4(float3(depth01), 1.0);
}