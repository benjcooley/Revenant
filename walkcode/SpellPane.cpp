// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     SpellPane.cpp - Spell Pane                        *
// *************************************************************************

#include "revenant.h"
#include "bitmap.h"
#include "character.h"
#include "display.h"
#include "mappane.h"
#include "multi.h"
#include "player.h"
#include "playscreen.h"
#include "spell.h"
#include "font.h"

char *Talismans[] =
{ "Sun", "Life", "Ocean", "Law", "Soul", "Stars", "Death", "Chaos", "Sky", "Earth", "Ward", "Moon", NULL };

// *******************
// * TTalismanButton *
// *******************

void TTalismanButton::Draw()
{
    TButton::Draw();

    if (hidden)
        return;

    int start = x + xoffset + ((SPELLSIZE - len) * 11) + 14;

    for (int i = 0; i < len; i++)
        Display->Put(start + (i*23), y + 3 + (down ? 1 : 0), GameData->Bitmap(Talismans[spell[i]]), DM_BACKGROUND | DM_TRANSPARENT);
}

void TTalismanButton::AddTalisman(int t)
{
    if (len < SPELLSIZE)
    {
        spell[len++] = t;
        SetDirty();
    }
}

void TTalismanButton::Backspace()
{
    if (len > 0)
    {
        len--;
        SetDirty();
    }
}

void TTalismanButton::Clear()
{
    if (len > 0)
    {
        len = 0;
        SetDirty();
    }
}

void TTalismanButton::Invoke()
{
    if (Player && len > 0)
    {
        for(int i = 0; i < len; ++i)
            list[i] = spell[i] + 'a';
        list[i] = '\0';

        Player->CastByTalismans(list);
    }
}

// **************
// * TSpellPane *
// **************

void BtnSpellPane()
{
    SpellPane.Invoke();
}

void BtnSpellBook()
{
}

void BtnSpellAdd()
{
}

void BtnSpellBack()
{
    SpellPane.RemoveTal();
}

void BtnSpellDown()
{
    SpellPane.Scroll(1);
}

void BtnSpellUp()
{
    SpellPane.Scroll(-1);
}

void BtnSpellMin()
{
    SpellPane.ToggleTalismanNames();
}

#define REPEATRATE      8

BOOL TSpellPane::Initialize()
{
    TButtonPane::Initialize();

    NewButton(new TTalismanButton("spell",  24,  95, 120, 34, VK_RETURN, BtnSpellPane, GameData->Bitmap("spellinvokedown"), GameData->Bitmap("spellinvokeup"), FALSE, FALSE, TRUE));
    NewButton("book",   3,   62, 16, 14, 0, BtnSpellBook, GameData->Bitmap("spellbookdown"), GameData->Bitmap("spellbookup"));
    NewButton("add",    3,   84, 16, 14, 0, BtnSpellAdd, GameData->Bitmap("spelladddown"), GameData->Bitmap("spelladdup"));
    NewButton("back",   142, 94, 12, 11, VK_BACK, BtnSpellBack, GameData->Bitmap("spellbackdown"), GameData->Bitmap("spellbackup"));
    NewButton("down",   149, 83, 16, 14, 0, BtnSpellDown, GameData->Bitmap("spelldowndown"), GameData->Bitmap("spelldownup"), FALSE, FALSE, FALSE, -1, REPEATRATE);
    NewButton("up",     149, 66, 16, 14, 0, BtnSpellUp, GameData->Bitmap("spellupdown"), GameData->Bitmap("spellupup"), FALSE, FALSE, FALSE, -1, REPEATRATE);
    NewButton("min",    149, 45, 16, 14, 0, BtnSpellMin, GameData->Bitmap("spellmindown"), GameData->Bitmap("spellminup"));

    shownames = TRUE;
    startline = 0;
    clickedtal = -1;

    Update();

    return TRUE;
}

#define TAL_STARTX      25
#define TAL_STARTY      9
#define TAL_WIDTH       30
#define TAL_HEIGHT      30
#define TAL_WRAPWIDTH   130
#define TAL_WRAPHEIGHT  76
#define TAL_NONAMEGAP   30

