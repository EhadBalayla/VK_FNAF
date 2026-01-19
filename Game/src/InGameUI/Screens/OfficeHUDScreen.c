#include <cglm/cglm.h>
#include "../../core/Game.h"

void OfficeHUDScreen_Initialize(OfficeHUDScreen* pScreen) {
    ImageDisplayer* monitorHover = &pScreen->monitorHover;
    //setting a projection matrix that 0,0 is the center on the X but bottom on the Y
    monitorHover->position[0] = 0.0f; monitorHover->position[1] = 50.0f;
    monitorHover->scale[0] = 300.0f; monitorHover->scale[1] = 30.0f;
    monitorHover->texID = MONITOR_HOVER;
}
void OfficeHUDScreen_Render(OfficeHUDScreen* pScreen) {
    ImageDisplayer* monitorHover = &pScreen->monitorHover;
    monitorHover->Left = -GGame->Width / 2.0f; monitorHover->Right = GGame->Width / 2.0f; monitorHover->Bottom = -GGame->Height; monitorHover->Top = 0.0f;

    Render_Image(&pScreen->monitorHover);
}