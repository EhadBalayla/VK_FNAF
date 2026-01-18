#version 450

layout (location = 0) out vec2 uv;

vec2 positions[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, 1.0),

    vec2(1.0, 1.0),
    vec2(1.0, -1.0),
    vec2(-1.0, -1.0)
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
    int idx;
    int framesCount;
} ProjModel_Mat;

void main() {
    vec2 frameUVs = vec2(uvs[gl_VertexIndex].x / float(ProjModel_Mat.framesCount), uvs[gl_VertexIndex].y); //gets the uvs for a single frame
    float offset = (1.0 / float(ProjModel_Mat.framesCount)) * ProjModel_Mat.idx;

    uv = vec2(frameUVs.x + offset, frameUVs.y);
    gl_Position = ProjModel_Mat.ProjModel * vec4(positions[gl_VertexIndex], 0.0, 1.0);
}