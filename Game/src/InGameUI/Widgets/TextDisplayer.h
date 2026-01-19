#ifndef TEXT_DISPLAYER
#define TEXT_DISPLAYER

typedef struct {
    float Left, Right, Bottom, Top; //for the projection
    float position[2];

    int textCount;
    char buffer[256]; //the text basically, since this isn't C++ then im just going to preallocate an array
    float textScale;
} TextDisplayer;

void SetText(TextDisplayer* pText, char* text, int textCount);
void RenderText(TextDisplayer* pText);

#endif