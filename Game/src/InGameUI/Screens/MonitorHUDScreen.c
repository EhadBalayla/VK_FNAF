#include "MonitorHUDScreen.h"

#include "../../core/Game.h"

void FlipMonitorDown(void* pHoverable) {
    if(GGame->states == Monitor) {
        GGame->states = FlippingDown;
    }
}

void MonitorHUDScreen_Initialize(MonitorHUDScreen* pScreen) {
    UIHoverable* monitorHover = &pScreen->monitorHover;

    //setting the monitor hover
    monitorHover->position[0] = 0.0f; monitorHover->position[1] = 50.0f;
    monitorHover->scale[0] = 300.0f; monitorHover->scale[1] = 30.0f;
    monitorHover->texID = MONITOR_HOVER;
    monitorHover->IsHovered = 0;
    monitorHover->OnHovered = FlipMonitorDown;
    monitorHover->projMat = Center_Bottom;
}
void MonitorHUDScreen_Update(MonitorHUDScreen* pScreen) {
    UpdateHoverable(&pScreen->monitorHover);
}
void MonitorHUDScreen_Render(MonitorHUDScreen* pScreen) {
    UIHoverable* monitorHover = &pScreen->monitorHover;

    RenderHoverable(monitorHover);
}