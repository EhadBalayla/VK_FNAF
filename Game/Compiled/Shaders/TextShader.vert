#version 450

layout (location = 0) out vec2 uv;

vec2 positions[6] = vec2[](
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),

    vec2(1.0, 1.0),
    vec2(1.0, 0.0),
    vec2(0.0, 0.0)
);

vec2 uv_offsets[6] = vec2[](
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),

    vec2(1.0, 1.0),
    vec2(1.0, 0.0),
    vec2(0.0, 0.0)
);

layout (push_constant) uniform matrices {
    mat4 ProjModel;
    vec2 uvs; //uvs
    vec2 offsets;
} ProjModel_Mat;

void main() {
    uv = ProjModel_Mat.uvs + (uv_offsets[gl_VertexIndex] * ProjModel_Mat.offsets);
    gl_Position = ProjModel_Mat.ProjModel * vec4(positions[gl_VertexIndex], 0.0, 1.0);
}