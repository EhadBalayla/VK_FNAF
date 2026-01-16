#ifndef GAME
#define GAME

#include "Window.h"
#include "Renderer.h"
#include "Shader.h"

typedef struct {
    //the entire systems
    Window m_Window;
    Renderer m_Renderer;

    //some configurational parameters
    int Width;
    int Height;

    //shader resources
    Shader firstShader;
} Game;

extern Game* GGame; //a global pointer for the game instance

void Game_Init(Game* pGame);
void Game_Loop(Game* pGame);
void Game_Terminate(Game* pGame);

#endif