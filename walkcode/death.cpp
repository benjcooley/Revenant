// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    death.cpp - TDeathPane module                      *
// *************************************************************************

#include "revenant.h"
#include "death.h"
#include "display.h"
#include "multi.h"
#include "playscreen.h"
#include "savegame.h"

static BOOL saveisfullscreen;

TDeathPane DeathPane;

void DeathBtnRestart()
{
    SaveGame.ReadGame();
    DeathPane.Close();
}

void DeathBtnLoad()
{
    SaveGame.ReadGame();
    DeathPane.Close();
}

void DeathBtnExit()
{
    SaveGame.ReadGame();
    DeathPane.Close();
}

BOOL TDeathPane::Initialize()
{
    TButtonPane::Initialize();

    deathdata = TMulti::LoadMulti("death.dat");

    if (deathdata)
    {
        saveisfullscreen = PlayScreen.IsFullScreen();
        PlayScreen.SetFullScreen(FALSE);

        NewButton("restart", 76, 144, 148, 80, VK_TAB, DeathBtnRestart, deathdata->Bitmap("restartdown"), deathdata->Bitmap("restartup"));
        NewButton("load", 240, 146, 112, 72, VK_RETURN, DeathBtnLoad, deathdata->Bitmap("loaddown"), deathdata->Bitmap("loadup"));
        NewButton("exit", 384, 152, 96, 64, VK_ESCAPE, DeathBtnExit, deathdata->Bitmap("exitdown"), deathdata->Bitmap("exitup"));

        PlayScreen.AddPane(this);
        PlayScreen.SetExclusivePane(this, TRUE);
        return TRUE;
    }

    return FALSE;
}

void TDeathPane::Close()
{
    TButtonPane::Close();

    if (deathdata)
        delete deathdata;

    PlayScreen.ReleaseExclusivePane(this);
    PlayScreen.RemovePane(this);
    PlayScreen.Redraw();

    PlayScreen.SetFullScreen(saveisfullscreen);
}

void TDeathPane::DrawBackground()
{
    if (IsDirty())
    {
        Display->Put(0, 0, deathdata->Bitmap("background"), DM_BACKGROUND);
        PlayScreen.DrawOverhangs();
        SetClipRect();      // drawing overhangs screws them up
        SetDirty(FALSE);
    }

    TButtonPane::DrawBackground();
}

