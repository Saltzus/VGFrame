#version 450

layout(binding = 1) uniform PBRbufferObject
{
    vec4 baseColorFactor;
    vec3 emissiveFactor;
    float metallicFactor;
    float roughnessFactor;
    float occlusionStrength;
}ubo;

layout(binding = 2) uniform sampler2D colorSampler;
layout(binding = 3) uniform sampler2D metallicRoughnessSampler;
layout(binding = 4) uniform sampler2D emissiveSampler;
layout(binding = 5) uniform sampler2D occulsionSampler;
layout(binding = 6) uniform sampler2D normalSampler;


layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(location = 4) in vec3 fragObjPos; // Interpolated object-space position

const float GAMMA = 2.2;
const float INV_GAMMA = 1. / GAMMA;
const float M_PI = 3.141592653589793;
const float M_1_PI = 1.0 / M_PI;

vec3 LINEARtoSRGB(vec3 color)  { return pow(color, vec3(INV_GAMMA)); }
vec4 SRGBtoLINEAR(vec4 srgbIn) { return vec4(pow(srgbIn.xyz, vec3(GAMMA)), srgbIn.w); }

void main() {

    vec3 emissive = SRGBtoLINEAR(texture(emissiveSampler, fragTexCoord)).rgb;

    vec4 baseColor = texture(colorSampler, fragTexCoord);
    float occlusion = texture(occulsionSampler, fragTexCoord).r;

    baseColor.rgb *= occlusion;
    baseColor.rgb += emissive;

    outColor = ubo.baseColorFactor;
}