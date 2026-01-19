#include <cglm/cglm.h>
#include "../../core/Game.h"

void OfficeHUDScreen_Initialize(OfficeHUDScreen* pScreen) {
    ImageDisplayer* monitorHover = &pScreen->monitorHover;
    TextDisplayer* clockText = &pScreen->clockText;

    //setting the monitor hover
    monitorHover->position[0] = 0.0f; monitorHover->position[1] = 50.0f;
    monitorHover->scale[0] = 300.0f; monitorHover->scale[1] = 30.0f;
    monitorHover->texID = MONITOR_HOVER;

    //setting the text
    SetText(clockText, "12:00", 5);
    clockText->position[0] = 100.0f; clockText->position[1] = 100.0f;
    clockText->textScale = 1.0f;
}
void OfficeHUDScreen_Render(OfficeHUDScreen* pScreen) {
    ImageDisplayer* monitorHover = &pScreen->monitorHover;
    TextDisplayer* clockText = &pScreen->clockText;
    
    //setting the projection matrices
    monitorHover->Left = -GGame->Width / 2.0f; monitorHover->Right = GGame->Width / 2.0f; monitorHover->Bottom = -GGame->Height; monitorHover->Top = 0.0f;
    clockText->Left = 0.0f; clockText->Right = GGame->Width; clockText->Bottom = 0.0f; clockText->Top = GGame->Height;

    Render_Image(monitorHover);
    RenderText(clockText);
}