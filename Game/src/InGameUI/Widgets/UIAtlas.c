#include "UIAtlas.h"

#include <cglm/cglm.h>
#include "../../core/Game.h"

void RenderUIAtlas(UIAtlas* pUIAtlas) {
    int frame = (int)(pUIAtlas->sequence * (float)pUIAtlas->maxFrames) % pUIAtlas->maxFrames;

    mat4 trans;
    glm_mat4_identity(trans);
    glm_translate(trans, (vec3){pUIAtlas->position[0], pUIAtlas->position[1], 0.0f});
    glm_scale(trans, (vec3){pUIAtlas->scale[0], pUIAtlas->scale[1], 1.0f});
    
    mat4 overall;
    glm_mat4_identity(overall);
    glm_mat4_mul(pUIAtlas->projMat, trans, overall);

    vkCmdBindDescriptorSets(GGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, GGame->m_Renderer.pipelineLayout, 0, 1, &GGame->m_Renderer.textureSets[pUIAtlas->texID][currentFrame], 0, NULL);
    vkCmdBindPipeline(GGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, GGame->UIAtlasShader.graphicsPipeline);
    vkCmdPushConstants(GGame->m_Renderer.commandBuffers[currentFrame], GGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall);
    vkCmdPushConstants(GGame->m_Renderer.commandBuffers[currentFrame], GGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4), sizeof(float), &frame);
    vkCmdPushConstants(GGame->m_Renderer.commandBuffers[currentFrame], GGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4) + sizeof(float), sizeof(float), &pUIAtlas->maxFrames);
    vkCmdDraw(GGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);

    if(pUIAtlas->sequence >= 1.0f) pUIAtlas->sequence = 0.0f;
}