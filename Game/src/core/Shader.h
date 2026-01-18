#ifndef SHADER
#define SHADER

#include <vulkan/vulkan.h>

typedef struct {
    VkPipeline graphicsPipeline;
} Shader;

void Shader_Load(Shader* pShader, const char* vertexFile, const char* fragmentFile, int IsScreen);
void Shader_Delete(Shader* pShader);

#endif