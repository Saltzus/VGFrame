#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(location = 4) in vec3 fragObjPos; // Interpolated object-space position

void main() {
    vec2 fragPos = fragObjPos.xy;
    // Use position to create a gradient
    //vec3 color = vec3(fragPos.x / 800.0, fragPos.y / 600.0, 0.5); // Assuming 800x600 viewport

    outColor = texture(texSampler, fragTexCoord);

    //vec3 color = fragObjPos * 0.5 + 0.5; // Normalize to [0,1] range
    //outColor = vec4(color, 1.0);
}