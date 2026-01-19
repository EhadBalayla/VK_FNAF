#ifndef TEXTURE
#define TEXTURE

#include <vk_mem_alloc.h>

typedef struct {
    VkImage image;
    VkImageView imageView;
    VmaAllocation allocation; //since we are using VMA, this is instead of VkDeviceMemory
} Texture;

void LoadTexture(Texture* pTexture, const char* path);
void LoadTexturePixels(Texture* pTexture, unsigned char* data, int Width, int Height);
void DeleteTexture(Texture* pTexture);

#endif