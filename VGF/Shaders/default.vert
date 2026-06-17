#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 4) in mat4 inModel;
layout(location = 8) in uint textureId;

layout(location = 0) out vec3 vertexColor;
layout(location = 1) out vec2 vertexTexCoord;
layout(location = 2) out vec3 vertexPosition;
layout(location = 3) out vec3 vertexNormal;

void main() {
    vec4 position = inModel * vec4(inPosition, 1.0);
    vertexPosition = vec3(position.xyz) / position.w;
    
    vertexColor = inColor;
    vertexTexCoord = inTexCoord;

    vertexNormal = normalize(vec3(inModel * vec4(inNormal.xyz, 0.0)));

    gl_Position = ubo.proj * ubo.view * inModel * vec4(inPosition, 1.0);
}