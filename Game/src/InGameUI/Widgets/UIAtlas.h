#ifndef UIATLAS
#define UIATLAS

typedef struct {
    float position[2];
    float scale[2];
    void* projMat;
    int texID;
    float sequence; //the sequence will be hardcoded to be limited to 1 and the speed scale will control the incrementation up until 1
    float speedScale;
    int maxFrames;
} UIAtlas;

void RenderUIAtlas(UIAtlas* pUIAtlas);

#endif