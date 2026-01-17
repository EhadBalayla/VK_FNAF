#version 450

layout (location = 0) out vec2 uv;

vec2 positions[6] = vec2[](
    vec2(-0.5, -0.5),
    vec2(-0.5, 0.5),
    vec2(0.5, 0.5),

    vec2(0.5, 0.5),
    vec2(0.5, -0.5),
    vec2(-0.5, -0.5)
);

vec2 uvs[6] = vec2[](
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),

    vec2(1.0, 1.0),
    vec2(1.0, 0.0),
    vec2(0.0, 0.0)
);

layout (push_constant) uniform matrices {
    mat4 ProjModel;
} ProjModel_Mat;

void main() {
    uv = uvs[gl_VertexIndex];
    gl_Position = ProjModel_Mat.ProjModel * vec4(positions[gl_VertexIndex], 0.0, 1.0);
}