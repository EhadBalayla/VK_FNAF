#include "Game.h"

#include <cglm/cglm.h>

Game* GGame = NULL;

//function declarations
void Window_Resize(GLFWwindow* GLFWwindow, int width, int height);



void Game_Init(Game* pGame) {
    pGame->Width = 1600;
    pGame->Height = 900;

    //create the Window and initialize context and swapchain
    Window_InitGLFW();
    Window_CreateWindow(&pGame->m_Window, pGame->Width, pGame->Height, "VK_FNAF");
    Window_CreateContext(&pGame->m_Window);

    //set the window callbacks
    Window_SetResizeCallback(&pGame->m_Window, Window_Resize);

    //give the renderer the necessary handles and initialize the renderer
    pGame->m_Renderer.instance = pGame->m_Window.m_Context.instance;
    pGame->m_Renderer.debugMessenger = pGame->m_Window.m_Context.debugMessenger;
    pGame->m_Renderer.physicalDevice = pGame->m_Window.m_Context.physicalDevice;
    pGame->m_Renderer.device = pGame->m_Window.m_Context.device;
    pGame->m_Renderer.graphicsFamily = pGame->m_Window.m_Context.graphicsFamily;
    pGame->m_Renderer.presentFamily = pGame->m_Window.m_Context.presentFamily;
    pGame->m_Renderer.graphicsQueue = pGame->m_Window.m_Context.graphicsQueue;
    pGame->m_Renderer.PresentQueue = pGame->m_Window.m_Context.PresentQueue;
    pGame->m_Renderer.MaxFramesInFlight = MAX_FRAMES_IN_FLIGHT;
    Renderer_Init(&pGame->m_Renderer);


    //load in the textures
    LoadTexture(&pGame->officeTexture, "Textures/debugTex.jpeg");

    Renderer_CreateSets(&pGame->m_Renderer);

    //load in the shaders
    Shader_Load(&pGame->firstShader, "Shaders/BaseShader_vert.spv", "Shaders/BaseShader_frag.spv");
}
void Game_Loop(Game* pGame) {
    while(!Window_ShouldClose(&pGame->m_Window)) {
        Window_PollEvents();
        Window_StartFrame(&pGame->m_Window);

        mat4 proj;
        glm_ortho(-pGame->Width / 2.0f, pGame->Width / 2.0f, pGame->Height / 2.0f, -pGame->Height / 2.0f, -1.0, 1.0, proj);
        proj[1][1] *= -1;
        mat4 trans;
        glm_mat4_identity(trans);
        glm_scale(trans, (vec3){300.0f, 300.0f, 1.0f});

        mat4 overall;
        glm_mat4_mul(proj, trans, overall);

        vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->firstShader.pipelineLayout, 0, 1, &pGame->m_Renderer.officeTextureSets[currentFrame], 0, NULL);
        vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->firstShader.graphicsPipeline);
        vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->firstShader.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall);
        vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);

        Window_EndFrame(&pGame->m_Window);
    }
}
void Game_Terminate(Game* pGame) {
    Renderer_wait(&pGame->m_Renderer);

    Shader_Delete(&pGame->firstShader);

    DeleteTexture(&pGame->officeTexture);

    Renderer_Terminate(&pGame->m_Renderer);

    Window_DestroyContext(&pGame->m_Window);
    Window_DestroyWindow(&pGame->m_Window);
    Window_TerminateGLFW();
}



void Window_Resize(GLFWwindow* GLFWwindow, int width, int height) {
    GGame->Width = width;
    GGame->Height = height;
    
    VKSwapchain_Recreate(&GGame->m_Window.m_Swapchain);
}