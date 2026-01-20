#ifndef IMAGE_DISPLAYER
#define IMAGE_DISPLAYER

typedef struct {
    float position[2];
    float scale[2];
    void* projMat;
    int texID;
} ImageDisplayer;

void Render_Image(ImageDisplayer* pImage);

#endif
