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

layout (location = 0) out vec4 outPos;
layout (location = 1) out vec2 uv;

void main() {
	outPos = vec4(inPosition.xyz, 1.0);
	gl_Position = outPos;
    uv = inTexCoord;
}