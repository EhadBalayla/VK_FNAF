#ifndef GAME
#define GAME

#include "Window.h"
#include "Renderer.h"

typedef struct {
    Window m_Window;
    Renderer m_Renderer;
} Game;

extern Game* GGame; //a global pointer for the game instance

void Game_Init(Game* pGame);
void Game_Loop(Game* pGame);
void Game_Terminate(Game* pGame);

#endif