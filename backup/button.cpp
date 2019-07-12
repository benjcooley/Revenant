// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     button.cpp - TButton object                       *
// *************************************************************************

#include <ctype.h>

#include "revenant.h"
#include "display.h"
#include "button.h"
#include "font.h"

// ***********
// * TButton *
// ***********

void DrawFrame(PTSurface surface, int x, int y, int w, int h, BOOL down)
{
    int topcolor = 216, bottomcolor = 40, leftcolor = 196, rightcolor = 70;
    int centercolor = 128;
    SColor color;

    if (down)
    {
        int tmp;
        tmp = topcolor; topcolor = bottomcolor; bottomcolor = tmp;
        tmp = leftcolor; leftcolor = rightcolor; rightcolor = tmp;
    }

    // center
    color.red = color.green = color.blue = centercolor;
    surface->Box(x + 1, y + 1, w - 2, h - 2, TranslateColor(color), 0, 0, DM_BACKGROUND);
    // top
    color.red = color.green = color.blue = topcolor;
    surface->Box(x, y, w, 1, TranslateColor(color), 0, 0, DM_BACKGROUND);
    // left
    color.red = color.green = color.blue = leftcolor;
    surface->Box(x, y + 1, 1, h - 1, TranslateColor(color), 0, 0, DM_BACKGROUND);
    // right
    color.red = color.green = color.blue = rightcolor;
    surface->Box(x + w - 1, y + 1, 1, h - 2, TranslateColor(color), 0, 0, DM_BACKGROUND);
    // bottom
    color.red = color.green = color.blue = bottomcolor;
    surface->Box(x + 1, y + h - 1, w - 1, 1, TranslateColor(color), 0, 0, DM_BACKGROUND);
}

TButton::TButton(char *bname, int bx, int by, int bw, int bh, WORD keypr,
                 void (*bfunc)(), PTBitmap dbm, PTBitmap ubm, BOOL rad,
                 BOOL tog, BOOL notsquare, int group, int repeat)
{
    strcpy(name, bname);
    x = bx;
    y = by;
    w = bw;
    h = bh;
    key = keypr;
    radial = rad;
    toggle = tog;
    buttonfunc = bfunc;
    down = FALSE;
    dirty = TRUE;
    hidden = FALSE;
    pixelcheck = notsquare && (ubm != NULL);
    level = 0;
    radiogroup = group;
    repeatrate = repeat;
    if (radiogroup >= 0)
        toggle = TRUE;

    downbitmap = dbm;
    if (downbitmap && rad)
    {
        downbitmap->flags |= BM_REGPOINT;
        downbitmap->regx = w + 1;
        downbitmap->regy = w;
    }

    upbitmap = ubm;
    if (upbitmap && rad)
    {
        downbitmap->flags |= BM_REGPOINT;
        upbitmap->regx = w + 1;
        upbitmap->regy = w;
    }
}

void TButton::Draw()
{
    if (hidden)
        return;

    if (downbitmap)
    {
        if (pixelcheck)
            //Display->PutSV(x, y, down ? downbitmap : upbitmap, DM_USEREG | DM_BACKGROUND | DM_TRANSPARENT, level * 2, level);
            Display->Put(x, y, down ? downbitmap : upbitmap, DM_USEREG | DM_BACKGROUND | DM_TRANSPARENT);
        else
            Display->Put(x, y, down ? downbitmap : upbitmap, DM_USEREG | DM_BACKGROUND);
    }
    else
    {
        // generate a generic button
        int add = down ? 1 : 0;
        int nx = x + (w / 2) - ((strlen(name) * SystemFont->GetChar(SystemFont->FirstChar())->width) / 2);
        int ny = y + (h / 2) - (SystemFont->height / 2) - 3;
        DrawFrame(Display, x, y, w, h, down);
        SColor color;
        color.red = color.blue = color.green = 40;
        Display->WriteText(name, nx + add, ny + add, 1, SystemFont, &color, DM_TRANSPARENT | DM_ALIAS | DM_BACKGROUND);
        Display->AddUpdateRect(x, y, w, h, UPDATE_RESTORE);
    }

    dirty = FALSE;
}

#define REPEATWAIT      7

void TButton::Animate(BOOL draw)
{
    if (hidden)
        return;

    if (repeatrate > 0 && down)
        if (counter++ >= REPEATWAIT && (counter % (FRAMERATE / repeatrate)) == 0)
            ButtonFunc();
}

BOOL TButton::OnButton(int bx, int by)
{
    if (hidden || (radiogroup >= 0 && down))
        return FALSE;

    if (radial)
        return ((sqr(absval(x - bx)) + sqr(absval(y - by))) <= sqr(w));
    else
    {
        BOOL insquare = (bx >= x && by >= y && bx <= (x + w) && by <= (y + h));
        if (!pixelcheck || !insquare)
            return insquare;

        return upbitmap->OnPixel(bx - x, by - y);
    }
}

