
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

struct FragmentIn
{
    float3 color;
    float2 texCoord;
    float3 position;
    float3 normal;
    bool isFrontFacing : SV_IsFrontFace;
};

struct FragmentOut
{
    float4 color : SV_Target;
};

struct PBRInfo
{
    float NdotL;                // cos angle between normal and light direction
    float NdotV;                // cos angle between normal and view direction
    float NdotH;                // cos angle between normal and half vector
    float LdotH;                // cos angle between light direction and half vector
    float VdotH;                // cos angle between view direction and half vector
    float perceptualRoughness;  // roughness value, as authored by the model creator (input to shader)
    float metalness;            // metallic value at the surface
    float3 reflectance0;        // full reflectance color (normal incidence angle)
    float3 reflectance90;       // reflectance color at grazing angle
    float alphaRoughness;       // roughness mapped to a more linear change in the roughness (proposed by [2])
    float3 diffuseColor;        // color contribution from diffuse lighting
    float3 specularColor;       // color contribution from specular lighting
};

static const float M_PI = 3.141592653589793;
static const float c_MinRoughness = 0.04;

// Converts a color from linear light gamma to sRGB gamma
float4 fromLinear(float4 linearRGB)
{
    bool3 cutoff = linearRGB.rgb < float3(0.0031308);
    float3 higher = float3(1.055) * pow(linearRGB.rgb, float3(1.0 / 2.4)) - float3(0.055);
    float3 lower = linearRGB.rgb * float3(12.92);

    return float4(select(cutoff, lower, higher), linearRGB.a);
}

// Converts a color from sRGB gamma to linear light gamma
float4 toLinear(float4 sRGB)
{
    bool3 cutoff = sRGB.rgb < float3(0.04045);
    float3 higher = pow((sRGB.rgb + float3(0.055)) / float3(1.055), float3(2.4));
    float3 lower = sRGB.rgb / float3(12.92);

    return float4(select(cutoff, lower, higher), sRGB.a);
}

