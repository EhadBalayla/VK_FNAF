#include "Game.h"

Game* GGame = NULL;

//function declarations
void Window_Resize(void* GLFWwindow, int width, int height);



void Game_Init(Game* pGame) {
    //create the Window and initialize context and swapchain
    Window_InitGLFW();
    Window_CreateWindow(&pGame->m_Window, 1280, 720, "VK_FNAF");
    Window_CreateContext(&pGame->m_Window);

    //set the window callbacks
    Window_SetResizeCallback(&pGame->m_Window, Window_Resize);

    //give the renderer the necessary handles and initialize the renderer
    pGame->m_Renderer.device = pGame->m_Window.m_Context.device;
    pGame->m_Renderer.graphicsFamily = pGame->m_Window.m_Context.graphicsFamily;
    pGame->m_Renderer.presentFamily = pGame->m_Window.m_Context.presentFamily;
    pGame->m_Renderer.graphicsQueue = pGame->m_Window.m_Context.graphicsQueue;
    pGame->m_Renderer.PresentQueue = pGame->m_Window.m_Context.PresentQueue;
    pGame->m_Renderer.MaxFramesInFlight = MAX_FRAMES_IN_FLIGHT;
    Renderer_Init(&pGame->m_Renderer);
}
void Game_Loop(Game* pGame) {
    while(!Window_ShouldClose(&pGame->m_Window)) {
        Window_PollEvents();
        Window_StartFrame(&pGame->m_Window);

        Window_EndFrame(&pGame->m_Window);
    }
}
void Game_Terminate(Game* pGame) {
    Renderer_wait(&pGame->m_Renderer);

    Renderer_Terminate(&pGame->m_Renderer);

    Window_DestroyContext(&pGame->m_Window);
    Window_DestroyWindow(&pGame->m_Window);
    Window_TerminateGLFW();
}



void Window_Resize(void* GLFWwindow, int width, int height) {
    VKSwapchain_Recreate(&GGame->m_Window.m_Swapchain);
}