void TSpellPane::DrawBackground()
{
    BOOL wasdirty = IsDirty();

    if (wasdirty)
    {
        Display->Put(0, 0, GameData->Bitmap("spell"), DM_BACKGROUND);

        RedrawButtons();

        if (Player)
        {
            // find their spell pouch
            PTObjectInstance pouch = Player->FindObjInventory("Spell Pouch");
            if (pouch)
            {
                int x = TAL_STARTX, y = TAL_STARTY;

                SColor color = { 0, 0, 0 };

                int line = 0;
                for (int i = 0; Talismans[i]; i++)
                    if (pouch->FindObjInventory(Talismans[i]))
                    {
                        if ((line++ >= startline || !ShowTalismanNames()) && y < TAL_WRAPHEIGHT)
                        {
                            int add = 0;
                            if (onclickedtal && i == clickedtal)
                                add = 1;

                            char buf[80];
                            sprintf(buf, "%scandy", Talismans[i]);
                            Display->Put(x+2, y-2+2, GameData->Bitmap(buf), DM_BACKGROUND | DM_TRANSPARENT, &color);
                            Display->Put(x+add, y-2+add, GameData->Bitmap(buf), DM_BACKGROUND | DM_TRANSPARENT);
                            Display->Put(x+2+1, y+1+1, GameData->Bitmap(Talismans[i]), DM_BACKGROUND | DM_TRANSPARENT, &color);
                            Display->Put(x+2+add, y+1+add, GameData->Bitmap(Talismans[i]), DM_BACKGROUND | DM_TRANSPARENT);

                            if (ShowTalismanNames())
                                Display->WriteTextShadow(Talismans[i], x + TAL_WIDTH - 1, y, 1, GameData->Font("goldfont"));

                            x += TAL_WIDTH;
                            if (ShowTalismanNames())
                                x += TAL_NONAMEGAP;

                            if (x > TAL_WRAPWIDTH)
                            {
                                x = TAL_STARTX;
                                y += TAL_HEIGHT;
                            }
                        }
                    }

                //color.red = 255;
                //Display->WriteTextShadow("Greater Slurpee", 32, 79, 1, GameData->Font("tinyfont"), &color);

                // set up button visibility
                if (!ShowTalismanNames() || startline < 1)
                {
                    Button(5)->SetState(FALSE);
                    Button(5)->Hide();
                }
                else
                    Button(5)->Show();

                if (!ShowTalismanNames() || (line - startline) <= 6)
                {
                    Button(4)->SetState(FALSE);
                    Button(4)->Hide();
                }
                else
                    Button(4)->Show();
            }
        }

        if (shownames)
        {
            Button(6)->SetUpBitmap(GameData->Bitmap("spellminup"));
            Button(6)->SetDownBitmap(GameData->Bitmap("spellmindown"));
        }
        else
        {
            Button(6)->SetUpBitmap(GameData->Bitmap("spellmaxup"));
            Button(6)->SetDownBitmap(GameData->Bitmap("spellmaxdown"));
        }

        PlayScreen.MultiUpdate();
        SetDirty(FALSE);
    }


    if (wasdirty)
        TButtonPane::DrawBackground();
}

void TSpellPane::MouseClick(int button, int x, int y)
{
    TButtonPane::MouseClick(button, x, y);

    if (button == MB_LEFTDOWN)
    {
        clickedtal = OnTal(x, y);
        if (clickedtal >= 0)
        {
            onclickedtal = TRUE;
            Update();
        }
    }
    else if (button == MB_LEFTUP && clickedtal >= 0)
    {
        if (onclickedtal)
            AddTal(clickedtal);

        clickedtal = -1;
        Update();
    }
}

void TSpellPane::MouseMove(int button, int x, int y)
{
    TButtonPane::MouseMove(button, x, y);

    if (button == MB_LEFTDOWN && clickedtal >= 0)
    {
        BOOL oldonclickedtal = onclickedtal;

        if (OnTal(x, y) == clickedtal)
            onclickedtal = TRUE;
        else
            onclickedtal = FALSE;

        if (oldonclickedtal != onclickedtal)
            Update();
    }
}

