// ************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   statpane.cpp - FORSAKEN Stat Pane                   *
// *************************************************************************

#include "revenant.h"
#include "statpane.h"
#include "display.h"
#include "playscreen.h"
#include "player.h"
#include "multi.h"
#include "font.h"

void BtnStatStats()
{
    StatPane.Update();
}

void BtnStatSkills()
{
    StatPane.Update();
}

void BtnStatMin()
{
    StatPane.ContractAll();
    StatPane.Scroll(-10000);
}

void BtnStatMax()
{
    StatPane.ExpandAll();
}

void BtnStatUp()
{
    StatPane.Scroll(-1);
}

void BtnStatDown()
{
    StatPane.Scroll(1);
}

#define NUMLINES    6
#define SCROLLRATE  8
#define TEXT_POS_Y  23

BOOL TStatPane::Initialize()
{
    TButtonPane::Initialize();

    NewButton("stats", 19, 5, 64, 19, VK_RETURN, BtnStatStats, GameData->Bitmap("statsdown"), GameData->Bitmap("statsup"), FALSE, TRUE, FALSE, 1);
    NewButton("skills", 84, 5, 64, 19, VK_RETURN, BtnStatSkills, GameData->Bitmap("skillsdown"), GameData->Bitmap("skillsup"), FALSE, TRUE, FALSE, 1);

    NewButton("up", 149, 66, 16, 14, 0, BtnStatUp, GameData->Bitmap("spellupdown"), GameData->Bitmap("spellupup"), FALSE, FALSE, FALSE, -1, SCROLLRATE);
    NewButton("down", 149, 83, 16, 14, 0, BtnStatDown, GameData->Bitmap("spelldowndown"), GameData->Bitmap("spelldownup"), FALSE, FALSE, FALSE, -1, SCROLLRATE);

    NewButton("min", 149, 49, 16, 14, 0, BtnStatMin, GameData->Bitmap("statmindown"), GameData->Bitmap("statminup"));
    NewButton("max", 149, 34, 16, 14, 0, BtnStatMax, GameData->Bitmap("statmaxdown"), GameData->Bitmap("statmaxup"));

    startline = 0;
    memset(&expanded, 0, sizeof(BOOL) * NUM_SKILLS);

    Button(0)->SetState(TRUE);

    return TRUE;
}

void TStatPane::Close()
{
    TButtonPane::Close();
}

#define GEM         (char)('~' + 13)
#define GEM_LINE    (char)(GEM + 1)
#define LINE        (char)(GEM_LINE + 1)
#define LINE_BRANCH (char)(LINE + 1)
#define LINE_END    (char)(LINE_BRANCH + 1)

