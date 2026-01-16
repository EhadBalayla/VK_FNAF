#ifndef SHADER
#define SHADER

#include <vulkan/vulkan.h>

typedef struct {
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
} Shader;

void Shader_Load(Shader* pShader, const char* vertexFile, const char* fragmentFile);
void Shader_Delete(Shader* pShader);

#endif