#include "Game.h"

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

//small helper function
float fclamp(float value, float min, float max) {
    if(value < min) value = min;
    else if (value > max) value = max;

    return value;
}
//temporary parameters
double LastTime = 0.0;

double lastMouseX = 0.0;
double lastMouseY = 0.0;

float fanSequence = 0.0f;
float monitorFlipSequence = 0.0f;

char* textureLocations[] = {
    "Textures/Debug_Office.png",
    "Textures/Debug_Fan_Reborn.png",
    "Textures/debug_monitorFlip.jpg",
    "Textures/Debug_MonitorFlip.png"
};



Game* GGame = NULL;
float Center[16];
float Center_Bottom[16];
float Left_Top[16];

//function declarations
void Window_Resize(GLFWwindow* GLFWwindow, int width, int height);
void Window_KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void Window_MouseCallback(GLFWwindow *window, double xpos, double ypos);
void Window_MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

void Game_Tick(Game* pGame); //basically a function dedicated to ticking the game, instead of needing to spam logic in Game_Loop
void Game_RenderGameLayer(Game* pGame); //a function dedicated to rendering what goes into the offscreen buffer instead of spamming logic in Game_Loop
void Game_RenderUILayer(Game* pGame); //a function dedicated to rendering the in game UI instead of needing to spam logic in Game_Loop

//helper functions for rendering the in game renders and a few miscaleanous (like the monitor flip)
void Game_RenderOffice(Game* pGame);
void Game_RenderMonitorFlip(Game* pGame);


void Game_Init(Game* pGame) {
    //initializing parameters
    pGame->Width = 1600;
    pGame->Height = 900;

    pGame->DeltaTime = 0.0;

    pGame->horizontalScroll = 0.0f;

    pGame->MouseX = 0.0;
    pGame->MouseY = 0.0;

    pGame->GameTime = 0.0f;

    pGame->SelectedButton = NULL;

    pGame->states = Office;

    //create the Window and initialize context and swapchain
    Window_InitGLFW();
    Window_CreateWindow(&pGame->m_Window, pGame->Width, pGame->Height, "VK_FNAF");
    Window_CreateContext(&pGame->m_Window);

    //set the window callbacks
    Window_SetResizeCallback(&pGame->m_Window, Window_Resize);
    Window_SetKeyCallback(&pGame->m_Window, Window_KeyCallback);
    Window_SetMouseCallback(&pGame->m_Window, Window_MouseCallback);
    Window_SetMouseButtonCallback(&pGame->m_Window, Window_MouseButtonCallback);

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
    for(int i = 0; i < MAX_RENDERS; i++) {
        LoadTexture(&pGame->allTextures[i], textureLocations[i]);
    }
    LoadFont(&pGame->m_Font); //also loads the font texture automatically
    Renderer_CreateSets(&pGame->m_Renderer); //after finishing loading the textures, create descriptor sets for them
    
    //load in the shaders
    Shader_Load(&pGame->FullscreenShader, "Shaders/FullscreenShader_vert.spv", "Shaders/FullscreenShader_frag.spv", 1, 0);
    Shader_Load(&pGame->firstShader, "Shaders/BaseShader_vert.spv", "Shaders/BaseShader_frag.spv", 0, 0);
    Shader_Load(&pGame->atlasShader, "Shaders/AtlasShader_vert.spv", "Shaders/AtlasShader_frag.spv", 0, 0);
    Shader_Load(&pGame->UIShader, "Shaders/BaseShader_vert.spv", "Shaders/BaseShader_frag.spv", 1, 0); //this is the same as the first shader, just for the swapchain and not the offscreen buffer
    Shader_Load(&pGame->TextShader, "Shaders/TextShader_vert.spv", "Shaders/TextShader_frag.spv", 1, 1);
    Shader_Load(&pGame->UIAtlasShader, "Shaders/AtlasShader_vert.spv", "Shaders/AtlasShader_frag.spv", 1, 1); //same as the atlas shader but for the swapchain and with alpha blending

    //one last thing, initialize the in game UI
    OfficeHUDScreen_Initialize(&pGame->officeHUD);
    MonitorHUDScreen_Initialize(&pGame->monitorHUD);
}
void Game_Loop(Game* pGame) {
    while(!Window_ShouldClose(&pGame->m_Window)) {
        Window_PollEvents();
        Window_StartFrame(&pGame->m_Window);

        //simply updating DeltaTime
        double currentTime = glfwGetTime();
        pGame->DeltaTime = currentTime - LastTime;
        LastTime = currentTime;

        //ticking the game
        Game_Tick(pGame);

        //draw the offscreen buffer
        Renderer_StartDraw(&pGame->m_Renderer);

        Game_RenderGameLayer(pGame);

        Renderer_EndDraw(&pGame->m_Renderer);

        

        //draw the swapchain
        Window_StartScreen(&pGame->m_Window);

        Game_RenderUILayer(pGame);

        Window_EndScreen(&pGame->m_Window);



        Window_EndFrame(&pGame->m_Window);
    }
}
void Game_Terminate(Game* pGame) {
    Renderer_wait(&pGame->m_Renderer);

    Shader_Delete(&pGame->UIAtlasShader);
    Shader_Delete(&pGame->TextShader);
    Shader_Delete(&pGame->UIShader);
    Shader_Delete(&pGame->firstShader);
    Shader_Delete(&pGame->FullscreenShader);
    Shader_Delete(&pGame->atlasShader);

    DeleteTexture(&pGame->fontTexture);
    for(int i = 0; i < MAX_RENDERS; i++) {
        DeleteTexture(&pGame->allTextures[i]);
    }

    Renderer_Terminate(&pGame->m_Renderer);

    Window_DestroyContext(&pGame->m_Window);
    Window_DestroyWindow(&pGame->m_Window);
    Window_TerminateGLFW();
}



