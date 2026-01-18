#version 450

layout (location = 0) in vec2 uv;

layout (location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform sampler2D tex;

void main() {
    float curvature = -0.35;

    vec2 inTexCoord = uv * 2.0 - 1.0;

    float x_sq = inTexCoord.x * inTexCoord.x;

    vec2 warpedUV;
    warpedUV.x = inTexCoord.x; 
    warpedUV.y = inTexCoord.y / (1.0 - (x_sq * curvature));

    warpedUV = (warpedUV + 1.0) / 2.0;


    FragColor = texture(tex, warpedUV);
}