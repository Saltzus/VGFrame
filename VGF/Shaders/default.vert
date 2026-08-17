#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 3) uniform AnimationBufferObject {
    mat4 jointMatrices[128];
} abo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec4 inJointIndices;
layout(location = 5) in vec4 inJointWeights;

layout(location = 6) in mat4 inModel;
layout(location = 10) in uint textureId;

layout(location = 0) out vec3 vertexColor;
layout(location = 1) out vec2 vertexTexCoord;
layout(location = 2) out vec3 vertexPosition;
layout(location = 3) out vec3 vertexNormal;

void main()
{
    float weightSum = inJointWeights.x + inJointWeights.y + inJointWeights.z + inJointWeights.w;
    mat4 skinMatrix;
    if (weightSum < 0.001)
    {
        skinMatrix = mat4(1.0);
    }
    else
    {
        skinMatrix =
            inJointWeights.x * abo.jointMatrices[int(inJointIndices.x)] +
            inJointWeights.y * abo.jointMatrices[int(inJointIndices.y)] +
            inJointWeights.z * abo.jointMatrices[int(inJointIndices.z)] +
            inJointWeights.w * abo.jointMatrices[int(inJointIndices.w)];

        if (skinMatrix[3][3] < 0.001)
        skinMatrix = mat4(1.0);
    }

    vec4 skinnedPosition = skinMatrix * vec4(inPosition, 1.0);
    vec4 worldPosition   = inModel * skinnedPosition;

    vertexPosition = worldPosition.xyz / worldPosition.w;
    vertexColor    = inColor;
    vertexTexCoord = vec2(inTexCoord.x, 1.0 - inTexCoord.y);
    vertexNormal   = normalize(vec3(inModel * skinMatrix * vec4(inNormal, 0.0)));

    gl_Position = ubo.proj * ubo.view * worldPosition;
}