#ifndef UIBUTTON
#define UIBUTTON

typedef struct {
    float position[2];
    float scale[2];
    void* projMat;

    int texID;

    int IsHovered;

    void (*OnHovered)(void* pButton);
    void (*OnClick)(void* pButton);
} UIButton;

void UpdateButton(UIButton* pButton);
void RenderButton(UIButton* pButton);

#endif