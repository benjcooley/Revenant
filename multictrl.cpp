// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                multictrl.cpp - Multipane Control Panel                *
// *************************************************************************

#include "revenant.h"
#include "multictrl.h"
#include "button.h"

// Pane list
PTPane MultiPanes[NUMMCBUTTONS] =
    { (PTPane)&EquipPane, (PTPane)&AutoMap, (PTPane)&SpellPane, (PTPane)&StatPane };

// Button functions

void BtnEquip()
{
    MultiCtrl.ActivatePane(0);
}

void BtnMap()
{
    MultiCtrl.ActivatePane(1);
}

void BtnSpell()
{
    MultiCtrl.ActivatePane(2);
}

void BtnStats()
{
    MultiCtrl.ActivatePane(3);
}

// ******************
// * TMultiCtrlPane *
// ******************

BOOL TMultiCtrlPane::Initialize()
{
    TButtonPane::Initialize();

    buttondata = TMulti::LoadMulti("intrface.dat");

    NewButton("Equip",  30+BUTTONRADIUS,  2+BUTTONRADIUS, BUTTONRADIUS, BUTTONRADIUS, '1', BtnEquip, buttondata->Bitmap("equipdown"), buttondata->Bitmap("equipup"), TRUE, FALSE, TRUE, 1);
    NewButton("Map",    2+BUTTONRADIUS,  35+BUTTONRADIUS, BUTTONRADIUS, BUTTONRADIUS, '2', BtnMap, buttondata->Bitmap("mapdown"), buttondata->Bitmap("mapup"), TRUE, FALSE, FALSE, 1);
    NewButton("Spell",  2+BUTTONRADIUS,  77+BUTTONRADIUS, BUTTONRADIUS, BUTTONRADIUS, '3', BtnSpell, buttondata->Bitmap("spelldown"), buttondata->Bitmap("spellup"), TRUE, FALSE, FALSE, 1);
    NewButton("Stat",   30+BUTTONRADIUS,109+BUTTONRADIUS, BUTTONRADIUS, BUTTONRADIUS, '4', BtnStats, buttondata->Bitmap("skilldown"), buttondata->Bitmap("skillup"), TRUE, FALSE, TRUE, 1);

    curpane = -1;
    ActivatePane(0);
    Button(0)->SetState(TRUE);
    return TRUE;
}

void TMultiCtrlPane::Close()
{
    delete buttondata;
}

void TMultiCtrlPane::ActivatePane(int pane)
{
    if (pane == curpane)
        return;

    for (int p = 0; p < NUMMCBUTTONS; p++)
        if (p != pane)
            MultiPanes[p]->Hide();

    MultiPanes[pane]->Show();

    curpane = pane;
}

void TMultiCtrlPane::Hide()
{
    TButtonPane::Hide();

    for (int p = 0; p < NUMMCBUTTONS; p++)
        MultiPanes[p]->Hide();
}

void TMultiCtrlPane::Show()
{
    TButtonPane::Show();

    MultiPanes[curpane]->Show();
}

void TMultiCtrlPane::RedrawCurPane()
{
    MultiPanes[curpane]->SetDirty(TRUE);
}

