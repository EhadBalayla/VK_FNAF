#include "Game.h"

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

#include <stdlib.h> //for the rand()
#include <time.h> //for time(NULL)

//small helper function
float fclamp(float value, float min, float max) {
    if(value < min) value = min;
    else if (value > max) value = max;

    return value;
}
int GetRandomNumber(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

//temporary parameters
double LastTime = 0.0;

double lastMouseX = 0.0;
double lastMouseY = 0.0;

float fanSequence = 0.0f;
float monitorFlipSequence = 0.0f;
float LeftDoorSequence = 1.0f;
float RightDoorSequence = 1.0f;
float MiddleDoorSequence = 1.0f;
float JumpscareSequence = 0.0f;
float FeddyTimer = 0.0f;

char* textureLocations[] = {
    "Textures/Debug_Office.png",
    "Textures/Debug_Fan_Reborn.png",
    "Textures/debug_monitorFlip.jpg",
    "Textures/Debug_MonitorFlip.png",
    "Textures/Debug_CameraStatic.png",
    "Textures/Debug_CameraUnselected.png",
    "Textures/Debug_CameraSelected.png",
    "Textures/Debug_CameraStaticAlpha.png",
    "Textures/Debug_CAM1.png",
    "Textures/Debug_CAM2.png",
    "Textures/Debug_CAM3.png",
    "Textures/Debug_CAM4.png",
    "Textures/Debug_LeftDoor.png",
    "Textures/Debug_RightDoor.png",
    "Textures/Debug_MiddleDoor.png",
    "Textures/Debug_FeddyCam1.png",
    "Textures/Debug_FeddyCam2.png",
    "Textures/Debug_FeddyCam3.png",
    "Textures/Debug_FeddyCam4.png",
    "Textures/Debug_FeddyJumpscare.png"
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
void Game_RenderCam1(Game* pGame);
void Game_RenderCam2(Game* pGame);
void Game_RenderCam3(Game* pGame);
void Game_RenderCam4(Game* pGame);
void Game_RenderMonitorFlip(Game* pGame);
void Game_RenderFeddyJumpscare(Game* pGame);

//specifically for freeing the console on Windows
#ifdef _WIN32
#include <windows.h>
#endif

void Game_Init(Game* pGame) {
    //freeing the console on Windows
    #ifdef _WIN32
    FreeConsole();
    #endif

    srand(time(NULL));

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
    pGame->selectedCam = CAM1;
    pGame->feddyState = FeddyCam1;

    pGame->FeddyAI = 13;

    pGame->IsLeftDoorClosed = 0;
    pGame->IsRightDoorClosed = 0;
    pGame->IsMiddleDoorClosed = 0;

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

    //initializing the audio engine
    AudioPlayer_Init(&pGame->m_Audio);

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

    AudioPlayer_Terminate(&pGame->m_Audio);

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

    if(pGame->states != Jumpscare) {
        FeddyTimer += pGame->DeltaTime;
        if(FeddyTimer >= 7.0f) { //feddy moves every 7 seconds
            FeddyTimer = 0.0f;
            int rng = GetRandomNumber(1, 20);
            if(pGame->FeddyAI >= rng) {
                if(pGame->feddyState == FeddyCam1) pGame->feddyState = FeddyCam2;
                else if(pGame->feddyState == FeddyCam2) pGame->feddyState = FeddyCam3;
                else if(pGame->feddyState == FeddyCam3) pGame->feddyState = FeddyCam4;
                else if(pGame->feddyState == FeddyCam4) {
                    if(pGame->IsMiddleDoorClosed) {
                        pGame->feddyState = FeddyCam1;
                    }
                    else { 
                        pGame->feddyState = FeddyJumpscare;
                        pGame->states = Jumpscare;
                        AudioPlayer_Play2DSound(&pGame->m_Audio, "Sounds/Debug_Jumpscare.mp3");
                    }
                }
            }
        }
    }

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
            switch(pGame->selectedCam) {
                case CAM1:
                    Game_RenderCam1(pGame);
                break;
                case CAM2:
                    Game_RenderCam2(pGame);
                break;
                case CAM3:
                    Game_RenderCam3(pGame);
                break;
                case CAM4:
                    Game_RenderCam4(pGame);
                break;
            }
            break;
        }
        case FlippingDown: {
            Game_RenderOffice(pGame);
            break;
        }
        case Jumpscare: {

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
        case Jumpscare: {
            Game_RenderFeddyJumpscare(pGame);
            break;
        }
    }
}


void Game_RenderOffice(Game* pGame) {
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
    


    float relationX = (float)GGame->Width / 1600.0f;
    float relationY = (float)GGame->Height / 900.0f;


    //render middle door
    if(pGame->IsMiddleDoorClosed) {
        if(MiddleDoorSequence > 0.0f) {
            MiddleDoorSequence -= pGame->DeltaTime * 10.0f;
        }
    } else {
        if(MiddleDoorSequence < 1.0f) {
            MiddleDoorSequence += pGame->DeltaTime * 10.0f;
        }
    }
    MiddleDoorSequence = fclamp(MiddleDoorSequence, 0.01f, 1.0f);
    int midDoorFrame = MiddleDoorSequence * 8 - 1;
    int midDoorMaxFrames = 8;

    mat4 trans5;
    glm_mat4_identity(trans5);
    glm_translate(trans5, (vec3){pGame->horizontalScroll, 39.375f * relationY, 0.0f});
    glm_scale(trans5, (vec3){320.0f * relationX, 268.125 * relationY, 1.0f});

    mat4 overall5;
    glm_mat4_mul(proj, trans5, overall5);

    vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[MIDDLE_DOOR][currentFrame], 0, NULL);
    vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->atlasShader.graphicsPipeline);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall5);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4), sizeof(int), &midDoorFrame);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4) + sizeof(int), sizeof(int), &midDoorMaxFrames);
    vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);




    //render office fan
    fanSequence += pGame->DeltaTime * 50.0f;
    int fanFrames = 24;
    int fanIdx = (int)fanSequence % 24;

    mat4 trans2;
    glm_mat4_identity(trans2);
    glm_translate(trans2, (vec3){-340.0f * relationX + pGame->horizontalScroll, 232.0f * relationY, 0.0f});
    glm_scale(trans2, (vec3){132.0f * relationX, 160.0f * relationY, 1.0f});

    mat4 overall2;
    glm_mat4_mul(proj, trans2, overall2);

    vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[OFFICE_FAN][currentFrame], 0, NULL);
    vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->atlasShader.graphicsPipeline);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall2);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4), sizeof(int), &fanIdx);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4) + sizeof(int), sizeof(int), &fanFrames);
    vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);



    //render left door
    if(pGame->IsLeftDoorClosed) {
        if(LeftDoorSequence > 0.0f) {
            LeftDoorSequence -= pGame->DeltaTime * 10.0f;
        }
    } else {
        if(LeftDoorSequence < 1.0f) {
            LeftDoorSequence += pGame->DeltaTime * 10.0f;
        }
    }
    LeftDoorSequence = fclamp(LeftDoorSequence, 0.01f, 1.0f);
    int leftDoorFrame = LeftDoorSequence * 8 - 1;
    int leftDoorMaxFrames = 8;

    mat4 trans3;
    glm_mat4_identity(trans3);
    glm_translate(trans3, (vec3){990.0f * relationX + pGame->horizontalScroll, 86.25f * relationY, 0.0f});
    glm_scale(trans3, (vec3){155.0f * relationX, 367.5f * relationY, 1.0f});

    mat4 overall3;
    glm_mat4_mul(proj, trans3, overall3);

    vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[LEFT_DOOR][currentFrame], 0, NULL);
    vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->atlasShader.graphicsPipeline);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall3);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4), sizeof(int), &leftDoorFrame);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4) + sizeof(int), sizeof(int), &leftDoorMaxFrames);
    vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);



    //render right door
    if(pGame->IsRightDoorClosed) {
        if(RightDoorSequence > 0.0f) {
            RightDoorSequence -= pGame->DeltaTime * 10.0f;
        }
    } else {
        if(RightDoorSequence < 1.0f) {
            RightDoorSequence += pGame->DeltaTime * 10.0f;
        }
    }
    RightDoorSequence = fclamp(RightDoorSequence, 0.01f, 1.0f);
    int rightDoorFrame = RightDoorSequence * 8 - 1;
    int rightDoorMaxFrames = 8;

    mat4 trans4;
    glm_mat4_identity(trans4);
    glm_translate(trans4, (vec3){-990.0f * relationX + pGame->horizontalScroll, 86.25f * relationY, 0.0f});
    glm_scale(trans4, (vec3){155.0f * relationX, 367.5f * relationY, 1.0f});

    mat4 overall4;
    glm_mat4_mul(proj, trans4, overall4);

    vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[RIGHT_DOOR][currentFrame], 0, NULL);
    vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->atlasShader.graphicsPipeline);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall4);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4), sizeof(int), &rightDoorFrame);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4) + sizeof(int), sizeof(int), &rightDoorMaxFrames);
    vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
}
void Game_RenderCam1(Game* pGame) {
    mat4 proj;
    glm_ortho(-pGame->Width / 2.0f, pGame->Width / 2.0f, pGame->Height / 2.0f, -pGame->Height / 2.0f, -1.0, 1.0, proj);
    proj[1][1] *= -1;

    mat4 trans;
    glm_mat4_identity(trans);
    glm_translate(trans, (vec3){0.0f, 0.0f, 0.0f});
    glm_scale(trans, (vec3){pGame->Width, pGame->Height, 1.0f});

    mat4 overall;
    glm_mat4_mul(proj, trans, overall);

    //render cam1
    vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[TCAM1][currentFrame], 0, NULL);
    vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->firstShader.graphicsPipeline);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall);
    vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);

    float relationX = (float)GGame->Width / 1600.0f;
    float relationY = (float)GGame->Height / 900.0f;

    if(pGame->feddyState == FeddyCam1) {
        mat4 trans2;
        glm_mat4_identity(trans2);
        glm_translate(trans2, (vec3){102.5f * relationX, -90.0f * relationY, 0.0f});
        glm_scale(trans2, (vec3){232.5f * relationX,  255.0f * relationY, 1.0f});

        mat4 overall2;
        glm_mat4_mul(proj, trans2, overall2);

        //render cam1
        vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[FEDDY_CAM1][currentFrame], 0, NULL);
        vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->firstShader.graphicsPipeline);
        vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall2);
        vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
    }
}
void Game_RenderCam2(Game* pGame) {
    mat4 proj;
    glm_ortho(-pGame->Width / 2.0f, pGame->Width / 2.0f, pGame->Height / 2.0f, -pGame->Height / 2.0f, -1.0, 1.0, proj);
    proj[1][1] *= -1;

    mat4 trans;
    glm_mat4_identity(trans);
    glm_translate(trans, (vec3){0.0f, 0.0f, 0.0f});
    glm_scale(trans, (vec3){pGame->Width, pGame->Height, 1.0f});

    mat4 overall;
    glm_mat4_mul(proj, trans, overall);

    //render cam1
    vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[TCAM2][currentFrame], 0, NULL);
    vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->firstShader.graphicsPipeline);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall);
    vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);


    float relationX = (float)GGame->Width / 1600.0f;
    float relationY = (float)GGame->Height / 900.0f;

    if(pGame->feddyState == FeddyCam2) {
        mat4 trans2;
        glm_mat4_identity(trans2);
        glm_translate(trans2, (vec3){255.0f * relationX, -86.25f * relationY, 0.0f});
        glm_scale(trans2, (vec3){135.0f * relationX, 240.0f * relationY, 1.0f});

        mat4 overall2;
        glm_mat4_mul(proj, trans2, overall2);

        //render cam1
        vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[FEDDY_CAM2][currentFrame], 0, NULL);
        vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->firstShader.graphicsPipeline);
        vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall2);
        vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
    }
}
void Game_RenderCam3(Game* pGame) {
    mat4 proj;
    glm_ortho(-pGame->Width / 2.0f, pGame->Width / 2.0f, pGame->Height / 2.0f, -pGame->Height / 2.0f, -1.0, 1.0, proj);
    proj[1][1] *= -1;

    mat4 trans;
    glm_mat4_identity(trans);
    glm_translate(trans, (vec3){0.0f, 0.0f, 0.0f});
    glm_scale(trans, (vec3){pGame->Width, pGame->Height, 1.0f});

    mat4 overall;
    glm_mat4_mul(proj, trans, overall);

    //render cam1
    vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[TCAM3][currentFrame], 0, NULL);
    vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->firstShader.graphicsPipeline);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall);
    vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);

    float relationX = (float)GGame->Width / 1600.0f;
    float relationY = (float)GGame->Height / 900.0f;

    if(pGame->feddyState == FeddyCam3) {
        mat4 trans2;
        glm_mat4_identity(trans2);
        glm_translate(trans2, (vec3){-285.0f * relationX, 71.25f * relationY, 0.0f});
        glm_scale(trans2, (vec3){215.0f * relationX, 202.5 * relationY, 1.0f});

        mat4 overall2;
        glm_mat4_mul(proj, trans2, overall2);

        //render cam1
        vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[FEDDY_CAM3][currentFrame], 0, NULL);
        vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->firstShader.graphicsPipeline);
        vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall2);
        vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
    }
}
void Game_RenderCam4(Game* pGame) {
    mat4 proj;
    glm_ortho(-pGame->Width / 2.0f, pGame->Width / 2.0f, pGame->Height / 2.0f, -pGame->Height / 2.0f, -1.0, 1.0, proj);
    proj[1][1] *= -1;

    mat4 trans;
    glm_mat4_identity(trans);
    glm_translate(trans, (vec3){0.0f, 0.0f, 0.0f});
    glm_scale(trans, (vec3){pGame->Width, pGame->Height, 1.0f});

    mat4 overall;
    glm_mat4_mul(proj, trans, overall);

    //render cam1
    vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[TCAM4][currentFrame], 0, NULL);
    vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->firstShader.graphicsPipeline);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall);
    vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);

    float relationX = (float)GGame->Width / 1600.0f;
    float relationY = (float)GGame->Height / 900.0f;

    if(pGame->feddyState == FeddyCam4) {
        mat4 trans2;
        glm_mat4_identity(trans2);
        glm_translate(trans2, (vec3){302.5f * relationX, 461.25f * relationY, 0.0f});
        glm_scale(trans2, (vec3){642.5f * relationX, 438.75f * relationY, 1.0f});

        mat4 overall2;
        glm_mat4_mul(proj, trans2, overall2);

        //render cam1
        vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[FEDDY_CAM4][currentFrame], 0, NULL);
        vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->firstShader.graphicsPipeline);
        vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame],pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall2);
        vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
    }
}
void Game_RenderMonitorFlip(Game* pGame) {
    int flippingFrame = monitorFlipSequence * 10.0f * 24.0f;
    int flipFrames = 24;

    mat4 pos;
    glm_mat4_identity(pos);
    glm_scale(pos, (vec3){pGame->Width / 2.0f, pGame->Height / 2.0f, 0.0f});

    mat4 overall;
    glm_mat4_identity(overall);
    glm_mat4_mul((vec4*)Center, pos, overall);

    vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[MONITOR_FLIP][currentFrame], 0, NULL);
    vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->UIAtlasShader.graphicsPipeline);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame], pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame], pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4), sizeof(float), &flippingFrame);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame], pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4) + sizeof(float), sizeof(float), &flipFrames);
    vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
}
void Game_RenderFeddyJumpscare(Game* pGame) {
    mat4 proj;
    glm_ortho(-pGame->Width / 2.0f, pGame->Width / 2.0f, pGame->Height / 2.0f, -pGame->Height / 2.0f, -1.0, 1.0, proj);
    proj[1][1] *= -1;

    mat4 trans;
    glm_mat4_identity(trans);
    glm_translate(trans, (vec3){0.0f, 0.0f, 0.0f});
    glm_scale(trans, (vec3){pGame->Width / 2.0f, pGame->Height / 2.0f, 1.0f});

    mat4 overall;
    glm_mat4_mul(proj, trans, overall);


    int frame = fclamp(JumpscareSequence, 0.0f, 1.9f) * 24.0f;
    int maxFrames = 24;
    if(JumpscareSequence < 0.9f)
    JumpscareSequence += pGame->DeltaTime;

    //render office
    vkCmdBindDescriptorSets(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->m_Renderer.pipelineLayout, 0, 1, &pGame->m_Renderer.textureSets[FEDDY_JUMPSCARE][currentFrame], 0, NULL);
    vkCmdBindPipeline(pGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pGame->UIAtlasShader.graphicsPipeline);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame], pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), overall);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame], pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4), sizeof(float), &frame);
    vkCmdPushConstants(pGame->m_Renderer.commandBuffers[currentFrame], pGame->m_Renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4) + sizeof(float), sizeof(float), &maxFrames);
    vkCmdDraw(pGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
}




