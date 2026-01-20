#ifndef MONITOR_HUD_SCREEN
#define MONITOR_HUD_SCREEN

#include "../Widgets/UIHoverable.h"
#include "../Widgets/UIAtlas.h"
#include "../Widgets/UIButton.h"

typedef struct {
    UIHoverable monitorHover; //this is specifically for taking down the monitor
    UIAtlas cameraStatic;
    UIButton cams[4]; //since for the start we will have 4 cameras
} MonitorHUDScreen;

void MonitorHUDScreen_Initialize(MonitorHUDScreen* pScreen);
void MonitorHUDScreen_Update(MonitorHUDScreen* pScreen);
void MonitorHUDScreen_Render(MonitorHUDScreen* pScreen);

#endif