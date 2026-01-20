#include "MonitorHUDScreen.h"

#include "../../core/Game.h"

void FlipMonitorDown(void* pHoverable) {
    if(GGame->states == Monitor) {
        GGame->states = FlippingDown;
    }
}

UIButton* selectedCamButton;
void RedrawButtons(UIButton* pButton) {
    if(pButton != selectedCamButton) {
        selectedCamButton->texID = CAMERA_UNSELECTED;
        selectedCamButton = pButton;
        pButton->texID = CAMERA_SELECTED;
    }
}

void Cam1Click(void *pButton) {
    GGame->selectedCam = CAM1;
    RedrawButtons(pButton);
}
void Cam2Click(void *pButton) {
    GGame->selectedCam = CAM2;
    RedrawButtons(pButton);
}
void Cam3Click(void *pButton) {
    GGame->selectedCam = CAM3;
    RedrawButtons(pButton);
}
void Cam4Click(void *pButton) {
    GGame->selectedCam = CAM4;
    RedrawButtons(pButton);
}
void (*camClicks[])(void*) = {
    Cam1Click,
    Cam2Click,
    Cam3Click,
    Cam4Click
};

void MonitorHUDScreen_Initialize(MonitorHUDScreen* pScreen) {
    UIHoverable* monitorHover = &pScreen->monitorHover;
    UIAtlas* cameraStatic = &pScreen->cameraStatic;

    //setting the monitor hover
    monitorHover->position[0] = 0.0f; monitorHover->position[1] = 30.0f;
    monitorHover->scale[0] = 300.0f; monitorHover->scale[1] = 30.0f;
    monitorHover->texID = MONITOR_HOVER;
    monitorHover->IsHovered = 0;
    monitorHover->OnHovered = FlipMonitorDown;
    monitorHover->projMat = Center_Bottom;

    //setting the camera static
    cameraStatic->position[0] = 0.0f; cameraStatic->position[1] = 0.0f;
    cameraStatic->scale[0] = GGame->Width / 2.0f; cameraStatic->scale[1] = GGame->Height / 2.0f;
    cameraStatic->texID = CAMERA_STATIC_ALPHA;
    cameraStatic->sequence = 0.0f;
    cameraStatic->speedScale = 5.0f;
    cameraStatic->maxFrames = 4;
    cameraStatic->projMat = Center;

    //setting up the camera buttons
    for(int i = 0; i < 4; i++) {
        UIButton* camButton = &pScreen->cams[i];
        camButton->position[0] = 450.0f; camButton->position[1] = 0.0f + i * 50.0f;
        camButton->scale[0] = 80.0f; camButton->scale[1] = 20.0f;
        camButton->texID = i == 0 ? CAMERA_SELECTED : CAMERA_UNSELECTED;
        camButton->projMat = Center;
        camButton->OnClick = camClicks[i];
    }
    selectedCamButton = &pScreen->cams[0];
}
void MonitorHUDScreen_Update(MonitorHUDScreen* pScreen) {
    UpdateHoverable(&pScreen->monitorHover);
    for(int i = 0; i < 4; i++) {
        UpdateButton(&pScreen->cams[i]);
    }

    //resizing if the window size changes
    if(GGame->Width / 2.0f != pScreen->cameraStatic.scale[0]) pScreen->cameraStatic.scale[0] = GGame->Width / 2.0f;
    if(GGame->Height / 2.0f != pScreen->cameraStatic.scale[1]) pScreen->cameraStatic.scale[1] = GGame->Height / 2.0f;

    pScreen->cameraStatic.sequence += GGame->DeltaTime * pScreen->cameraStatic.speedScale; //advance forward the sequence of the
}
void MonitorHUDScreen_Render(MonitorHUDScreen* pScreen) {
    UIHoverable* monitorHover = &pScreen->monitorHover;
    UIAtlas* cameraStatic = &pScreen->cameraStatic;

    RenderUIAtlas(cameraStatic);
    RenderHoverable(monitorHover);
    for(int i = 0; i < 4; i++) {
        RenderButton(&pScreen->cams[i]);
    }
}