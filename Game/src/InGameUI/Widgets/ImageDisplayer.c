#include "ImageDisplayer.h"

#include "../../core/Game.h"
#include <cglm/cglm.h>

void Render_Image(ImageDisplayer* pImage) {
    mat4 trans;
    glm_mat4_identity(trans);
    glm_translate(trans, (vec3){pImage->position[0], pImage->position[1], 0.0f});
    glm_scale(trans, (vec3){pImage->scale[0], pImage->scale[1], 1.0f});
    
    mat4 overall;
    glm_mat4_identity(overall);
    glm_mat4_mul(pImage->projMat, trans, overall);

    vkCmdBindDescriptorSets(GGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, GGame->m_Renderer.pipelineLayout, 0, 1, &GGame->m_Renderer.textureSets[pImage->texID][currentFrame], 0, NULL);
    vkCmdBindPipeline(GGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, GGame->UIShader.graphicsPipeline);
    vkCmdPushConstants(GGame->m_Renderer.commandBuffers[currentFrame], GGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall);
    vkCmdDraw(GGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
}