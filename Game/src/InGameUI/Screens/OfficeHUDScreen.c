#include "OfficeHUDScreen.h"

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
    if(GGame->states == Office) {
        GGame->states = FlippingUp;
        AudioPlayer_Play2DSound(&GGame->m_Audio, "Sounds/Debug_MonitorFlip.mp3");
    }
}

void PressLeftButton(void* pButton) {
    if(!GGame->IsLeftDoorClosed) GGame->IsLeftDoorClosed = 1;
    else GGame->IsLeftDoorClosed = 0;

    AudioPlayer_Play2DSound(&GGame->m_Audio, "Sounds/Debug_Door.mp3");
}
void PressRightButton(void* pButton) {
    if(!GGame->IsRightDoorClosed) GGame->IsRightDoorClosed = 1;
    else GGame->IsRightDoorClosed = 0;

    AudioPlayer_Play2DSound(&GGame->m_Audio, "Sounds/Debug_Door.mp3");
}
void PressMidButton(void* pButton) {
    if(!GGame->IsMiddleDoorClosed) GGame->IsMiddleDoorClosed = 1;
    else GGame->IsMiddleDoorClosed = 0;

    AudioPlayer_Play2DSound(&GGame->m_Audio, "Sounds/Debug_Door.mp3");
}


void OfficeHUDScreen_Initialize(OfficeHUDScreen* pScreen) {
    UIHoverable* monitorHover = &pScreen->monitorHover;
    TextDisplayer* clockText = &pScreen->clockText;
    TextDisplayer* sundayText = &pScreen->sundayText;
    UIButton* testButton = &pScreen->testButton;
    UIButton* testButton2 = &pScreen->testButton2;
    UIButton* testButton3 = &pScreen->testButton3;

    //setting the monitor hover
    monitorHover->position[0] = 0.0f; monitorHover->position[1] = 30.0f;
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

    //setting the sunday text
    SetText(sundayText, "Night 1 - Sunday", 16);
    sundayText->position[0] = 100.0f; sundayText->position[1] = 150.0f;
    sundayText->textScale = 0.75f;
    sundayText->projMat = Left_Top;


    //door buttons
    testButton->position[0] = 400.0f; testButton->position[1] = 250.0f;
    testButton->scale[0] = 50.0f; testButton->scale[1] = 50.0f;
    testButton->texID = MONITOR_HOVER;
    testButton->OnClick = PressLeftButton;
    testButton->projMat = Center;

    testButton2->position[0] = -400.0f; testButton2->position[1] = 250.0f;
    testButton2->scale[0] = 50.0f; testButton2->scale[1] = 50.0f;
    testButton2->texID = MONITOR_HOVER;
    testButton2->OnClick = PressRightButton;
    testButton2->projMat = Center;

    testButton3->position[0] = 0.0f; testButton3->position[1] = -300.0f;
    testButton3->scale[0] = 50.0f; testButton3->scale[1] = 50.0f;
    testButton3->texID = MONITOR_HOVER;
    testButton3->OnClick = PressMidButton;
    testButton3->projMat = Center;
}
void OfficeHUDScreen_Update(OfficeHUDScreen* pScreen) {
    UpdateHoverable(&pScreen->monitorHover);

    float relationX = (float)GGame->Width / 1600.0f;
    float relationY = (float)GGame->Height / 900.0f;

    pScreen->testButton.position[0] = GGame->horizontalScroll + 1250.0f * relationX;
    pScreen->testButton2.position[0] = GGame->horizontalScroll - 1250.0f * relationX;
    pScreen->testButton3.position[0] = GGame->horizontalScroll;

    UpdateButton(&pScreen->testButton);
    UpdateButton(&pScreen->testButton2);
    UpdateButton(&pScreen->testButton3);
}
void OfficeHUDScreen_Render(OfficeHUDScreen* pScreen) {
    UIHoverable* monitorHover = &pScreen->monitorHover;
    TextDisplayer* clockText = &pScreen->clockText;
    TextDisplayer* sundayText = &pScreen->sundayText;
    UIButton* testButton = &pScreen->testButton;
    UIButton* testButton2 = &pScreen->testButton2;
    UIButton* testButton3 = &pScreen->testButton3;

    if((int)GGame->GameTime != lastTime) {
        lastTime = (int)GGame->GameTime;
        char* text = GetGameTime(lastTime);
        SetText(clockText, text, 5);
        free(text);
    }

    RenderHoverable(monitorHover);
    RenderText(clockText);
    RenderText(sundayText);
    RenderButton(testButton);
    RenderButton(testButton2);
    RenderButton(testButton3);
}