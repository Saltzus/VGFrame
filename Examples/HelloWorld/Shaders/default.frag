// https://github.com/bwasty/gltf-viewer/blob/master/src/shaders/pbr-frag.glsl
#version 450

layout(binding = 1) uniform PBRbufferObject
{
    vec3 cameraPosition;
    vec4 baseColorFactor;
    vec3 emissiveFactor;
    float metallicFactor;
    float roughnessFactor;
    float occlusionStrength;
}ubo;

vec3 lightDirection = vec3(0, 0.5f, 0.5f);  
vec3 lightColor = vec3(0,0,5);

vec3 ambientLightColor = vec3(1.0,1.0,1.0);
float ambientLightIntensity = 0.2;

layout(binding = 2) uniform sampler2D colorSampler;
layout(binding = 3) uniform sampler2D metallicRoughnessSampler;
layout(binding = 4) uniform sampler2D emissiveSampler;
layout(binding = 5) uniform sampler2D occulsionSampler;
layout(binding = 6) uniform sampler2D normalSampler;


layout(location = 0) in vec3 vertexColor;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexPosition;
layout(location = 3) in vec3 vertexNormal;


layout(location = 0) out vec4 outColor;


struct PointLight
{
    vec3  position;
    vec3  color;
    float constant;
    float linear;
    float quadratic;
};

PointLight pointLight = PointLight(
    vec3(  0.0,  3.0,  -20.0 ),   // position: above and in front of the origin
    vec3(  10,  0,  0 ),   // color: warm hue, roughly “tungsten”
    1.0,                        // constant: no extra dimming at zero distance
    0.14,                       // linear: moderate range
    0.07                        // quadratic: gives ~20–30 unit falloff radius
);


struct PBRInfo
{
    float NdotL;                  // cos angle between normal and light direction
    float NdotV;                  // cos angle between normal and view direction
    float NdotH;                  // cos angle between normal and half vector
    float LdotH;                  // cos angle between light direction and half vector
    float VdotH;                  // cos angle between view direction and half vector
    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
    float metalness;              // metallic value at the surface
    vec3 reflectance0;            // full reflectance color (normal incidence angle)
    vec3 reflectance90;           // reflectance color at grazing angle
    float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
    vec3 diffuseColor;            // color contribution from diffuse lighting
    vec3 specularColor;           // color contribution from specular lighting
};  

const float M_PI = 3.141592653589793;
const float c_MinRoughness = 0.04;


vec3 getNormal()
{
    // Retrieve the tangent space matrix
    vec3 pos_dx = dFdx(vertexPosition);
    vec3 pos_dy = dFdy(vertexPosition);
    vec3 tex_dx = dFdx(vec3(vertexTexCoord, 0.0));
    vec3 tex_dy = dFdy(vec3(vertexTexCoord, 0.0));
    vec3 tangent = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);

    vec3 ng = normalize(vertexNormal);

    tangent = normalize(tangent - ng * dot(ng, tangent));
    vec3 b = normalize(cross(ng, tangent));
    mat3 tbn = mat3(tangent, b, ng);

    //vec3 normal = texture(normalSampler, vertexTexCoord).rgb;
    //normal = normalize(tbn * ((2.0 * normal - 1.0) * vec3(0.01, 0.01, 1.0)));
    vec3 normal = normalize(tbn[2].xyz);
    // reverse backface normals
    // TODO!: correct/best place? -> https://github.com/KhronosGroup/glTF-WebGL-PBR/issues/51
    normal *= (2.0 * float(gl_FrontFacing) - 1.0);

    return normal;
}

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
vec3 diffuse(PBRInfo pbrInputs)
{
    return pbrInputs.diffuseColor / M_PI;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
vec3 specularReflection(PBRInfo pbrInputs)
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

PBRInfo makePBRInfo(vec3 n, vec3 v, vec3 l, vec4 baseColor, float perceptualRoughness, float metallic) {
    vec3 h = normalize(l + v);
    float NdotL = clamp(dot(n, l), 0.001, 1.0);
    float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
    float NdotH = clamp(dot(n, h), 0.0, 1.0);
    float LdotH = clamp(dot(l, h), 0.0, 1.0);
    float VdotH = clamp(dot(v, h), 0.0, 1.0);
    float alphaRoughness = perceptualRoughness * perceptualRoughness;

    // same reflectance & diffuse/specular setup you already have...
    vec3 f0 = vec3(0.04);
    vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0) * (1.0 - metallic);
    vec3 specularColor = mix(f0, baseColor.rgb, metallic);
    float reflectance     = max(max(specularColor.r, specularColor.g), specularColor.b);
    float reflectance90   = clamp(reflectance * 25.0, 0.0, 1.0);
    vec3 R0 = specularColor;
    vec3 R90 = vec3(1.0) * reflectance90;

    return PBRInfo(
      NdotL, NdotV, NdotH, LdotH, VdotH,
      perceptualRoughness, metallic,
      R0, R90, alphaRoughness,
      diffuseColor, specularColor
    );
}


