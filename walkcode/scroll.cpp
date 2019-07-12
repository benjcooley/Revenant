// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     scroll.cpp - TScroll module                       *
// *************************************************************************

#include "revenant.h"
#include "scroll.h"
#include "display.h"
#include "bitmap.h"
#include "font.h"
#include "playscreen.h"

REGISTER_BUILDER(TScroll)
TObjectClass ScrollClass("SCROLL", OBJCLASS_SCROLL, 0);

// Hard coded class stats
DEFSTAT(Scroll, Value, VAL, 0, 0, 0, 1000000)

TScrollPane ScrollPane;
TBookPane BookPane;

void TScroll::SetText(char *newtext)
{
    if (text)
        delete text;

    text = new char[strlen(newtext)+1];
    if (text)
        strcpy(text, newtext);
}

void TScroll::Load(RTInputStream is, int version, int objversion)
{
    TObjectInstance::Load(is, version, objversion);

    short len;
    is >> len;

    if (len < 1)
        text = NULL;
    else
    {
        text = new char[len+1];

        if (text)
        {
            for (int i = 0; i < len; i++)
                is >> text[i];

            text[i] = 0;
        }
    }
}

void TScroll::Save(RTOutputStream os)
{
    TObjectInstance::Save(os);

    short len = text ? strlen(text) : 0;
    os << len;

    if (len)
    {
        for (int i = 0; i < len; i++)
            os << text[i];
    }
}

BOOL TScroll::Use(PTObjectInstance user, int with)
{
    if (with < 0)
    {
        PTScrollPane pane = &ScrollPane;

        if (!strcmpi(GetTypeName(), "Book"))
            pane = (PTScrollPane)&BookPane;

        PlayScreen.SetNextPane(pane);
        pane->SetScroll(this);
    }

    return FALSE;
}

// ***************
// * TScrollPane *
// ***************

#define NUMLINES        11      // depends on font and scroll height
#define SCROLLWIDTH     430     // depends on font and scroll width

void ScrollBtnScrollUp()
{
    ScrollPane.Scroll(-NUMLINES);
}

void ScrollBtnScrollDown()
{
    ScrollPane.Scroll(NUMLINES);
}

void ScrollBtnExit()
{
    ScrollPane.Close();
}

BOOL TScrollPane::Initialize()
{
    TButtonPane::Initialize();

    line = 0;
    numlines = 0;

    scrolldata = TMulti::LoadMulti("scroll.dat");
    scrollfont = TFont::LoadFont(103);

    SetScroll(scroll);      // recompute number of lines now that font is loaded up

    if (scrolldata)
    {
        PTBitmap bitmap = scrolldata->Bitmap("scroll");
        int x = (WIDTH - bitmap->width) / 2;
        int y = (HEIGHT - bitmap->height) / 4;

        NewButton("down", x+452, y+330, 28, 31, VK_NEXT, ScrollBtnScrollDown, scrolldata->Bitmap("downdown"), scrolldata->Bitmap("downup"));
        NewButton("up", x+484, y+323, 28, 31, VK_PRIOR, ScrollBtnScrollUp, scrolldata->Bitmap("updown"), scrolldata->Bitmap("upup"));
        NewButton("exit", x+518, y+321, 28, 31, VK_ESCAPE, ScrollBtnExit, scrolldata->Bitmap("exitdown"), scrolldata->Bitmap("exitup"));

        PlayScreen.AddPane(this);
        PlayScreen.SetExclusivePane(this, TRUE);
        return TRUE;
    }

    return FALSE;
}

void TScrollPane::Close()
{
    TButtonPane::Close();

    if (scrolldata)
        delete scrolldata;

    PlayScreen.ReleaseExclusivePane(this);
    PlayScreen.RemovePane(this);
    PlayScreen.Redraw();

    scroll = NULL;
}

void TScrollPane::DrawBackground()
{
    if (IsDirty())
    {
        PTBitmap bitmap = scrolldata->Bitmap("scroll");

        int x = (WIDTH - bitmap->width) / 2;
        int y = (HEIGHT - bitmap->height) / 4;
        Display->Put(x, y, bitmap, DM_TRANSPARENT | DM_BACKGROUND);

        if (scroll && scroll->GetText())
            Display->WriteText(scroll->GetText(), x+65, y+68, NUMLINES, scrollfont, NULL, DM_USEDEFAULT, SCROLLWIDTH, line);

        // decide which buttons should be visisble
        if (line < 1)
            Button(1)->Hide();
        else
            Button(1)->Show();

        if ((line + NUMLINES - 1) >= numlines)
            Button(0)->Hide();
        else
            Button(0)->Show();

        Button(2)->Show();

        RedrawButtons();
        SetDirty(FALSE);
    }

    // buttons shouldn't draw until after the rest is placed down
    TButtonPane::DrawBackground();
}

