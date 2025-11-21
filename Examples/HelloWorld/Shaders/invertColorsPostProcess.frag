#version 450

layout(binding = 1) uniform sampler2D colorSampler;

layout (location = 0) in vec4 inPos;
layout (location = 1) in vec2 uv;

layout (location = 0) out vec4 outColor;

void main() {
    vec4 tex = texture(colorSampler, uv);
    outColor = vec4(1.0 - tex.rgb, tex.a);
}
