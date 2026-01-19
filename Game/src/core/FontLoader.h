#ifndef FONT_LOADER
#define FONT_LOADER

typedef struct {
    float uvs[2];
    float uvOffsets[2];
    int size[2];
    int bearing[2];
    unsigned int advance;
} Glyph;

typedef struct {
    Glyph glyphs[128];
} FontLoader;

void LoadFont(FontLoader* pFont);

#endif