void main()
{
    // Metallic and Roughness material properties are packed together
    // In glTF, these factors can be specified by fixed scalar values
    // or from a metallic-roughness map
    float perceptualRoughness = ubo.roughnessFactor;
    float metallic = ubo.metallicFactor;

    // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
    // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
    vec4 mrSample = texture(metallicRoughnessSampler, vertexTexCoord);
    perceptualRoughness = mrSample.g * perceptualRoughness;
    metallic = mrSample.b * metallic;

    perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);
    metallic = clamp(metallic, 0.0, 1.0);
    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    float alphaRoughness = perceptualRoughness * perceptualRoughness;

    // The albedo may be defined from a base texture or a flat color
    vec4 baseColor = texture(colorSampler, vertexTexCoord) * ubo.baseColorFactor;

    // spec: COLOR_0 ... acts as an additional linear multiplier to baseColor
    baseColor *= vec4(vertexColor, 1);

    vec3 normal = getNormal();                                           // normal at surface point
    vec3 vector = normalize(ubo.cameraPosition - vertexPosition);        // Vector from surface point to camera

    vec3 color = {1,1,1};

    // Directional light
    {
        vec3 lightDir = normalize(lightDirection);
        PBRInfo dirInfo = makePBRInfo(normal, vector, lightDir, baseColor, perceptualRoughness, metallic);
        vec3 F = specularReflection(dirInfo);
        float G = geometricOcclusion(dirInfo);
        float D = microfacetDistribution(dirInfo);
        vec3 diffuseContrib = (1.0 - F) * diffuse(dirInfo);
        vec3 specContrib = F * G * D / (4.0 * dirInfo.NdotL * dirInfo.NdotV);
        color = dirInfo.NdotL * lightColor * (diffuseContrib + specContrib);
    }

    // Point light
    {
        vec3   lightVec = pointLight.position - vertexPosition;
        float  dist     = length(lightVec);
        vec3   lightPoint     = normalize(lightVec);
        float  attenuation = 1.0 / (pointLight.constant
                                  + pointLight.linear  * dist
                                  + pointLight.quadratic * dist * dist);

        PBRInfo ptInfo = makePBRInfo(normal, vector, lightPoint, baseColor, perceptualRoughness, metallic);
        vec3 Fp = specularReflection(ptInfo);
        float Gp = geometricOcclusion(ptInfo);
        float Dp = microfacetDistribution(ptInfo);
        vec3 diffp = (1.0 - Fp) * diffuse(ptInfo);
        vec3 specp = Fp * Gp * Dp / (4.0 * ptInfo.NdotL * ptInfo.NdotV);

        color += attenuation * ptInfo.NdotL * pointLight.color * (diffp + specp);
    }

    color += ambientLightColor * ambientLightIntensity * baseColor.xyz;

    float ao = texture(occulsionSampler, vertexTexCoord).r;
    color = mix(color, color * ao, ubo.occlusionStrength);

    vec3 emissive = texture(emissiveSampler, vertexTexCoord).rgb * ubo.emissiveFactor;
    color += emissive;

    outColor = vec4(color, 1);
}