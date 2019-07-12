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
#include "spellpane.h"
#include "textbar.h"

extern TObjectClass TalismanClass;

char *Old[] =
{ "Sun", "Life", "Ocean", "Law", "Soul", "Stars", "Death", "Chaos", "Sky", "Earth", "Ward", "Moon", NULL };

// *******************
// * TTalismanButton *
// *******************

// do this!
void TTalismanButton::Draw()
{
    TButton::Draw();

    if (!Player)
        return;

    char spell[SPELLSIZE + 1];
    strncpyz(spell, Player->GetQuickSpell(quickspellid), MAXTALISMANLEN);
    int len = strlen(spell);

    if (hidden)
        return;

    int start = x + xoffset + ((SPELLSIZE - len) * 11) - 4;

    for (int i = 0; i < len; i++)
    {
        for(int t = 0; t < TalismanClass.NumTypes(); t++)
        {
            char code = TalismanClass.GetStat(t, "Code");
            if (toupper(spell[i]) == toupper(code))
                Display->Put(start + (i*23), y + 3 + (down ? 1 : 0),
                GameData->Bitmap(*(Old + t)), DM_BACKGROUND | DM_TRANSPARENT); // t used to be x - Pepper
        }
    }
}

void TTalismanButton::AddTalisman(char t)
{
    if (!Player)
        return;

    char spell[SPELLSIZE + 1];
    strncpyz(spell, Player->GetQuickSpell(quickspellid), MAXTALISMANLEN);
    int len = strlen(spell);

    if (len < SPELLSIZE)
    {
        // add it
        spell[len++] = t;
        spell[len] = '\0';
                
        // now check for if you got that many
        if (!HasTalismans())
        {
            len--;
            spell[len] = '\0';
            TextBar.Print("No more talismans of that type.");
        }

        SetDirty();
    }
    
    Player->SetQuickSpell(quickspellid, spell);
}

void TTalismanButton::Backspace()
{
    if (!Player)
        return;

    char spell[SPELLSIZE + 1];
    strncpyz(spell, Player->GetQuickSpell(quickspellid), MAXTALISMANLEN);
    int len = strlen(spell);

    if (len > 0)
    {
        len--;
        spell[len] = '\0';
        SetDirty();
    }

    Player->SetQuickSpell(quickspellid, spell);
}

void TTalismanButton::Clear()
{
    if (!Player)
        return;

    Player->SetQuickSpell(quickspellid, "");
}

char *TTalismanButton::GetSpell()
{
    if (!Player)
        return "";

    return Player->GetQuickSpell(quickspellid);
}

void TTalismanButton::SetSpell(char *talismans)
{
    if (!Player)
        return;

    Player->SetQuickSpell(quickspellid, talismans);

    SetDirty();
}

// Checks to see if player has talismans for this spell
BOOL TTalismanButton::HasTalismans()
{
    if (!Player)
        return FALSE;

    return Player->HasTalismans(Player->GetQuickSpell(quickspellid));
}