float3 getNormal(FragmentIn fragmentIn)
{
    // Retrieve the tangent space matrix
    float3 pos_dx = ddx(fragmentIn.position);
    float3 pos_dy = ddy(fragmentIn.position);
    float3 tex_dx = ddx(float3(fragmentIn.texCoord, 0.0));
    float3 tex_dy = ddy(float3(fragmentIn.texCoord, 0.0));
    float3 tangent = (tex_dy.y * pos_dx - tex_dx.y * pos_dy) / (tex_dx.x * tex_dy.y - tex_dy.x * tex_dx.y);

    float3 ng = normalize(fragmentIn.normal);

    tangent = normalize(tangent - ng * dot(ng, tangent));
    float3 b = normalize(cross(ng, tangent));
    float3x3 tbn = transpose(float3x3(tangent, b, ng));

    float3 normal = normalize(tbn[2].xyz);
    normal *= (2.0 * float(fragmentIn.isFrontFacing) - 1.0);

    return normal;
}

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
float3 diffuse(PBRInfo pbrInputs)
{
    return pbrInputs.diffuseColor / M_PI;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
float3 specularReflection(PBRInfo pbrInputs)
{
    return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
float geometricOcclusion(PBRInfo pbrInputs)
{
    float NdotL = pbrInputs.NdotL;
    float NdotV = pbrInputs.NdotV;
    float roughness = pbrInputs.alphaRoughness;

    float attenuationL = 2.0 * NdotL / (NdotL + sqrt(roughness * roughness + (1.0 - roughness * roughness) * (NdotL * NdotL)));
    float attenuationV = 2.0 * NdotV / (NdotV + sqrt(roughness * roughness + (1.0 - roughness * roughness) * (NdotV * NdotV)));
    return attenuationL * attenuationV;
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
float microfacetDistribution(PBRInfo pbrInputs)
{
    float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
    return roughnessSq / (M_PI * f * f);
}

PBRInfo makePBRInfo(float3 n, float3 v, float3 l, float4 baseColor, float perceptualRoughness, float metallic) {
    float3 h = normalize(l + v);
    float NdotL = clamp(dot(n, l), 0.001, 1.0);
    float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
    float NdotH = clamp(dot(n, h), 0.0, 1.0);
    float LdotH = clamp(dot(l, h), 0.0, 1.0);
    float VdotH = clamp(dot(v, h), 0.0, 1.0);
    float alphaRoughness = perceptualRoughness * perceptualRoughness;

    // same reflectance & diffuse/specular setup you already have...
    float3 f0 = float3(0.04);
    float3 diffuseColor = baseColor.rgb * (float3(1.0) - f0) * (1.0 - metallic);
    float3 specularColor = lerp(f0, baseColor.rgb, metallic);
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);
    float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    float3 R0 = specularColor;
    float3 R90 = float3(1.0) * reflectance90;

    return PBRInfo(
        NdotL, NdotV, NdotH, LdotH, VdotH,
        perceptualRoughness, metallic,
        R0, R90, alphaRoughness,
        diffuseColor, specularColor
    );
}

[shader("fragment")]
FragmentOut main(FragmentIn fragmentIn)
{
    // Metallic and Roughness material properties are packed together
    // In glTF, these factors can be specified by fixed scalar values
    // or from a metallic-roughness map
    float perceptualRoughness = roughnessFactor;
    float metallic = metallicFactor;

    // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
    // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
    float4 mrSample = metallicRoughnessSampler.Sample(fragmentIn.texCoord);
    perceptualRoughness = mrSample.g * perceptualRoughness;
    metallic = mrSample.b * metallic;

    perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);
    metallic = clamp(metallic, 0.0, 1.0);
    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    float alphaRoughness = perceptualRoughness * perceptualRoughness;

    // The albedo may be defined from a base texture or a flat color
    float4 baseColor = colorSampler.Sample(fragmentIn.texCoord) * baseColorFactor;
    if (baseColor.a < 0.1) discard;
    // vec4 baseColor     = toLinear(baseColorSRGB);

    // spec: COLOR_0 ... acts as an additional linear multiplier to baseColor
    baseColor *= float4(fragmentIn.color, 1);

    float3 normal = getNormal(fragmentIn);                                 // normal at surface point
    float3 vector = normalize(cameraPosition - fragmentIn.position); // Vector from surface point to camera

    float3 f0 = float3(0.04);
    float3 diffuseColor = baseColor.rgb * (float3(1.0) - f0) * (1.0 - metallic);
    float3 specularColor = lerp(f0, baseColor.rgb, metallic);

    float3 color = float3(0);

    // Directional light
    for (int i = 0; i < directionLightAmount; i++)
    {
        float3 lightDir = normalize(directionalLights[i].direction);
        PBRInfo dirInfo = makePBRInfo(normal, vector, lightDir, baseColor, perceptualRoughness, metallic);
        float3 F = specularReflection(dirInfo);
        float G = geometricOcclusion(dirInfo);
        float D = microfacetDistribution(dirInfo);
        float3 diffuseContrib = (1.0 - F) * diffuse(dirInfo);
        float3 specContrib = F * G * D / (4.0 * dirInfo.NdotL * dirInfo.NdotV);
        color += dirInfo.NdotL * directionalLights[i].color * (diffuseContrib + specContrib);
    }

    // Point light
    for (int i = 0; i < pointLightAmount; i++)
    {
        float3 lightVec = pointLights[i].position - fragmentIn.position;
        float dist = length(lightVec);
        float3 lightPoint = normalize(lightVec);

        PBRInfo ptInfo = makePBRInfo(normal, vector, lightPoint, baseColor, perceptualRoughness, metallic);
        float3 Fp = specularReflection(ptInfo);
        float Gp = geometricOcclusion(ptInfo);
        float Dp = microfacetDistribution(ptInfo);
        float3 diffp = (1.0 - Fp) * diffuse(ptInfo);
        float3 specp = Fp * Gp * Dp / (4.0 * ptInfo.NdotL * ptInfo.NdotV);
        color += ptInfo.NdotL * pointLights[i].color * (diffp + specp);
    }

    color += ambientLightColor * ambientLightIntensity * baseColor.xyz;

    float ao = occulsionSampler.Sample(fragmentIn.texCoord).a;
    color = lerp(color, color * ao, occlusionStrength);

    float3 emissive = emissiveSampler.Sample(fragmentIn.texCoord).rgb * emissiveFactor;
    color += emissive;

    FragmentOut output;
    output.color = float4(color, 1.0);
    return output;
}