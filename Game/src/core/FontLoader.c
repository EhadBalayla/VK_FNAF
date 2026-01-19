#include "FontLoader.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <stdlib.h>
#include "Game.h"

void LoadFont(FontLoader* pFont) {
    FT_Library ft;
    if(FT_Init_FreeType(&ft)) {
        fprintf(stderr, "failed to load the freetype library");
        exit(EXIT_FAILURE);
    }

    FT_Face face;
    if(FT_New_Face(ft, "font.ttf", 0, &face)) {
        fprintf(stderr, "failed to load font");
        exit(EXIT_FAILURE);
    }

    const int cellSize = 64;
    FT_Set_Pixel_Sizes(face, 0, cellSize); //so that each cell size of each glyph will be 64x64
    int atlasWidthLength = 16 * cellSize;
    unsigned char* pixelData = calloc(atlasWidthLength * atlasWidthLength * 4, 1);
    int cellIDX = 0;
    for(unsigned char c = 0; c < 128; c++) {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            fprintf(stderr, "failed to load one of the characters");
            exit(EXIT_FAILURE);
        }

        int cellX = cellIDX % 16;
        int cellY = cellIDX / 16;

        for(int x = 0; x < face->glyph->bitmap.width; x++) {
            for(int y = 0; y < face->glyph->bitmap.rows; y++) {
                int cursorX = cellX * cellSize + x;
                int cursorY = cellY * cellSize + y;

                pixelData[(cursorY * atlasWidthLength + cursorX) * 4] = 255;
                pixelData[(cursorY * atlasWidthLength + cursorX) * 4 + 1] = 255;
                pixelData[(cursorY * atlasWidthLength + cursorX) * 4 + 2] = 255;
                pixelData[(cursorY * atlasWidthLength + cursorX) * 4 + 3] = face->glyph->bitmap.buffer[x + y * face->glyph->bitmap.pitch];
            }
        }
        //getting the UV start of the grid
        pFont->glyphs[c].uvs[0] = (cellX * cellSize) / (float)atlasWidthLength;
        pFont->glyphs[c].uvs[1] = (cellY * cellSize) / (float)atlasWidthLength;
        
        //getting the UV offset
        pFont->glyphs[c].uvOffsets[0] = face->glyph->bitmap.width / (float)atlasWidthLength; //these two should already be in pixels
        pFont->glyphs[c].uvOffsets[1] = face->glyph->bitmap.rows / (float)atlasWidthLength;

        //getting the size
        pFont->glyphs[c].size[0] = face->glyph->bitmap.width;
        pFont->glyphs[c].size[1] = face->glyph->bitmap.rows;

        //getting the bearing
        pFont->glyphs[c].bearing[0] = face->glyph->bitmap_left;
        pFont->glyphs[c].bearing[1] = face->glyph->bitmap_top;

        //getting the advance
        pFont->glyphs[c].advance = face->glyph->advance.x;

        cellIDX++;
    }

    LoadTexturePixels(&GGame->fontTexture, pixelData, atlasWidthLength, atlasWidthLength);

    free(pixelData);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}