int TSpellPane::OnTal(int x, int y)
{
    if (x < TAL_STARTX || x >= (TAL_STARTX + TAL_WRAPWIDTH) ||
        y < TAL_STARTY || y >= (TAL_STARTY + TAL_WRAPHEIGHT))
        return -1;

    if (Player)
    {
        PTObjectInstance pouch = Player->FindObjInventory("Spell Pouch");
        if (pouch)
        {
            int x0 = TAL_STARTX, y0 = TAL_STARTY;

            int line = 0;
            for (int i = 0; Talismans[i]; i++)
                if (pouch->FindObjInventory(Talismans[i]))
                    if ((line++ >= startline || !ShowTalismanNames()) && y0 < TAL_WRAPHEIGHT)
                    {
                        if (x >= x0 && x < (x0 + TAL_WIDTH) && y >= y0 && y < (y0 + TAL_HEIGHT))
                            return i;

                        x0 += TAL_WIDTH;
                        if (ShowTalismanNames())
                            x0 += TAL_NONAMEGAP;

                        if (x0 > TAL_WRAPWIDTH)
                        {
                            x0 = TAL_STARTX;
                            y0 += TAL_HEIGHT;
                        }
                    }
        }
    }

    return -1;
}

void TSpellPane::Scroll(int numlines)
{
    int oldstartline = startline;

    startline += numlines * 2;      // two talismans on a line

    if (startline < 0)
        startline = 0;

    if (startline != oldstartline)
        SetDirty(TRUE);
}

void TSpellPane::ToggleTalismanNames()
{
    shownames = !shownames;
    SetDirty(TRUE);
}

BOOL TSpellPane::AddTal(int tal)
{
    ((PTTalismanButton)Button(0))->AddTalisman(tal);
    return FALSE;
}

BOOL TSpellPane::RemoveTal(int numtals)
{
    for (int i = 0; i < numtals; i++)
        ((PTTalismanButton)Button(0))->Backspace();

    return TRUE;
}

void TSpellPane::Invoke()
{
    ((PTTalismanButton)Button(0))->Invoke();
}

// ******************
// * QuickSpellPane *
// ******************

void BtnSpellOne()
{
    QuickSpells.Invoke(1);
}

void BtnSpellTwo()
{
    QuickSpells.Invoke(2);
}

void BtnSpellThree()
{
    QuickSpells.Invoke(3);
}

void BtnSpellFour()
{
    QuickSpells.Invoke(4);
}

BOOL TQuickSpellPane::Initialize()
{
    TButtonPane::Initialize();

    NewButton(new TTalismanButton("1", 0, 0, 120, 28, '1', BtnSpellOne, GameData->Bitmap("spell1down"), GameData->Bitmap("spell1up"), FALSE, FALSE, TRUE, -4));
    NewButton(new TTalismanButton("2", 99, 7, 120, 28, '2', BtnSpellTwo, GameData->Bitmap("spell2down"), GameData->Bitmap("spell2up"), FALSE, FALSE, TRUE, 4));
    NewButton(new TTalismanButton("3", 218, 7, 120, 28, '3', BtnSpellThree, GameData->Bitmap("spell3down"), GameData->Bitmap("spell3up"), FALSE, FALSE, TRUE, -4));
    NewButton(new TTalismanButton("4", 317, 0, 120, 28, '4', BtnSpellFour, GameData->Bitmap("spell4down"), GameData->Bitmap("spell4up"), FALSE, FALSE, TRUE, 4));

    return TRUE;
}

void TQuickSpellPane::DrawBackground()
{
    TButtonPane::DrawBackground();
}

void TQuickSpellPane::Invoke(int button)
{
    // James - Was <, but button is one based and NUMBUTTONS is 4
    if (button > 0 && button <= NUMBUTTONS)
        ((PTTalismanButton)Button(button - 1))->Invoke();
}

