#include "UIHoverable.h"

#include "../../core/Game.h"
#include <cglm/cglm.h>

void UpdateHoverable(UIHoverable* pHoverable) {
    float* m = pHoverable->projMat;

    float halfWidth = 1.0f / m[0];
    float halfHeight = 1.0f / m[5];
    float centerX = -m[12] / m[0];
    float centerY = -m[13] / m[5];

    float Right  = centerX + halfWidth;
    float Bottom = centerY - halfHeight;
    float Left   = centerX - halfWidth;
    float Top    = centerY + halfHeight;
    
    float realXPos = (pHoverable->position[0] - Left) / (Right - Left) * GGame->Width;
    float realYPos = (Bottom - pHoverable->position[1]) / (Bottom - Top) * GGame->Height;

    double MouseX = GGame->MouseX;
    double MouseY = GGame->MouseY;

    if ((MouseX > realXPos - pHoverable->scale[0] && MouseX < realXPos + pHoverable->scale[0]) &&
		(MouseY > realYPos - pHoverable->scale[1] && MouseY < realYPos + pHoverable->scale[1])) {
        if(!pHoverable->IsHovered) {
            pHoverable->IsHovered = 1;
            pHoverable->OnHovered(NULL);
        }
    }
    else {
        if(pHoverable->IsHovered) {
            pHoverable->IsHovered = 0;
        }
    }
}
void RenderHoverable(UIHoverable* pHoverable) {
    mat4 trans;
    glm_mat4_identity(trans);
    glm_translate(trans, (vec3){pHoverable->position[0], pHoverable->position[1], 0.0f});
    glm_scale(trans, (vec3){pHoverable->scale[0], pHoverable->scale[1], 1.0f});

    mat4 overall;
    glm_mat4_identity(overall);
    glm_mat4_mul(pHoverable->projMat, trans, overall);

    vkCmdBindDescriptorSets(GGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, GGame->m_Renderer.pipelineLayout, 0, 1, &GGame->m_Renderer.textureSets[pHoverable->texID][currentFrame], 0, NULL);
    vkCmdBindPipeline(GGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, GGame->UIShader.graphicsPipeline);
    vkCmdPushConstants(GGame->m_Renderer.commandBuffers[currentFrame], GGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall);
    vkCmdDraw(GGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
}