void TStatPane::DrawBackground()
{
    if (IsDirty())
    {
        Display->Put(0, 0, GameData->Bitmap("statpane"), DM_BACKGROUND);

        RedrawButtons();

        char buf1[512] = "";
        char buf2[512] = "";

        BOOL showdownbutton = TRUE;

        if (Button(0)->GetState() == TRUE)
        {
            for (int i = 0; i < NUM_PLRSTATS; i++)
                sprintf(buf1, "%s   %s\n", buf1, Player->ObjStatName(PLRSTAT_FIRST + i));

            if (Player)
                for (int i = 0; i < NUM_PLRSTATS; i++)
                    sprintf(buf2, "%s%d\n", buf2, Player->GetObjStat(PLRSTAT_FIRST + i));

            showdownbutton = FALSE;
        }
        else
        {
            int line = 0;
            for (int i = 0; i < NUM_SKILLS; i++, line++)
            {
                BOOL isline = TRUE;

                if (SkillTree[i].ancestor < 0)
                {
                    if (line >= startline)
                    {
                        if (!SkillTree[i].children)
                            sprintf(buf1, "%s   %s\n", buf1, SkillTree[i].name);
                        else
                        {
                            if (expanded[i])
                                sprintf(buf1, "%s%c %s\n", buf1, GEM_LINE, SkillTree[i].name);
                            else
                                sprintf(buf1, "%s%c %s\n", buf1, GEM, SkillTree[i].name);
                        }
                    }
                }
                else
                {
                    int lastlen = strlen(buf1);

                    // check for ancestor of the ancestor...
                    int grandparent = SkillTree[i].ancestor;
                    while (grandparent >= 0)
                    {
                        if (!expanded[grandparent])
                        {
                            isline = FALSE;
                            break;
                        }

                        char fillchar;

                        if (SkillTree[grandparent].lastchild)
                            fillchar = ' ';
                        else
                            fillchar = LINE;

                        grandparent = SkillTree[grandparent].ancestor;
                        if (grandparent < 0)
                            break;

                        if (fillchar == ' ')
                            sprintf(buf1, "%s  ", buf1);
                        else
                            sprintf(buf1, "%s%c", buf1, fillchar);
                    }

                    if (!isline || line < startline)
                        buf1[lastlen] = 0;
                    else
                    {
                        if (!SkillTree[i].children)
                        {
                            if (SkillTree[i].lastchild)
                                sprintf(buf1, "%s%c %s\n", buf1, LINE_END, SkillTree[i].name);
                            else
                                sprintf(buf1, "%s%c %s\n", buf1, LINE_BRANCH, SkillTree[i].name);
                        }
                        else
                        {
                            int gemchar = expanded[i] ? GEM_LINE : GEM;

                            if (SkillTree[i].lastchild)
                                sprintf(buf1, "%s%c%c %s\n", buf1, LINE_END, gemchar, SkillTree[i].name);
                            else
                                sprintf(buf1, "%s%c%c %s\n", buf1, LINE_BRANCH, gemchar, SkillTree[i].name);
                        }
                    }
                }

                if (!isline)
                    line--;     // the line is hidden, don't count it
                else if (line >= startline && Player)
                    sprintf(buf2, "%s%d\n", buf2, Player->Skill(i));
            }

            if ((line - startline) <= NUMLINES)
                showdownbutton = FALSE;
        }

        Display->WriteTextShadow(buf1, 12, TEXT_POS_Y, NUMLINES, GameData->Font("goldfont"));

        if (Player)
            Display->WriteTextShadow(buf2, 138, TEXT_POS_Y, NUMLINES, GameData->Font("goldfont"), NULL, FONT_DRAWMODE, -1, 0, JUSTIFY_RIGHT);

        // determine which scroll buttons are showing
        if (startline < 1 || Button(0)->GetState())
        {
            ScrollUpButton()->SetState(FALSE);
            ScrollUpButton()->Hide();
        }
        else
            ScrollUpButton()->Show();

        if (showdownbutton)
            ScrollDownButton()->Show();
        else
        {
            ScrollDownButton()->SetState(FALSE);
            ScrollDownButton()->Hide();
        }
        
        SetDirty(FALSE);
        PlayScreen.MultiUpdate();
    }

    TButtonPane::DrawBackground();
}

void TStatPane::MouseClick(int button, int x, int y)
{
    TButtonPane::MouseClick(button, x, y);

    if (button == MB_LEFTDOWN)
    {
        int slot = OnSlot(x, y);
        if (slot >= 0 && SkillTree[slot].children)
        {
            expanded[slot] = !expanded[slot];
            SetDirty(TRUE);
        }
    }
}

void TStatPane::MouseMove(int button, int x, int y)
{
    TButtonPane::MouseMove(button, x, y);
}

void TStatPane::Scroll(int numlines)
{
    int oldstartline = startline;

    startline += numlines;

    if (startline < 0)
        startline = 0;
    if ((startline + NUMLINES) > NUM_SKILLS)
        startline = NUM_SKILLS - NUMLINES;

    if (startline != oldstartline)
        SetDirty(TRUE);
}

void TStatPane::SetAllExpanded(BOOL value)
{
    BOOL changed = FALSE;

    for (int i = 0; i < NUM_SKILLS; i++)
        if (SkillTree[i].children)
            if (expanded[i] != value)
            {
                expanded[i] = value;
                changed = TRUE;
            }

    if (changed)
        SetDirty(TRUE);
}

int TStatPane::OnSlot(int x, int y)
{
    if (x > 113)            // buttons are to the right of this value
        return -1;

    y -= TEXT_POS_Y + 4;
    if (y < 0)
        return -1;

    int slot = y / GameData->Font("goldfont")->height;
    if (slot < 0 || slot >= NUMLINES)
        return -1;

    slot += startline;

    int line = 0;
    for (int i = 0; i < NUM_SKILLS; i++)
    {
        BOOL isline = TRUE;
        int grandparent = SkillTree[i].ancestor;
        while (grandparent >= 0)
        {
            if (!expanded[grandparent])
            {
                isline = FALSE;
                break;
            }
            grandparent = SkillTree[grandparent].ancestor;
        }

        if (isline && line++ == slot)
            return i;
    }

    return -1;
}