void Window_Resize(GLFWwindow* GLFWwindow, int width, int height) {
    GGame->Width = width;
    GGame->Height = height;

    Renderer_wait(&GGame->m_Renderer);

    VKSwapchain_Recreate(&GGame->m_Window.m_Swapchain);
    Renderer_RecreateOffscreenFramebuffer(&GGame->m_Renderer);
}
void Window_KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {

}
void Window_MouseCallback(GLFWwindow *window, double xpos, double ypos) {
    GGame->MouseX = xpos;
    GGame->MouseY = ypos;
}
void Window_MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if(action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
        if(GGame->SelectedButton != NULL) {
            UIButton* button = GGame->SelectedButton;
            button->OnClick(button);
        }
    }
}

void Game_Tick(Game* pGame) {
    pGame->GameTime += pGame->DeltaTime;

    //apply the orthographic projection matrices
    glm_ortho(-pGame->Width / 2.0f, pGame->Width / 2.0f, -pGame->Height / 2.0f, pGame->Height / 2.0f, -1.0f, 1.0f, (vec4*)Center);
    glm_ortho(-pGame->Width / 2.0f, pGame->Width / 2.0f, pGame->Height, 0.0f, -1.0f, 1.0f, (vec4*)Center_Bottom);
    glm_ortho(0.0f, pGame->Width, 0.0f, pGame->Height, -1.0f, 1.0f, (vec4*)Left_Top);
    
    switch(pGame->states) {
        case Office: {
            OfficeHUDScreen_Update(&pGame->officeHUD);
            break;
        }
        case FlippingUp: {
            monitorFlipSequence += pGame->DeltaTime;
            if(monitorFlipSequence >= 0.1f) {
                monitorFlipSequence = 0.1f;
                pGame->states = Monitor;
            }
            break;
        }
        case Monitor: {
            MonitorHUDScreen_Update(&pGame->monitorHUD);
            break;
        }
        case FlippingDown: {
            monitorFlipSequence -= pGame->DeltaTime;
            if(monitorFlipSequence <= 0.0f) {
                monitorFlipSequence = 0.0f;
                pGame->states = Office;
            }
            break;
        }
    }
}
void Game_RenderGameLayer(Game* pGame) {
    switch(pGame->states) {
        case Office: {
            Game_RenderOffice(pGame);
            break;
        }
        case FlippingUp: {
            Game_RenderOffice(pGame);
            break;
        }
        case Monitor: {

            break;
        }
    }
}
void Game_RenderUILayer(Game* pGame) {
    switch(pGame->states) {
        case Office: {
            OfficeHUDScreen_Render(&pGame->officeHUD);
            break;
        }
        case FlippingUp: {
            Game_RenderMonitorFlip(pGame);
            break;
        }
        case Monitor: {
            MonitorHUDScreen_Render(&pGame->monitorHUD);
            break;
        }
        case FlippingDown: {
            Game_RenderMonitorFlip(pGame);
            break;
        }
    }
}


