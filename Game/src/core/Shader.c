#include "Shader.h"

#include "Game.h"
#include <stdio.h>
#include <stdlib.h>

char* ReadFile(const char* filename, size_t* returnedSize) {
    FILE* file = fopen(filename, "rb");
    if(file == NULL) {
        fprintf(stderr, "failed to open file for shader");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);

    size_t fileSize = ftell(file);

    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(fileSize);

    fread(buffer, fileSize, 1, file);

    fclose(file);

    *returnedSize = fileSize;
    return buffer;
}
VkShaderModule createShaderModule(char* code, size_t codeSize) {
    VkShaderModuleCreateInfo moduleInfo = {0};
    moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleInfo.pCode = code;
    moduleInfo.codeSize = codeSize;

    VkShaderModule shaderModule;
    if(vkCreateShaderModule(GGame->m_Renderer.device, &moduleInfo, NULL, &shaderModule) != VK_SUCCESS) {
        fprintf(stderr, "failed to create shader module for graphics pipeline");
        exit(EXIT_FAILURE);
    }

    return shaderModule;
}


void Shader_Load(Shader* pShader, const char* vertexFile, const char* fragmentFile) {
    size_t vertexCodeSize, fragmentCodeSize;
    char* vertexCode = ReadFile(vertexFile, &vertexCodeSize);
    char* fragmentCode = ReadFile(fragmentFile, &fragmentCodeSize);

    VkShaderModule vertexModule = createShaderModule(vertexCode, vertexCodeSize);
    VkShaderModule fragmentModule = createShaderModule(fragmentCode, fragmentCodeSize);

    //remember to free the allocated buffers for the code after we finish loading the shader modules
    free(vertexCode);
    free(fragmentCode);

    //creating the vertex and fragment stages
    VkPipelineShaderStageCreateInfo vertexStage = {0};
    vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStage.module = vertexModule;
    vertexStage.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentStage = {0};
    fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStage.module = fragmentModule;
    fragmentStage.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertexStage, fragmentStage};



    //creating the dynamic states
	uint32_t dynamicStatesCount = 2;
	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
	};

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {0};
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.dynamicStateCount = dynamicStatesCount;
	dynamicStateCreateInfo.pDynamicStates = dynamicStates;



    //creating a proxy viewport state even though we are using them as dynamic states
    VkPipelineViewportStateCreateInfo viewportState = {0};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pViewports = NULL;
    viewportState.viewportCount = 1;
    viewportState.pScissors = NULL;
    viewportState.scissorCount = 1;



    //creating the vertex input, since its a 2D game then it is going to be 100% null
    VkPipelineVertexInputStateCreateInfo vertexInputState = {0};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.vertexAttributeDescriptionCount = 0;
    vertexInputState.pVertexAttributeDescriptions = NULL;
    vertexInputState.vertexBindingDescriptionCount = 0;
    vertexInputState.pVertexBindingDescriptions = NULL;



    //creating the assembly input state
    VkPipelineInputAssemblyStateCreateInfo assemblyState = {0};
    assemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assemblyState.primitiveRestartEnable = VK_FALSE;



    //creating the rasterizer state
    VkPipelineRasterizationStateCreateInfo rasterizerState = {0};
    rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerState.depthClampEnable = VK_FALSE;
	rasterizerState.rasterizerDiscardEnable = VK_FALSE;
	rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizerState.lineWidth = 1.0f;
	rasterizerState.cullMode = VK_CULL_MODE_NONE;
	rasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizerState.depthBiasEnable = VK_FALSE;
	rasterizerState.depthBiasConstantFactor = 0.0f;
	rasterizerState.depthBiasClamp = 0.0f;
	rasterizerState.depthBiasSlopeFactor = 0.0f;



    //creating the multisampler state
    VkPipelineMultisampleStateCreateInfo multisamplingState = {0};
    multisamplingState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingState.sampleShadingEnable = VK_FALSE;
	multisamplingState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;



    //creating the color blending state
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState attachments[] = { colorBlendAttachment };

	VkPipelineColorBlendStateCreateInfo colorBlendState = {0};
	colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendState.logicOpEnable = VK_FALSE;
	colorBlendState.attachmentCount = 1;
	colorBlendState.pAttachments = attachments;



    //creating the depth state
    VkPipelineDepthStencilStateCreateInfo depthStencilState = {0};
	depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilState.depthTestEnable = VK_FALSE;
	depthStencilState.depthWriteEnable = VK_FALSE;
	depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilState.depthBoundsTestEnable = VK_FALSE;
	depthStencilState.minDepthBounds = 0.0f;
	depthStencilState.maxDepthBounds = 1.0f;
	depthStencilState.stencilTestEnable = VK_FALSE;



    //creating a temporary pipeline layout (later on i will hardcode the pipeline layout)
    VkPushConstantRange range = {0};
    range.offset = 0;
    range.size = sizeof(float) * 16; //the size of a mat4 too, thing is i dont wanna include cglm in here
    range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &range;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &GGame->m_Renderer.singleTexLayout;

    if(vkCreatePipelineLayout(GGame->m_Renderer.device, &pipelineLayoutInfo, NULL, &pShader->pipelineLayout) != VK_SUCCESS) {
        fprintf(stderr, "failed to create temporary pipeline layout");
        exit(EXIT_FAILURE);
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = {0};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pInputAssemblyState = &assemblyState;
	pipelineInfo.pVertexInputState = &vertexInputState;
	pipelineInfo.pMultisampleState = &multisamplingState;
	pipelineInfo.pRasterizationState = &rasterizerState;
	pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pColorBlendState = &colorBlendState;
	pipelineInfo.pDepthStencilState = &depthStencilState;
    pipelineInfo.layout = pShader->pipelineLayout;
	pipelineInfo.renderPass = GGame->m_Window.m_Swapchain.swapchainRenderPass;
	pipelineInfo.subpass = 0;

    if(vkCreateGraphicsPipelines(GGame->m_Renderer.device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pShader->graphicsPipeline) != VK_SUCCESS) {
        fprintf(stderr, "failed to create graphics pipeline of one of the shaders");
        exit(EXIT_FAILURE);
    }

    vkDestroyShaderModule(GGame->m_Renderer.device, vertexModule, NULL);
    vkDestroyShaderModule(GGame->m_Renderer.device, fragmentModule, NULL);
}
void Shader_Delete(Shader* pShader) {
    vkDestroyPipeline(GGame->m_Renderer.device, pShader->graphicsPipeline, NULL);
    vkDestroyPipelineLayout(GGame->m_Renderer.device, pShader->pipelineLayout, NULL);
}