#version 450

layout(binding = 0) uniform TextBufferObject {
    mat4 model;
    mat4 proj;
    vec4 color;
    vec2 uvScale;
} tbo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 4) in mat4 model;
layout(location = 8) in uint textureId;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = tbo.proj * tbo.model * vec4(inPosition, 1.0);
    fragColor = tbo.color;
    fragTexCoord = inTexCoord;
}