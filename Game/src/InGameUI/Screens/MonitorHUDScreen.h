#ifndef MONITOR_HUD_SCREEN
#define MONITOR_HUD_SCREEN

#include "../Widgets/UIHoverable.h"
#include "../Widgets/ImageDisplayer.h"

typedef struct {
    UIHoverable monitorHover; //this is specifically for taking down the monitor
} MonitorHUDScreen;

void MonitorHUDScreen_Initialize(MonitorHUDScreen* pScreen);
void MonitorHUDScreen_Update(MonitorHUDScreen* pScreen);
void MonitorHUDScreen_Render(MonitorHUDScreen* pScreen);

#endif