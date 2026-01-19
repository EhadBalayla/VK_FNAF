#ifndef OFFICE_HUD_SCREEN
#define OFFICE_HUD_SCREEN

#include "../Widgets/TextDisplayer.h"
#include "../Widgets/UIHoverable.h"

typedef struct {
    UIHoverable monitorHover; //will later be redone into a hoverable
    TextDisplayer clockText;
} OfficeHUDScreen;

void OfficeHUDScreen_Initialize(OfficeHUDScreen* pScreen);
void OfficeHUDScreen_Update(OfficeHUDScreen* pScreen);
void OfficeHUDScreen_Render(OfficeHUDScreen* pScreen);

#endif