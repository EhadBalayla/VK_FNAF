#ifndef OFFICE_HUD_SCREEN
#define OFFICE_HUD_SCREEN

#include "../Widgets/ImageDisplayer.h"
#include "../Widgets/TextDisplayer.h"

typedef struct {
    ImageDisplayer monitorHover; //will later be redone into a hoverable
    TextDisplayer clockText;
} OfficeHUDScreen;

void OfficeHUDScreen_Initialize(OfficeHUDScreen* pScreen);
void OfficeHUDScreen_Render(OfficeHUDScreen* pScreen);

#endif