inline BOOL TButton::IsKey(int keypr, BOOL keydown)
{
    if (hidden || (radiogroup >= 0 && down))
        return FALSE;

    if (Editor)
        return (keypr == key && (key < 'A' || key > 'Z' || CtrlDown || !keydown));
    else
        return (keypr == key && !CtrlDown);
}

// ***************
// * TButtonPane *
// ***************

BOOL TButtonPane::Initialize()
{
    TPane::Initialize();

    Buttons.Clear();
    SetDirty(TRUE);
    clicked = -1;
    return TRUE;
}

void TButtonPane::Close()
{
    TPointerIterator<TButton> i(&Buttons);
    
    while (i)
    {
        if (i.Item() == NULL)
            i++;
        else
        {
            delete i.Item();
            Buttons.Remove(i);
        }
    }
}

void TButtonPane::DrawBackground()
{
    for (TPointerIterator<TButton> i(&Buttons); i; i++)
        if (i.Item() && i.Item()->IsDirty())
            i.Item()->Draw();
}

void TButtonPane::Animate(BOOL draw)
{
    for (TPointerIterator<TButton> i(&Buttons); i; i++)
        if (i.Item())
            i.Item()->Animate(draw);
}

void TButtonPane::RedrawButtons()
{
    for (TPointerIterator<TButton> i(&Buttons); i; i++)
        if (i.Item())
            i.Item()->SetDirty();
}

void TButtonPane::KeyPress(int key, BOOL down)
{
    if (down)
    {
        for (TPointerIterator<TButton> i(&Buttons); i; i++)
            if (i.Item() && i.Item()->IsKey(key, down) &&
                (i.Item()->GetState() == FALSE || i.ItemNum() != clicked) &&
                (i.Item()->RadioGroup() < 0 || i.Item()->GetState() == FALSE))
            {
                if (i.Item()->IsToggle())
                {
                    ClearGroup(i.Item()->RadioGroup());
                    i.Item()->SetState(!(i.Item()->GetState()));
                    i.Item()->ButtonFunc();
                }
                else
                    i.Item()->SetState(TRUE);

                if (i.Item()->Repeats())
                    i.Item()->ButtonFunc();
                break;
            }
    }
    else
    {
        for (TPointerIterator<TButton> i(&Buttons); i; i++)
            if (i.Item() && i.Item()->IsKey(key, down) && i.Item()->GetState() == TRUE &&
                i.ItemNum() != clicked)
            {
                if (!i.Item()->IsToggle())
                    i.Item()->SetState(FALSE);

                if (!i.Item()->Repeats())
                    i.Item()->ButtonFunc();
            }
    }
}

void TButtonPane::MouseClick(int button, int x, int y)
{
    if (button == MB_LEFTDOWN)
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
                    i.Item()->ButtonFunc();
                break;
            }
    }
    else if (button == MB_LEFTUP)
    {
        if (clicked >= 0 && Buttons[clicked]->OnButton(x, y) &&
            !Buttons[clicked]->IsToggle())
        {
            if (!Buttons[clicked]->IsToggle())
                Buttons[clicked]->SetState(FALSE);
            if (!Buttons[clicked]->Repeats())
                Buttons[clicked]->ButtonFunc();
        }
        clicked = -1;
    }
}

void TButtonPane::MouseMove(int button, int x, int y)
{
    if (button == MB_LEFTDOWN && clicked >= 0)
    {
        if (Buttons[clicked]->OnButton(x, y))
        {
            if (Buttons[clicked]->GetState() == FALSE &&
                !Buttons[clicked]->IsToggle())
                Buttons[clicked]->SetState(TRUE);
        }
        else
        {
            if (Buttons[clicked]->GetState() == TRUE ||
                !Buttons[clicked]->IsToggle())
                Buttons[clicked]->SetState(FALSE);
        }
    }
}

BOOL TButtonPane::NewButton(PTButton b)
{
    if (b)
        return (Buttons.Add(b) >= 0);

    CheckGroup(b->RadioGroup());

    return FALSE;
}

void TButtonPane::ClearGroup(int group)
{
    if (group < 0)
        return;

    for (TPointerIterator<TButton> i(&Buttons); i; i++)
        if (i.Item()->RadioGroup() == group)
            i.Item()->SetState(FALSE);
}

void TButtonPane::CheckGroup(int group)
{
    if (group < 0)
        return;

    for (TPointerIterator<TButton> i(&Buttons); i; i++)
        if (i.Item()->RadioGroup() == group)
        {
            i.Item()->SetState(TRUE);
            break;
        }
}

