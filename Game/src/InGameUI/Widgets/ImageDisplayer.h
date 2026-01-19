#ifndef IMAGE_DISPLAYER
#define IMAGE_DISPlAYER

//so that we wont need to include cglm, i wanna save on as much includes as possible for quick compile times
typedef float vec2[2];
typedef struct {
    vec2 position;
    vec2 scale;

    float Left, Right, Bottom, Top; //for the projection
    int texID;
} ImageDisplayer;

void Render_Image(ImageDisplayer* pImage);

#endif
