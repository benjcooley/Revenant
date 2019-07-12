// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           statusbar.cpp - Status bars (health, stamina)               *
// *************************************************************************

#include "revenant.h"
#include "statusbar.h"
#include "display.h"

// **************
// * TStatusBar *
// **************

BOOL TStatusBar::Initialize()
{
    tubedata = NULL;
    level = targetlevel = 1000;
    animating = FALSE;
    SetDirty(TRUE);
    return TRUE;
}

void TStatusBar::Close()
{
    if (tubedata)
        delete tubedata;
}

#define HEALTH_INCREMENT    10

void TStatusBar::DrawBackground()
{
    if (animating || level != targetlevel || IsDirty())
    {
        if (absval(level - targetlevel) <= HEALTH_INCREMENT)
            level = targetlevel;
        else if (level < targetlevel)
            level += HEALTH_INCREMENT;
        else if (level > targetlevel)
            level -= HEALTH_INCREMENT;

        BOOL drawmode = 0;

        if (level != targetlevel)
        {
            animating = TRUE;
            drawmode |= DM_NORESTORE;
        }
        else
        {
            animating = FALSE;
            drawmode |= DM_BACKGROUND;
        }

        Display->PutHue(0, -(176 * level / 1000), tubedata->Bitmap("tube"), drawmode, GetHue());

        drawmode |= DM_TRANSPARENT;

        Display->Put(0, 0, tubedata->Bitmap("topoverlay"), drawmode);
        Display->Put(0, 59, tubedata->Bitmap("middleoverlay"), drawmode);
        Display->Put(0, 161, tubedata->Bitmap("bottomoverlay"), drawmode);

        SetDirty(FALSE);
    }
}

// **************
// * THealthBar *
// **************

BOOL THealthBar::Initialize()
{
    TStatusBar::Initialize();

    tubedata = TMulti::LoadMulti("health.dat");

    return tubedata != NULL;
}

int THealthBar::GetHue()
{
    int hue = level * 155 / 1000;
    if (hue > 16)
        hue -= 16;
    else
        hue = 0;

    return hue;
}

// ***************
// * TStaminaBar *
// ***************

BOOL TStaminaBar::Initialize()
{
    TStatusBar::Initialize();

    tubedata = TMulti::LoadMulti("stamina.dat");

    return tubedata != NULL;
}

int TStaminaBar::GetHue()
{
    return ((1000 - level) * 65 / 1000) + 240;
}

