#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 sunDirection;
    vec3 sunColor;
    float padding;
} ubo;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;
layout(location = 2) in float blockType;
layout(location = 3) in vec3 normal;

layout(location = 0) out vec4 outColor;

float hash(vec2 p) { return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453); }
float noise(vec2 p) {
    vec2 i = floor(p); vec2 f = fract(p);
    f = f*f*(3.0-2.0*f);
    return mix(mix(hash(i), hash(i+vec2(1,0)), f.x), mix(hash(i+vec2(0,1)), hash(i+vec2(1,1)), f.x), f.y);
}
float fbm(vec2 p) { float v=0.0, a=0.5; for(int i=0;i<4;i++){v+=a*noise(p);p*=2.0;a*=0.5;} return v; }

vec3 texStone(vec2 uv) {
    vec2 pc = floor(uv*64.0);
    vec2 bc = vec2(floor(pc.x/16.0), floor(pc.y/8.0));
    float rowOff = mod(bc.y, 2.0)*8.0;
    vec2 lb = vec2(mod(pc.x+rowOff, 16.0), mod(pc.y, 8.0));
    float s = hash(bc);
    vec3 c = s<0.4?vec3(0.5,0.47,0.44):s<0.7?vec3(0.65,0.6,0.55):vec3(0.35,0.32,0.3);
    c *= 0.75+0.35*fbm(pc*0.5+bc*100.0);
    float e = min(min(lb.x,15.0-lb.x), min(lb.y,7.0-lb.y));
    c *= 0.6+0.4*smoothstep(0.0,2.0,e);
    float m = step(lb.x,1.0)+step(15.0-lb.x,lb.x)+step(lb.y,1.0)+step(7.0-lb.y,lb.y);
    c = mix(c, vec3(0.2,0.18,0.17), min(m,1.0));
    return c;
}

vec3 texDirt(vec2 uv) {
    vec2 pc = floor(uv*64.0);
    vec3 c = vec3(0.5,0.4,0.3)*(0.8+0.4*fbm(pc*0.4));
    c = mix(c, vec3(0.55,0.45,0.35), fbm(pc*0.8+50.0)*0.3);
    return c;
}

vec3 texGrass(vec2 uv) {
    vec2 pc = floor(uv*64.0);
    vec3 soil = vec3(0.5,0.4,0.3)*(0.8+0.4*fbm(pc*0.3));
    float g = fbm(pc*0.5+100.0);
    vec3 grass = mix(vec3(0.2,0.5,0.15), vec3(0.3,0.7,0.2), g);
    return mix(soil, grass, step(0.45, g)*0.7);
}

vec3 calcLighting(vec3 baseColor, vec3 normal) {
    vec3 lightDir = normalize(ubo.sunDirection);
    vec3 lightColor = ubo.sunColor;
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 ambient = vec3(0.25, 0.3, 0.4);
    vec3 viewDir = vec3(0.0, 0.0, 1.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = lightColor * spec * 0.15;
    return baseColor * (ambient + lightColor * diff) + specular;
}

vec3 ACESFilm(vec3 x) {
    float a=2.51, b=0.03, c=2.43, d=0.59, e=0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

void main() {
    vec3 baseColor;
    float bt = floor(blockType + 0.5);
    if (bt < 1.5) baseColor = texStone(fragUV);
    else if (bt < 2.5) baseColor = texDirt(fragUV);
    else if (bt < 3.5) baseColor = texGrass(fragUV);
    else baseColor = texStone(fragUV);
    
    baseColor *= fragColor;
    vec3 litColor = calcLighting(baseColor, normal);
    litColor = ACESFilm(litColor);
    litColor = pow(litColor, vec3(1.0/2.2));
    outColor = vec4(litColor, 1.0);
}
