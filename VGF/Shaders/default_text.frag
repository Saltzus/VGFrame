#version 450

layout(binding = 1) uniform sampler2D colorSampler;
layout(binding = 3) uniform sampler2D metallicRoughnessSampler;
layout(binding = 4) uniform sampler2D emissiveSampler;
layout(binding = 5) uniform sampler2D occulsionSampler;
layout(binding = 6) uniform sampler2D normalSampler;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 color = texture(colorSampler, vec2(fragTexCoord.x, 1 - fragTexCoord.y));
    if (color.a < 0.1) discard;
    
    outColor = vec4(fragColor.xyz, fragColor.w * color.r);
}