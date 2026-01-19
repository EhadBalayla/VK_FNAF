#ifndef UIHOVERABLE
#define UIHOVERABLE

typedef struct {
    float position[2];
    float scale[2];
    void* projMat;

    int texID;

    int IsHovered;

    void (*OnHovered)(void* pHoverable);
} UIHoverable;

void UpdateHoverable(UIHoverable* pHoverable);
void RenderHoverable(UIHoverable* pHoverable);

#endif