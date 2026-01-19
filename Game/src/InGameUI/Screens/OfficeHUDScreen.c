#include <cglm/cglm.h>
#include "../../core/Game.h"

#include <stdio.h>

char* GetGameTime(int time) { //remember to free since it mallocs
    char* buffer = malloc(sizeof(char) * 5); //for each digit in the clock and the :
    if (time / 60 == 0) {
        buffer[0] = '1';
        buffer[1] = '2';
    } else {
        buffer[0] = '0';
        buffer[1] = '0' + time / 60;
    }
    buffer[2] = ':';

    int seconds = time % 60;
    int tens = seconds / 10;
    int singles = seconds % 10;

    buffer[3] = '0' + tens;
    buffer[4] = '0' + singles;

    return buffer;
}

int lastTime = 0;


void FlipMonitorUp(void* pHoverable) {
    printf("hovered over the monitor hover\n");
}


void OfficeHUDScreen_Initialize(OfficeHUDScreen* pScreen) {
    UIHoverable* monitorHover = &pScreen->monitorHover;
    TextDisplayer* clockText = &pScreen->clockText;

    //setting the monitor hover
    monitorHover->position[0] = 0.0f; monitorHover->position[1] = 50.0f;
    monitorHover->scale[0] = 300.0f; monitorHover->scale[1] = 30.0f;
    monitorHover->texID = MONITOR_HOVER;
    monitorHover->IsHovered = 0;
    monitorHover->OnHovered = FlipMonitorUp;
    monitorHover->projMat = Center_Bottom;

    //setting the text
    SetText(clockText, "12:00", 5);
    clockText->position[0] = 100.0f; clockText->position[1] = 100.0f;
    clockText->textScale = 1.0f;
    clockText->projMat = Left_Top;
}
void OfficeHUDScreen_Update(OfficeHUDScreen* pScreen) {
    UpdateHoverable(&pScreen->monitorHover);
}
void OfficeHUDScreen_Render(OfficeHUDScreen* pScreen) {
    UIHoverable* monitorHover = &pScreen->monitorHover;
    TextDisplayer* clockText = &pScreen->clockText;

    if((int)GGame->GameTime != lastTime) {
        lastTime = (int)GGame->GameTime;
        char* text = GetGameTime(lastTime);
        SetText(clockText, text, 5);
        free(text);
    }

    RenderHoverable(monitorHover);
    RenderText(clockText);
}