void TTalismanButton::Invoke()
{
    if (Player)
        Player->InvokeQuickSpell(quickspellid);
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

    NewButton(new TTalismanButton("spell",  24,  95, 120, 34, 0, BtnSpellPane, GameData->Bitmap("spellinvokedown"), GameData->Bitmap("spellinvokeup"), FALSE, FALSE, TRUE, QSPELL_CONSTRUCT));
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
            PTObjectInstance pouch = Player->FindObjInventory("spell pouch");
            if (!pouch)
                PTObjectInstance pouch = Player->FindObjInventory("spellpouch");
            if (pouch)
            {
                int x = TAL_STARTX, y = TAL_STARTY;

                SColor color = { 0, 0, 0 };

                int line = 0;
                for (int i = 0; i<TalismanClass.NumTypes(); i++)
                {
                    char *name = TalismanClass.GetObjType(i)->name;

                    if (pouch->FindObjInventory(name))
                    {
                        if ((line++ >= startline || !ShowTalismanNames()) && y < TAL_WRAPHEIGHT)
                        {
                            int add = 0;
                            if (onclickedtal && i == clickedtal)
                                add = 1;

                            char buf[80];
                            sprintf(buf, "%scandy", Old[i]);
                            Display->Put(x+2, y-2+2, GameData->Bitmap(buf), DM_BACKGROUND | DM_TRANSPARENT, &color);
                            Display->Put(x+add, y-2+add, GameData->Bitmap(buf), DM_BACKGROUND | DM_TRANSPARENT);
                            Display->Put(x+2+1, y+1+1, GameData->Bitmap(Old[i]), DM_BACKGROUND | DM_TRANSPARENT, &color);
                            Display->Put(x+2+add, y+1+add, GameData->Bitmap(Old[i]), DM_BACKGROUND | DM_TRANSPARENT);

                            if (ShowTalismanNames())
                                Display->WriteTextShadow(name, x + TAL_WIDTH - 1, y, 1, GameData->Font("goldfont"));

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
        PTObjectInstance pouch = Player->FindObjInventory("spell pouch");
        if (!pouch)
            PTObjectInstance pouch = Player->FindObjInventory("spellpouch");
        if (pouch)
        {
            int x0 = TAL_STARTX, y0 = TAL_STARTY;

            int line = 0;
            for (int i = 0; Old[i]; i++)
                if (pouch->FindObjInventory(Old[i]))
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
    char code = TalismanClass.GetStat(tal, "Code");
    ((PTTalismanButton)Button(0))->AddTalisman(code);
    SetDirty(TRUE);
    return FALSE;
}

BOOL TSpellPane::RemoveTal(int numtals)
{
    for(int i = 0; i < numtals; ++i)
        ((PTTalismanButton)Button(0))->Backspace();

    SetDirty(TRUE);

    return TRUE;
}

void TSpellPane::Invoke()
{
    ((PTTalismanButton)Button(0))->Invoke();
}

char *TSpellPane::GetSpell()
{
    return ((PTTalismanButton)Button(0))->GetSpell();
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

    NewButton(new TTalismanButton("1", 0, 0, 120, 28, 0, BtnSpellOne, GameData->Bitmap("spell1down"), GameData->Bitmap("spell1up"), FALSE, FALSE, TRUE, QSPELL_1, -4));
    NewButton(new TTalismanButton("2", 99, 7, 120, 28, 0, BtnSpellTwo, GameData->Bitmap("spell2down"), GameData->Bitmap("spell2up"), FALSE, FALSE, TRUE, QSPELL_2, 4));
    NewButton(new TTalismanButton("3", 218, 7, 120, 28, 0, BtnSpellThree, GameData->Bitmap("spell3down"), GameData->Bitmap("spell3up"), FALSE, FALSE, TRUE, QSPELL_3, -4));
    NewButton(new TTalismanButton("4", 317, 0, 120, 28, 0, BtnSpellFour, GameData->Bitmap("spell4down"), GameData->Bitmap("spell4up"), FALSE, FALSE, TRUE, QSPELL_4, 4));

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

void TQuickSpellPane::AddTalisman(int button, char tal)
{
    if(button > 0 && button <= NUMBUTTONS)
        ((PTTalismanButton)Button(button - 1))->AddTalisman(tal);
}

void TQuickSpellPane::Backspace(int button)
{
    if(button > 0 && button <= NUMBUTTONS)
        ((PTTalismanButton)Button(button - 1))->Backspace();
}

void TQuickSpellPane::Clear(int button)
{
    if(button > 0 && button <= NUMBUTTONS)
        ((PTTalismanButton)Button(button - 1))->Clear();
}

void TQuickSpellPane::Set(int button)
{
    if(button > 0 && button <= NUMBUTTONS)
    {
        ((PTTalismanButton)Button(button - 1))->Clear();

        char buf[MAXTALISMANLEN];

        strncpyz(buf, SpellPane.GetSpell(), MAXTALISMANLEN);

        for(int i = 0; buf[i]; ++i)
            ((PTTalismanButton)Button(button - 1))->AddTalisman(buf[i]);

        SetDirty(TRUE);
    }
}

void TQuickSpellPane::MouseClick(int button, int x, int y)
{
    TButtonPane::MouseClick(button, x, y);

    if (button == MB_RIGHTDOWN)
    {
        for (TPointerIterator<TButton> i(&Buttons); i; i++)
            if (i.Item() && i.Item()->OnButton(x, y) &&
                (i.Item()->GetState() == FALSE || i.Item()->IsToggle()) &&
                (i.Item()->RadioGroup() < 0 || i.Item()->GetState() == FALSE))
            {
                if (i.Item()->IsToggle())
                {
                    ClearGroup(i.Item()->RadioGroup());
                    i.Item()->Invert();
                    i.Item()->ButtonFunc();
                }
                else
                {
                    i.Item()->SetState(TRUE);
                    clicked = i.ItemNum();
                }

                if (i.Item()->Repeats())
                    Set(clicked + 1);
                break;
            }
    }
    else if (button == MB_RIGHTUP)
    {
        if (clicked >= 0 && Buttons[clicked]->OnButton(x, y) &&
            !Buttons[clicked]->IsToggle())
        {
            if (!Buttons[clicked]->IsToggle())
                Buttons[clicked]->SetState(FALSE);
            if (!Buttons[clicked]->Repeats())
                Set(clicked + 1);
        }
        clicked = -1;
    }
}