void Game_RenderOffice(Game* pGame) {
    double midRelX = GGame->MouseX - GGame->Width / 2.0;
    double maxDisFromMid = GGame->Width / 2.0;
    if(midRelX < -(maxDisFromMid / 2.0)) {
        float scale = -(midRelX + maxDisFromMid / 2.0);
        GGame->horizontalScroll += 10.0f * scale * GGame->DeltaTime;
    }
    else if(midRelX > maxDisFromMid / 2.0) {
        float scale = midRelX - maxDisFromMid / 2.0;
        GGame->horizontalScroll -= 10.0f * scale * GGame->DeltaTime;
    }
    GGame->horizontalScroll = fclamp(GGame->horizontalScroll, -798.514893, 798.514893);

    mat4 proj;
    glm_ortho(-pGame->Width / 2.0f, pGame->Width / 2.0f, pGame->Height / 2.0f, -pGame->Height / 2.0f, -1.0, 1.0, proj);
    proj[1][1] *= -1;

    mat4 trans;
    glm_mat4_identity(trans);
    glm_translate(trans, (vec3){pGame->horizontalScroll, 0.0f, 0.0f});
    glm_scale(trans, (vec3){pGame->Width, pGame->Height, 1.0f});

    mat4 overall;
    glm_mat4_mul(proj, trans, overall);


    //render office
    vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[OFFICE][currentFrame], 0, NULL);
    vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->firstShader.graphicsPipeline);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall);
    vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
    
    //render office fan
    fanSequence += pGame->DeltaTime * 50.0f;
    int fanFrames = 24;
    int fanIdx = (int)fanSequence % 24;

    mat4 trans2;
    glm_mat4_identity(trans2);
    glm_translate(trans2, (vec3){-340.0f + pGame->horizontalScroll, 232.0f, 0.0f});
    glm_scale(trans2, (vec3){132.0f, 160.0f, 1.0f});

    mat4 overall2;
    glm_mat4_mul(proj, trans2, overall2);

    vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[OFFICE_FAN][currentFrame], 0, NULL);
    vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->atlasShader.graphicsPipeline);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall2);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4), sizeof(int), &fanIdx);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4) + sizeof(int), sizeof(int), &fanFrames);
    vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
}
void Game_RenderMonitorFlip(Game* pGame) {
    int flippingFrame = monitorFlipSequence * 10.0f * 24.0f;
    int flipFrames = 24;

    mat4 pos;
    glm_mat4_identity(pos);
    glm_scale(pos, (vec3){pGame->Width / 2.0f, pGame->Height / 2.0f, 0.0f});

    mat4 overall;
    glm_mat4_identity(overall);
    glm_mat4_mul(Center, pos, overall);

    vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[MONITOR_FLIP][currentFrame], 0, NULL);
    vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->UIAtlasShader.graphicsPipeline);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame], pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame], pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4), sizeof(float), &flippingFrame);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame], pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4) + sizeof(float), sizeof(float), &flipFrames);
    vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
}
