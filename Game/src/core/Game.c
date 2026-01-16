#include "Game.h"

void Game_Init(Game* pGame) {
    Window_InitGLFW();
    Window_CreateWindow(&pGame->m_Window, 1280, 720, "VK_FNAF");
    Window_CreateContext(&pGame->m_Window);
}
void Game_Loop(Game* pGame) {
    while(!Window_ShouldClose(&pGame->m_Window)) {
        Window_PollEvents();
        Window_StartFrame(&pGame->m_Window);

        Window_EndFrame(&pGame->m_Window);
    }
}
void Game_Terminate(Game* pGame) {
    Window_DestroyContext(&pGame->m_Window);
    Window_DestroyWindow(&pGame->m_Window);
    Window_TerminateGLFW();
}