void TScrollPane::KeyPress(int key, BOOL down)
{
    TButtonPane::KeyPress(key, down);

    if (down)
    {
        switch (key)
        {
            case VK_UP:
                Scroll(-1);
                break;
            case VK_DOWN:
                Scroll(1);
                break;
            case VK_HOME:
                Scroll(-100000);
                break;
            case VK_END:
                Scroll(100000);
                break;
        }
    }
}

void TScrollPane::SetScroll(PTScroll s)
{
    scroll = s;

    if (scroll && scroll->GetText() && scrollfont)
        numlines = scrollfont->FindNumLinesInText(scroll->GetText(), SCROLLWIDTH);
    else
        numlines = 0;
}

void TScrollPane::Scroll(int numscrolllines)
{
    int oldline = line;

    line += numscrolllines;

    if (line < 0)
        line = 0;
    else if (line > numlines)
        line = numlines;

    if (oldline != line)
        SetDirty(TRUE);
}

// *************
// * TBookPane *
// *************

#define BOOKLINES       16      // depends on font and scroll height
#define BOOKWIDTH       216     // depends on font and scroll width

void BookBtnScrollUp()
{
    BookPane.Scroll(-(BOOKLINES * 2));
}

void BookBtnScrollDown()
{
    BookPane.Scroll(BOOKLINES * 2);
}

void BookBtnExit()
{
    BookPane.Close();
}

BOOL TBookPane::Initialize()
{
    TButtonPane::Initialize();

    line = 0;
    numlines = 0;

    scrolldata = TMulti::LoadMulti("book.dat");
    scrollfont = TFont::LoadFont(103);

    SetScroll(scroll);          // recompute number of lines now that font is loaded up

    if (scrolldata)
    {
        PTBitmap bitmap = scrolldata->Bitmap("book");
        int x = (WIDTH - bitmap->width) / 2;
        int y = (HEIGHT - bitmap->height) / 3;

        NewButton("down", x+440, y+386, 28, 22, VK_NEXT, BookBtnScrollDown, scrolldata->Bitmap("downdown"), scrolldata->Bitmap("downup"));
        NewButton("up", x+472, y+379, 28, 23, VK_PRIOR, BookBtnScrollUp, scrolldata->Bitmap("updown"), scrolldata->Bitmap("upup"));
        NewButton("exit", x+505, y+377, 28, 30, VK_ESCAPE, BookBtnExit, scrolldata->Bitmap("exitdown"), scrolldata->Bitmap("exitup"));

        PlayScreen.AddPane(this);
        PlayScreen.SetExclusivePane(this, TRUE);
        return TRUE;
    }

    return FALSE;
}

void TBookPane::DrawBackground()
{
    if (IsDirty())
    {
        PTBitmap bitmap = scrolldata->Bitmap("book");

        int x = (WIDTH - bitmap->width) / 2;
        int y = (HEIGHT - bitmap->height) / 3;
        Display->Put(x, y, bitmap, DM_TRANSPARENT | DM_BACKGROUND);

        if (scroll && scroll->GetText())
        {
            char buf[80];
            for (int side = 0; side < 2; side++)
            {
                sprintf(buf, "%d", (line / BOOKLINES) + side + 1);
                Display->WriteText(buf, x+180+(side*232), y+21+(side*2), 1, scrollfont, NULL, DM_USEDEFAULT, -1, 0, JUSTIFY_CENTER);

                Display->WriteText(scroll->GetText(), x+82+(side*232), y+40+(side*2), BOOKLINES, scrollfont, NULL, DM_USEDEFAULT, BOOKWIDTH, line + (side*BOOKLINES));
            }
        }

        // decide which buttons should be visisble
        if (line < 1)
            Button(1)->Hide();
        else
            Button(1)->Show();

        if ((line + (BOOKLINES*2) - 1) >= numlines)
            Button(0)->Hide();
        else
            Button(0)->Show();

        Button(2)->Show();

        RedrawButtons();
        SetDirty(FALSE);
    }

    // buttons shouldn't draw until after the rest is placed down
    TButtonPane::DrawBackground();
}

