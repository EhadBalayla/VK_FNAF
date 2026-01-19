#include "UIButton.h"

#include "../../core/Game.h"
#include <cglm/cglm.h>

#include <stdio.h>

void UpdateButton(UIButton* pButton) {
    /*float orthoWidth  = pButton->Right - pButton->Left;
    float orthoHeight = pButton->Bottom - pButton->Top;

    float realXPos = ((pButton->position[0] - pButton->Left) / orthoWidth) * GGame->Width;
    float realYPos = ((pButton->position[1] - pButton->Top) / orthoHeight) * GGame->Height;

    double MouseX = GGame->MouseX;
    double MouseY = GGame->MouseY;

    if ((MouseX > realXPos - pButton->scale[0] && MouseX < realXPos + pButton->scale[0]) &&
		(MouseY > realYPos - pButton->scale[1] && MouseY < realYPos + pButton->scale[1])) {
        if(!pButton->IsHovered) {
            pButton->IsHovered = 1;
            printf("hovering over a button\n");
        }
    }
    else {
        if(pButton->IsHovered) {
            pButton->IsHovered = 0;
        }
    }*/
}
void RenderButton(UIButton* pButton) {
    mat4 trans;
    glm_mat4_identity(trans);
    glm_translate(trans, (vec3){pButton->position[0], pButton->position[1], 0.0f});
    glm_scale(trans, (vec3){pButton->scale[0], pButton->scale[1], 1.0f});

    mat4 overall;
    glm_mat4_identity(overall);
    glm_mat4_mul(pButton->projMat, trans, overall);

    vkCmdBindDescriptorSets(GGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, GGame->m_Renderer.pipelineLayout, 0, 1, &GGame->m_Renderer.textureSets[pButton->texID][currentFrame], 0, NULL);
    vkCmdBindPipeline(GGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, GGame->UIShader.graphicsPipeline);
    vkCmdPushConstants(GGame->m_Renderer.commandBuffers[currentFrame], GGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall);
    vkCmdDraw(GGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
}
