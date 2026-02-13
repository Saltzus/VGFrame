// https://github.com/bwasty/gltf-viewer/blob/master/src/shaders/pbr-frag.glsl
#version 450

layout(std140, binding = 1) uniform PBRbufferObject 
{
    vec3 cameraPosition;
    float metallicFactor;
    vec4 baseColorFactor;
    vec3 emissiveFactor;
    float roughnessFactor;
    float occlusionStrength;
}ubo;

struct DirectionalLight
{
    vec3 direction;  
    vec3 color;
};

struct PointLight
{
    vec3  position;
    vec3  color;
};

layout(std140, binding = 2) uniform LightBufferObject 
{
    vec3 ambientLightColor;
    float ambientLightIntensity;

    DirectionalLight directionalLights[30];
    PointLight pointLights[30];
    float directionLightAmount;
    float pointLightAmount;
}lightUbo;

layout(binding = 3) uniform sampler2D colorSampler;
layout(binding = 4) uniform sampler2D metallicRoughnessSampler;
layout(binding = 5) uniform sampler2D emissiveSampler;
layout(binding = 6) uniform sampler2D occulsionSampler;
layout(binding = 7) uniform sampler2D normalSampler;

layout(location = 0) in vec3 vertexColor;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexPosition;
layout(location = 3) in vec3 vertexNormal;

layout(location = 0) out vec4 outColor;

float near = 0.01;
float far = 100;

void main()
{
    // Raw depth from the depth buffer (0..1)
    float depth = gl_FragCoord.z;

    // Convert back to NDC z (-1 .. 1)
    float z_ndc = depth * 2.0 - 1.0;

    // Reconstruct linear view-space depth (positive distance from camera)
    // Formula: z_eye = (2 * near * far) / (far + near - z_ndc * (far - near))
    float linearDepth = (2.0 * near * far) / (far + near - z_ndc * (far - near));

    // Normalize to 0..1 where 0 => near and 1 => far
    float depth01 = (linearDepth - near) / (far - near);
    depth01 = clamp(depth01, 0.0, 1.0);

    outColor = vec4(vec3(depth01), 1.0); // grayscale visualization
}