// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   dialog.cpp - TDialogPane module                     *
// *************************************************************************

#include "revenant.h"
#include "dialog.h"
#include "display.h"
#include "multi.h"
#include "playscreen.h"
#include "character.h"
#include "command.h"
#include "bitmap.h"
#include "mappane.h"
#include "script.h"
#include "player.h"

TDialogPane DialogPane;

static PTObjectInstance DlgContext;
static BOOL savecontrolon;
static BOOL saveisfullscreen;

void SetDialogContext(PTObjectInstance context)
{
	DlgContext = context;
}

// Dialog line translator... Note: Dialog TAGS are listed in DLGTAG.TXT

char *DialogLine(char *line, char *outbuf, int buflen)
{
	char buf[128];
	char tag[20];
	char data[128];
	char *p, *b, *d;

	b = buf;
	for (p = line; *p != NULL; )
	{
		if (*p == '[')
		{
			p++;
			if (*p == '[' || *p == ']')
			{
				*b++ = *p++;
				continue;
			}
			char *t = tag;
			while (*p && *p != ']')
				*t++ = *p++;
			if (*p == ']')
				p++;
			*t++ = NULL;

			data[0] = NULL;
			if (!stricmp(tag, "me"))				// "me" is locke
				strcpy(data, Player->GetName());
			else if (!stricmp(tag, "chr") && DlgContext != NULL) // "chr" is the character talking
				strcpy(data, DlgContext->GetName());
			
			d = data;
			while (*d)
				*b++ = *d++;
		}
		else
			*b++ = *p++;
	}

	*b = NULL;

	return strncpyz(outbuf, buf, buflen);
}

BOOL TDialogPane::Initialize()
{
	if (IsOpen())
		return TRUE;

	if (!TPane::Initialize())
		return FALSE;

	dialogdata = TMulti::LoadMulti("dialog.dat");
	choice = -1;
	freshresponse = FALSE;

	for (int i = 0; i < MAXCHOICES; i++)
		choices[i] = label[i] = NULL;

	return TRUE;
}

void TDialogPane::Close()
{
	if (!IsOpen())
		return;

	TPane::Close();

	if (dialogdata)
		delete dialogdata;
}

void TDialogPane::Show()
{
	if (!IsHidden() || !IsOpen() || PlayScreen.IsDemoMode())
		return;

	TPane::Show();

	if (dialogdata)
	{
		saveisfullscreen = PlayScreen.IsFullScreen();
		PlayScreen.SetFullScreen(FALSE);
		PlayScreen.HideLowerPanes();
		savecontrolon = PlayScreen.IsControlOn();
		PlayScreen.SetControlOn(FALSE);		// don't want them wandering off or anything

//		OldScrollLock = ScrollLock;
//		ScrollLock = FALSE;
	}
	SetDirty(TRUE);
}

void TDialogPane::Hide()
{
	if (IsHidden() || !IsOpen())
		return;

	TPane::Hide();

	PlayScreen.ShowLowerPanes();
	PlayScreen.Redraw();
	PlayScreen.SetControlOn(savecontrolon);
	PlayScreen.SetFullScreen(saveisfullscreen);

	character = NULL;

//	ScrollLock = OldScrollLock;
}

#define CHOICEHEIGHT	21

void TDialogPane::DrawBackground()
{
	if (IsDirty())
	{
		Display->Put(0, 0, dialogdata->Bitmap("background"), DM_BACKGROUND);

		for (int i = 0; i < numchoices; i++)
		{
			SColor color = { 255, 0, 50 };
			Display->WriteText(choices[i], 32, (i * CHOICEHEIGHT) + 4, 1, GameData->Font("choicefont"),
								((grabslot == i || choice == i) && highlighted) ? &color : NULL);
		}

		SetDirty(FALSE);
	}
}

void TDialogPane::Animate(BOOL draw)
{
}

void TDialogPane::KeyPress(int key, BOOL down)
{
	if (down)
	{
		switch (key)
		{
			case '1':
				SetChoice(0);
				break;
			case '2':
				SetChoice(1);
				break;
			case '3':
				SetChoice(2);
				break;
			case '4':
				SetChoice(3);
				break;
			case ' ':
				Skip();
				break;
			case VK_ESCAPE:
				Skip();

				if (character)
					character->ScriptJump("Finish");

				Close();
		}
	}
}

void TDialogPane::MouseClick(int button, int x, int y)
{
	if (button == MB_LEFTDOWN)
	{
		if (numchoices > 0)
		{
			highlighted = TRUE;
			grabslot = OnSlot(x, y);
			SetDirty(TRUE);
		}
		else
			Skip();
	}
	else if (button == MB_LEFTUP && grabslot >= 0)
	{
		if (OnSlot(x, y) == grabslot)
			SetChoice(grabslot);

		grabslot = -1;
	}
}

void TDialogPane::MouseMove(int button, int x, int y)
{
	if (button == MB_LEFTDOWN)
	{
		int onslot = OnSlot(x, y);

		if ((onslot == grabslot && !highlighted) ||
			(onslot != grabslot && highlighted))
		{
			highlighted = !highlighted;
			SetDirty(TRUE);
		}
	}
}

int TDialogPane::OnSlot(int x, int y)
{
	if (!InPane(x, y))
		return -1;

	y -= 6;
	if (y < 0)
		return -1;

	y /= CHOICEHEIGHT;
	return y;
}

void TDialogPane::AddChoice(char *lab, char *txt)
{
	if (!IsOpen())
		return;

	if (choice >= 0)
		ResetResponses();

	if (numchoices >= MAXCHOICES)
		return;

	if (!txt)
		choices[numchoices] = NULL;
	else
		choices[numchoices] = _strdup(txt);

	label[numchoices] = lab ? _strdup(lab) : NULL;

	numchoices++;

	SetDirty(TRUE);
}

void TDialogPane::ResetResponses()
{
	if (!IsOpen())
		return;

	for (int i = 0; i < numchoices; i++)
	{
		if (choices[i])
			free(choices[i]);

		if (label[i])
			free(label[i]);
	}

	if (numchoices > 0)
		SetDirty(TRUE);		// only redraw if there wasn't anything there before

	numchoices = 0;
	choice = -1;
	freshresponse = FALSE;
	grabslot = -1;
}

void TDialogPane::Skip()
{
	if (!IsOpen())
		return;

	if (character && character->IsTalking())
		character->ForceCommandDone();
}


COMMAND(CmdChoice)
{
	if (t.Type() != TKN_IDENT)
		return CMD_BADPARAMS;

	char buf[80];
	strcpy(buf, t.Text());

	t.WhiteGet();
	if (t.Type() != TKN_TEXT)
		return CMD_BADPARAMS;

	DialogPane.AddChoice(buf, t.Text());

	t.Get();
	return 0;
}
