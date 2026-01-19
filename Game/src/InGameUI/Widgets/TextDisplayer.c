#include "TextDisplayer.h"

#include <string.h>
#include "../../core/Game.h"

#include <cglm/cglm.h>

void SetText(TextDisplayer* pText, char* text, int textCount) {
    textCount = textCount > 256 ? 256 : textCount; //maxing textCount so that we wont overflow and cause a crash
    memcpy(pText->buffer, text, textCount);
    pText->textCount = textCount;
}
void RenderText(TextDisplayer* pText) {
    vkCmdBindDescriptorSets(GGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, GGame->m_Renderer.pipelineLayout, 0, 1, &GGame->m_Renderer.fontSets[currentFrame], 0, NULL);
    vkCmdBindPipeline(GGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, GGame->TextShader.graphicsPipeline);

    float cursorX = 0.0f;
    for(int i = 0; i < pText->textCount; i++) {
        Glyph* glyph = &GGame->m_Font.glyphs[pText->buffer[i]];

        float charXPos = pText->position[0] + (cursorX + glyph->bearing[0]) * pText->textScale;
        float charYPos = pText->position[1] - glyph->bearing[1] * pText->textScale;

        mat4 trans;
        glm_mat4_identity(trans);
        glm_translate(trans, (vec3){charXPos, charYPos, 0.0f});
        glm_scale(trans, (vec3){(glyph->size[0]) * pText->textScale, (glyph->size[1]) * pText->textScale, 1.0f});

        mat4 proj;
        glm_ortho(pText->Left, pText->Right, pText->Bottom, pText->Top, -1.0f, 1.0f, proj);

        mat4 overall;
        glm_mat4_identity(overall);
        glm_mat4_mul(proj, trans, overall);
    
        vkCmdPushConstants(GGame->m_Renderer.commandBuffers[currentFrame], GGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall);
        vkCmdPushConstants(GGame->m_Renderer.commandBuffers[currentFrame], GGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4), sizeof(vec2), glyph->uvs);
        vkCmdPushConstants(GGame->m_Renderer.commandBuffers[currentFrame], GGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4) + sizeof(vec2), sizeof(vec2), glyph->uvOffsets);
        vkCmdDraw(GGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
    
        cursorX += (glyph->advance >> 6) * pText->textScale;
    }
}