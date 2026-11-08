#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 sunDirection;
    vec3 sunColor;
    float padding;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in float inBlockType;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;
layout(location = 2) out float blockType;
layout(location = 3) out vec3 normal;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragUV = inUV;
    blockType = inBlockType;
    
    // Normalna na podstawie cieniowania ściany
    // inColor: (1,1,1)=góra, (0.5,0.5,0.5)=dół, (0.8,0.8,0.8)=boki
    if (inColor.r > 0.95f) normal = vec3(0.0, 1.0, 0.0);
    else if (inColor.r < 0.6f) normal = vec3(0.0, -1.0, 0.0);
    else {
        // Boki - określ kierunek z UV
        if (inUV.x < 0.01f) normal = vec3(-1.0, 0.0, 0.0);
        else if (inUV.x > 0.99f) normal = vec3(1.0, 0.0, 0.0);
        else if (inUV.y < 0.01f) normal = vec3(0.0, 0.0, -1.0);
        else normal = vec3(0.0, 0.0, 1.0);
    }
}
