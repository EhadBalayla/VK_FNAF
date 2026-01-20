#ifndef GAME
#define GAME

#include "Window.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "FontLoader.h"

#include "../InGameUI/Screens/OfficeHUDScreen.h"
#include "../InGameUI/Screens/MonitorHUDScreen.h"

//the projection matrices
extern float Center[16];
extern float Center_Bottom[16];
extern float Left_Top[16];

typedef enum {
    Office, //for when the monitor is down
    FlippingUp, //for when the monitor is in the process of being flipped up
    Monitor, //for when the player is in the monitor
    FlippingDown //for when the monitor is in the process of being flipped down
} InGameState;

typedef struct {
    //the entire systems
    Window m_Window;
    Renderer m_Renderer;
    FontLoader m_Font;

    //some configurational parameters
    int Width;
    int Height;

    //texture resources
    Texture fontTexture;
    Texture allTextures[MAX_RENDERS];

    //shader resources
    Shader FullscreenShader;
    Shader firstShader;
    Shader atlasShader;
    Shader UIShader; //same as the firstShader, but for rendering to the swapchain, for in game UI
    Shader TextShader; //a unique shader for text, but rendering entirely to the swapchain
    Shader UIAtlasShader; //same as AtlasShader EXCEPT will be used for the swapchain AND with alpha blending on

    //in game UI screens
    OfficeHUDScreen officeHUD;
    MonitorHUDScreen monitorHUD;


    //real time parameters to be used
    double DeltaTime;

    float horizontalScroll;

    double MouseX;
    double MouseY;

    float GameTime;

    void* SelectedButton;

    InGameState states;
} Game;

extern Game* GGame; //a global pointer for the game instance

void Game_Init(Game* pGame);
void Game_Loop(Game* pGame);
void Game_Terminate(Game* pGame);

#endif