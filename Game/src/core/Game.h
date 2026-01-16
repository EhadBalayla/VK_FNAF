#ifndef GAME
#define GAME

#include "Window.h"

typedef struct {
    Window m_Window;
} Game;

void Game_Init(Game* pGame);
void Game_Loop(Game* pGame);
void Game_Terminate(Game* pGame);

#endif