#include "Texture.h"

#include <stb_image.h>
#include <string.h>
#include "Game.h"

void LoadTexture(Texture* pTexture, const char* path) {
    int Width, Height, Channels;
    unsigned char* data = stbi_load(path, &Width, &Height, &Channels, 4); //loading the image as RGBA
    if(data == NULL) {
        fprintf(stderr, "failed to load the pixel data of a texture");
        exit(EXIT_FAILURE);
    }

    VkDeviceSize imageSize = Width * Height * 4;
    

    //allocating a staging buffer and copying the image data to the staging buffer
    VkBuffer stagingBuffer;
    VmaAllocation stagingAllocation;

    VkBufferCreateInfo stagingBufferInfo = {0};
    stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferInfo.size = imageSize;
    stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo stagingAllocInfo = {0};
    stagingAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    if(vmaCreateBuffer(GGame->m_Renderer.allocator, &stagingBufferInfo, &stagingAllocInfo, &stagingBuffer, &stagingAllocation, NULL) != VK_SUCCESS) {
        fprintf(stderr, "failed to create staging buffer for loading one of the textures");
        exit(EXIT_FAILURE);
    }

    void* pData;
    vmaMapMemory(GGame->m_Renderer.allocator, stagingAllocation, &pData);
    memcpy(pData, data, Width * Height * 4);
    vmaUnmapMemory(GGame->m_Renderer.allocator, stagingAllocation);



    //creating the image with undefined layout
    VkImageCreateInfo imageInfo = {0};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.arrayLayers = 1;
    imageInfo.mipLevels = 1;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.extent.width = Width;
    imageInfo.extent.height = Height;
    imageInfo.extent.depth = 1;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;

    VmaAllocationCreateInfo allocInfo = {0};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    if(vmaCreateImage(GGame->m_Renderer.allocator, &imageInfo, &allocInfo, &pTexture->image, &pTexture->allocation, NULL) != VK_SUCCESS) {
        fprintf(stderr, "failed to create the texture of an image");
        exit(EXIT_FAILURE);
    }


    //creating a single use command buffer, for the transfer operation and image layout transfer
    VkCommandBuffer tempBuffer;
    VkCommandBufferAllocateInfo tempBufferAllocInfo = {0};
    tempBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    tempBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    tempBufferAllocInfo.commandPool = GGame->m_Renderer.commandPool;
    tempBufferAllocInfo.commandBufferCount = 1;
    if(vkAllocateCommandBuffers(GGame->m_Renderer.device, &tempBufferAllocInfo, &tempBuffer) != VK_SUCCESS) {
        fprintf(stderr, "failed to allocate single use command buffer");
        exit(EXIT_FAILURE);
    }

    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if(vkBeginCommandBuffer(tempBuffer, &beginInfo) != VK_SUCCESS) {
        fprintf(stderr, "failed to start single use command buffer");
        exit(EXIT_FAILURE);
    }

    //transitioning image to transfer_dst_optimal
    VkImageMemoryBarrier imageBarrier = {0};
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.image = pTexture->image;
    imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrier.subresourceRange.baseMipLevel = 0;
    imageBarrier.subresourceRange.levelCount = 1;
    imageBarrier.subresourceRange.baseArrayLayer = 0;
    imageBarrier.subresourceRange.layerCount = 1;
    imageBarrier.srcAccessMask = 0;
    imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

    vkCmdPipelineBarrier(tempBuffer, 
    srcStage, dstStage, 
    0,
    0, NULL,
    0, NULL,
    1, &imageBarrier);


    //copying from the staging buffer to the image
    VkBufferImageCopy region = {0};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

    region.imageOffset = (VkOffset3D){0};
    region.imageExtent = (VkExtent3D){Width, Height, 1};

    vkCmdCopyBufferToImage(tempBuffer, stagingBuffer, pTexture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);


    //transitioning image to shader_read_only
    VkImageMemoryBarrier imageBarrier2 = {0};
    imageBarrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrier2.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageBarrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier2.image = pTexture->image;
    imageBarrier2.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrier2.subresourceRange.baseMipLevel = 0;
    imageBarrier2.subresourceRange.levelCount = 1;
    imageBarrier2.subresourceRange.baseArrayLayer = 0;
    imageBarrier2.subresourceRange.layerCount = 1;
    imageBarrier2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrier2.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    VkPipelineStageFlags srcStage2 = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkPipelineStageFlags dstStage2 = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    vkCmdPipelineBarrier(tempBuffer, 
    srcStage2, dstStage2, 
    0,
    0, NULL,
    0, NULL,
    1, &imageBarrier2);


    //ending the single use command buffer and freeing it
    vkEndCommandBuffer(tempBuffer);

    VkSubmitInfo tempSubmitInfo = {0};
    tempSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    tempSubmitInfo.commandBufferCount = 1;
    tempSubmitInfo.pCommandBuffers = &tempBuffer;
    vkQueueSubmit(GGame->m_Renderer.graphicsQueue, 1, &tempSubmitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(GGame->m_Renderer.graphicsQueue);

    vkFreeCommandBuffers(GGame->m_Renderer.device, GGame->m_Renderer.commandPool, 1, &tempBuffer);

    //destroying the staging buffer
    vmaDestroyBuffer(GGame->m_Renderer.allocator, stagingBuffer, stagingAllocation);

    
    //creating the image view
    VkImageViewCreateInfo imageViewInfo = {0};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.image = pTexture->image;
    imageViewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;

    if(vkCreateImageView(GGame->m_Renderer.device, &imageViewInfo, NULL, &pTexture->imageView) != VK_SUCCESS) {
        fprintf(stderr, "failed to create image view of an image");
        exit(EXIT_FAILURE);
    }


    stbi_image_free(data);
}
void LoadTexturePixels(Texture* pTexture, unsigned char* data, int Width, int Height) {
    //literally the same functoin as LoadTexture() but without a filepath

    VkDeviceSize imageSize = Width * Height * 4;
    

    //allocating a staging buffer and copying the image data to the staging buffer
    VkBuffer stagingBuffer;
    VmaAllocation stagingAllocation;

    VkBufferCreateInfo stagingBufferInfo = {0};
    stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferInfo.size = imageSize;
    stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo stagingAllocInfo = {0};
    stagingAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    if(vmaCreateBuffer(GGame->m_Renderer.allocator, &stagingBufferInfo, &stagingAllocInfo, &stagingBuffer, &stagingAllocation, NULL) != VK_SUCCESS) {
        fprintf(stderr, "failed to create staging buffer for loading one of the textures");
        exit(EXIT_FAILURE);
    }

    void* pData;
    vmaMapMemory(GGame->m_Renderer.allocator, stagingAllocation, &pData);
    memcpy(pData, data, Width * Height * 4);
    vmaUnmapMemory(GGame->m_Renderer.allocator, stagingAllocation);



    //creating the image with undefined layout
    VkImageCreateInfo imageInfo = {0};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.arrayLayers = 1;
    imageInfo.mipLevels = 1;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.extent.width = Width;
    imageInfo.extent.height = Height;
    imageInfo.extent.depth = 1;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;

    VmaAllocationCreateInfo allocInfo = {0};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    if(vmaCreateImage(GGame->m_Renderer.allocator, &imageInfo, &allocInfo, &pTexture->image, &pTexture->allocation, NULL) != VK_SUCCESS) {
        fprintf(stderr, "failed to create the texture of an image");
        exit(EXIT_FAILURE);
    }


    //creating a single use command buffer, for the transfer operation and image layout transfer
    VkCommandBuffer tempBuffer;
    VkCommandBufferAllocateInfo tempBufferAllocInfo = {0};
    tempBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    tempBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    tempBufferAllocInfo.commandPool = GGame->m_Renderer.commandPool;
    tempBufferAllocInfo.commandBufferCount = 1;
    if(vkAllocateCommandBuffers(GGame->m_Renderer.device, &tempBufferAllocInfo, &tempBuffer) != VK_SUCCESS) {
        fprintf(stderr, "failed to allocate single use command buffer");
        exit(EXIT_FAILURE);
    }

    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if(vkBeginCommandBuffer(tempBuffer, &beginInfo) != VK_SUCCESS) {
        fprintf(stderr, "failed to start single use command buffer");
        exit(EXIT_FAILURE);
    }

    //transitioning image to transfer_dst_optimal
    VkImageMemoryBarrier imageBarrier = {0};
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.image = pTexture->image;
    imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrier.subresourceRange.baseMipLevel = 0;
    imageBarrier.subresourceRange.levelCount = 1;
    imageBarrier.subresourceRange.baseArrayLayer = 0;
    imageBarrier.subresourceRange.layerCount = 1;
    imageBarrier.srcAccessMask = 0;
    imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

    vkCmdPipelineBarrier(tempBuffer, 
    srcStage, dstStage, 
    0,
    0, NULL,
    0, NULL,
    1, &imageBarrier);


    //copying from the staging buffer to the image
    VkBufferImageCopy region = {0};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

    region.imageOffset = (VkOffset3D){0};
    region.imageExtent = (VkExtent3D){Width, Height, 1};

    vkCmdCopyBufferToImage(tempBuffer, stagingBuffer, pTexture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);


    //transitioning image to shader_read_only
    VkImageMemoryBarrier imageBarrier2 = {0};
    imageBarrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrier2.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageBarrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier2.image = pTexture->image;
    imageBarrier2.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrier2.subresourceRange.baseMipLevel = 0;
    imageBarrier2.subresourceRange.levelCount = 1;
    imageBarrier2.subresourceRange.baseArrayLayer = 0;
    imageBarrier2.subresourceRange.layerCount = 1;
    imageBarrier2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrier2.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    VkPipelineStageFlags srcStage2 = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkPipelineStageFlags dstStage2 = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    vkCmdPipelineBarrier(tempBuffer, 
    srcStage2, dstStage2, 
    0,
    0, NULL,
    0, NULL,
    1, &imageBarrier2);


    //ending the single use command buffer and freeing it
    vkEndCommandBuffer(tempBuffer);

    VkSubmitInfo tempSubmitInfo = {0};
    tempSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    tempSubmitInfo.commandBufferCount = 1;
    tempSubmitInfo.pCommandBuffers = &tempBuffer;
    vkQueueSubmit(GGame->m_Renderer.graphicsQueue, 1, &tempSubmitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(GGame->m_Renderer.graphicsQueue);

    vkFreeCommandBuffers(GGame->m_Renderer.device, GGame->m_Renderer.commandPool, 1, &tempBuffer);

    //destroying the staging buffer
    vmaDestroyBuffer(GGame->m_Renderer.allocator, stagingBuffer, stagingAllocation);

    
    //creating the image view
    VkImageViewCreateInfo imageViewInfo = {0};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.image = pTexture->image;
    imageViewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;

    if(vkCreateImageView(GGame->m_Renderer.device, &imageViewInfo, NULL, &pTexture->imageView) != VK_SUCCESS) {
        fprintf(stderr, "failed to create image view of an image");
        exit(EXIT_FAILURE);
    }
}
void DeleteTexture(Texture* pTexture) {
    vmaDestroyImage(GGame->m_Renderer.allocator, pTexture->image, pTexture->allocation);
    vkDestroyImageView(GGame->m_Renderer.device, pTexture->imageView, NULL);
}