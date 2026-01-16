#include "core/Game.h"

int main() {
    Game game;
    Game_Init(&game);
    Game_Loop(&game);
    Game_Terminate(&game);
    return 0;
}