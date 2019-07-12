// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    Script.cpp - Script functions                      *
// *************************************************************************

#include <stdio.h>
#include <io.h>
#include <ctype.h>
#include <math.h>

#include "revenant.h"
#include "script.h"
#include "parse.h"
#include "object.h"
#include "command.h"
#include "mappane.h"
#include "dialog.h"
#include "file.h"
#include "exit.h"
#include "player.h"
#include "textbar.h"

#ifdef _WINDOWS_
#error WINDOWS.H shouldn't be included here!
#endif

int TScript::pauseall = FALSE;

PTObjectInstance TakenObject = NULL;
PTObjectInstance DroppedObject = NULL;

void ScriptError(char *buf, int linenum)
{
	char buf2[100];
	sprintf(buf2, "Script error at line %d: %s\n", linenum, buf);
	if (Editor)
		Output(buf2);
	else
		TextBar.Print(buf2);
}

void SkipBlock(TToken &t)
{
	if (!t.SkipBlock())
		ScriptError("BEGIN without matching END", 0);
}

// ***********
// * TScript *
// ***********

TScript::TScript(PTScriptProto prototype)
{
	proto = prototype;
	ip = NULL;
	priority = 0;
	depth = 0;
	lastpriority = 0;
	newtrigger = 0;
	newtriggerstr[0] = NULL;
	block[0].conditional = COND_UNDEF;
	block[0].loopstart = NULL;
}

TScript::~TScript()
{
}

BOOL TScript::Load(char *filename)
{
	return TRUE;
}

BOOL TScript::Save(char *filename)
{
	return TRUE;
}

void TScript::SetText(char *buf)
{
	proto->SetBuffer(buf);
	ScriptManager.SetScriptsDirty();
}
 
void TScript::Start(char *ptr, int newpriority)
{
	ip = ptr;
	lastpriority = priority = newpriority;

	// validate
	if (ip && (ip < proto->text || ip > proto->text+strlen(proto->text)))
		ip = NULL;

	if (!ip)
		priority = 0;
}

BOOL TScript::Triggered(PSScriptTrigger st, PTObjectInstance context)
{
	BOOL retval = FALSE;

  // ******************
  // !!!!!REMEMBER!!!!!
  // ******************
  
  // This function is being called often, so 
  // make sure you don't do anything to intense
  // here.  KEEP IT SIMPLE AND FAST!

	switch (st->type)
	{
	  case TRIGGER_ALWAYS:		// This trigger will always fire if it has a higherer priority
	  {
		retval = TRUE;
		break;
	  }
	  case TRIGGER_TRIGGER:		// This is the manually fired trigger (triggered by script 'trigger' command)
	  {
		if (newtrigger == TRIGGER_TRIGGER && 
		  !stricmp(st->name, newtriggerstr))
			retval = TRUE;
		break;
	  }
	  case TRIGGER_DIALOG:		// This trigger is triggered by clicking on a friendly character
	  {
		if (newtrigger == TRIGGER_DIALOG)
			retval = TRUE;
		break;
	  }
	  case TRIGGER_PROXIMITY:	// This allow you to set a floating proximity field for a character
	  {							// Slightly slow, so be careful where you use this
		if (newtrigger == TRIGGER_PROXIMITY)
			retval = TRUE;
		else 
		{
			if (Player && !stricmp(st->name, Player->GetName()))
				retval = context->Pos().InRange(Player->Pos(), st->dist);
			else
			{
//				PTObjectInstance inst = MapPane.ObjectInRange(context->Pos(), st->dist, OBJSET_CHARACTER);
//				if (st->name[0] == NULL || !stricmp(st->name, inst->GetName()))
//					retval = TRUE;
			}
		}
		break;
	  }
	  case TRIGGER_CUBE:		// Nice and fast cube trigger
	  {
		if (newtrigger == TRIGGER_CUBE)
			retval = TRUE;
		else if (!stricmp(st->name, context->GetName()))
		{
			if (st->cube.In(context->Pos()))
				retval = TRUE;
		}
		else if (Player && !stricmp(st->name, Player->GetName()))
		{
			if (st->cube.In(Player->Pos()))
				retval = TRUE;
		}
		else 
		{
			PTObjectInstance inst = MapPane.ObjectInCube(&st->cube, OBJSET_MOVING);
			if (inst && (st->name[0] == NULL || !stricmp(st->name, inst->GetName())))
				retval = TRUE;
		}
		break;
	  }
	  case TRIGGER_ACTIVATE:	// This trigger is triggered when object is activated
	  {
		if (newtrigger == TRIGGER_ACTIVATE)
			retval = TRUE;
		break;
	  }
	  case TRIGGER_USE:			// Triggered when character uses an object
	  {
		if (newtrigger == TRIGGER_USE && 
		  !stricmp(st->name, newtriggerstr))
			retval = TRUE;
		break;
	  }
	  case TRIGGER_GIVE:		// Triggered when this character give another char an object
	  {
		if (newtrigger == TRIGGER_GIVE && 
		  !stricmp(st->name, newtriggerstr))
			retval = TRUE;
		break;
	  }
	  case TRIGGER_GET:			// Triggered when this character gets an object
	  {
		if (newtrigger == TRIGGER_GET && 
		  !stricmp(st->name, newtriggerstr))
			retval = TRUE;
		break;
	  }
	  case TRIGGER_COMBAT:		// Triggered when this character enters combat mode
	  {
		if (newtrigger == TRIGGER_COMBAT)
			retval = TRUE;
		break;
	  }
	  case TRIGGER_DEAD:		// Triggered when we die (Blahhh.. uuhhhh!!)
	  {
		if (newtrigger == TRIGGER_DEAD)
			retval = TRUE;
		break;
	  }
	}

	return retval;
}

#define MAXITERATIONS	6000			// for catching endless loops

void TScript::Continue(PTObjectInstance context)
{
	if (priority & SCRIPT_PAUSED || pauseall)
		return;

  // Setup stream and token
	TStringParseStream s(proto->text, strlen(proto->text));
	TToken t(s);

	// Check Triggers For Interruption
	// *******************************

	int newpriority;						// an aribitrary large number
	for (int c = 0; c < proto->triggers.NumItems(); c++)
	{
		newpriority = 1000 - c;			// First trigger is highest priority, going down from there

		if (newpriority <= priority)	// Priority not higher than current trigger, so quit
			break;
		
		PSScriptTrigger st = &(proto->triggers[c]);

		if (Triggered(st, context))	// Check to see if script is triggered
		{
			if (lastpriority != newpriority)
			{
				Start(proto->text + st->pos, newpriority); // Start a new script for this trigger
				trigger = st->type;			// Set current trigger we're going to do
				newtrigger = 0;				// Set manual new trigger (if any) to 0
				newtriggerstr[0] = NULL;	// Set manual new trigger key (if any) to NULL
				break;
			}
		}
		else if (lastpriority == newpriority)
			lastpriority = 0;			// no longer being triggered, so clear it
	}

	// Now Continue Script
	// *******************

	// now execute the current thread
	s.SetPos((DWORD)ip);

	int iterations = MAXITERATIONS;

	while (ip && priority)
	{
	  // Prevent main char self running demo script from interrupting this script...
		if (Player && context != Player && Player->GetScript() &&
			Player->GetScript()->Running() && 
			Player->GetScript()->GetTrigger() == TRIGGER_ALWAYS)
		{
			Player->GetScript()->End();
		}

		DWORD thisline = s.GetPos();			// hang onto it in case of loop
		if (thisline > (DWORD)ip && !isspace(*((char *)thisline)) &&
									!isspace(*((char *)thisline - 1)))
			thisline--;							// token code jacks the pointer sometimes

		t.Get();
		t.SkipBlanks();

		if (t.Type() == TKN_SYMBOL && t.Code() == ':')
		{
			// label - ignore this line
			t.SkipLine();
		}
		else if (t.Type() == TKN_IDENT || t.Type() == TKN_KEYWORD)
		{
			// call the command interpreter
			int bits = CommandInterpreter(context, t);  // ****** MAIN COMMAND PROCESSOR HERE *****

			// interpret the return code(s)
			if (bits & CMD_DELETED)
				return;					// ack!

			if (bits & CMD_CONDTRUE)
			{
				block[depth].conditional = TRUE;
			}
			else if (bits & CMD_CONDFALSE)
			{
				t.LineGet();
				if (t.Is("BEGIN"))
					SkipBlock(t);
				else
					t.SkipLine();
				block[depth].conditional = FALSE;
			}

			if (bits & CMD_ELSE)
			{
				if (block[depth].conditional == COND_UNDEF)
					ScriptError("ELSE without matching IF", t.LineNum());
				else if (block[depth].conditional == TRUE)
				{
					t.LineGet();
					if (t.Is("BEGIN"))
						SkipBlock(t);
					else
						t.SkipLine();
					block[depth].conditional = COND_UNDEF;
				}
			}

			if (bits & CMD_SKIPBLOCK)
				SkipBlock(t);

			if (bits & CMD_BEGIN)
			{
				block[++depth].loopstart = NULL;
				block[depth].conditional = COND_UNDEF;
			}

			if (bits & CMD_END)
			{
				if (--depth < 0)
					ScriptError("END without matching BEGIN", t.LineNum());
			}

			if (block[depth].loopstart != NULL)
			{
				s.SetPos(block[depth].loopstart);
				block[depth].loopstart = NULL;
			}

			if (bits & CMD_LOOP)
				block[depth].loopstart = thisline;

			if (bits & CMD_WAIT)
			{
				ip = (char *)s.GetPos();
				break;
			}

			if (bits & CMD_JUMP)
			{
				s.SetPos((DWORD)ip);
			}
		}
		else
			ScriptError("Bad token in trigger block", t.LineNum());

		// skip over any extra crap on the line
		while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
			t.Get();

		if (depth < 1 || iterations-- < 1)
		{
			if (iterations < 1)
				ScriptError("Infinite loop detected\n", t.LineNum());

			ip = NULL;
			priority = 0;
		}
	}

	// Nothing to trigger
	if (ip == NULL)	// Script done
	{
		lastpriority = 0;
		if (!DialogPane.IsHidden() &&				// Dialog pane is still up and...
		  DialogPane.GetCharacter() == context)		// This script was using the dialog pane
			DialogPane.Hide();
	}
}

void TScript::Jump(PTObjectInstance context, char *label)
{
	TStringParseStream s(proto->text, strlen(proto->text));
	TToken t(s);
	int newpriority = 1000;

	// skip down to the current trigger
	while (t.Type() != TKN_EOF && --newpriority > priority)
	{
		SkipBlock(t);
		t.SkipBlanks();
	}

	// find the label
	while (1)
	{
		t.SkipBlanks();

		if (t.Type() == TKN_EOF)
		{
			ScriptError("Jump to an unknown label attempted", t.LineNum());
			Start();		// reset the script
			return;
		}

		if (t.Type() == TKN_SYMBOL && t.Code() == ':')
		{
			t.Get();
			if (t.Is(label))
			{
				// we have a match
				t.SkipLine();
				break;
			}
		}

		t.LineGet();
	}

	ip = (char *)s.GetPos();		// save the new position
	depth = 1;			// a bit hacky - probably needs to count the begin/end pairs..
}

void TScript::Break()
{
	priority |= SCRIPT_PAUSED;
}

void TScript::Resume()
{
	priority &= ~SCRIPT_PAUSED;
}

void TScript::End()
{
	ip = NULL;
	lastpriority = priority = 0;
}

// ****************
// * TScriptProto *
// ****************

BOOL TScriptProto::FitsCriteria(PTObjectInstance inst)
{
	if (name && *name && stricmp(name, inst->GetName()) == 0)
		return TRUE;

	return FALSE;
}

void TScriptProto::SetBuffer(char *buffer)
{
	text = strdup(buffer);
}

BOOL TScriptProto::ParseCriteria(TToken &t)
{
	if (t.Is("CONTEXT") || t.Is("OBJTYPE") || t.Is("OBJECT"))
	{
		t.WhiteGet();
		if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
			ScriptError("Expected object context identifier", t.LineNum());
		name = strdup(t.Text());
		t.LineGet();
	}
	else
	{
		t.SkipLine();
		return FALSE;
	}

	return TRUE;
}

#define TRIGBUFGROW 512
#define MAXTRIGSIZE 256

int TScriptProto::ParseScript(TToken &t)
{
	char buf[80];
	char trigname[20];

	t.SkipBlanks();

	ParseCriteria(t);

	t.SkipBlanks();
	if (!t.Is("BEGIN"))
		ScriptError("Object block BEGIN expected", t.LineNum());
	
	while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
		t.Get();

  // Mark line after begin as beginning of script buffer
	DWORD start = t.GetPos();
	int depth = 0;

  // Now get first trigger token
	t.LineGet();
	
  // Iterate through the triggers and setup trigger list
	while (t.Type() != TKN_EOF && !t.Is("END"))
	{
	  // Parse trigger tags now
		if (t.Type() != TKN_IDENT)
			ScriptError("Trigger identifier expected", t.LineNum());

      // Setup basic trigger structure
		SScriptTrigger st;
		memset(&st, 0, sizeof(SScriptTrigger));
		st.type = 0;

      // Tags...
		strcpy(trigname, t.Text());
		if (t.Is("ALWAYS"))					// ALWAYS trigger
		{
			t.WhiteGet();
			st.type = TRIGGER_ALWAYS;
		}
		else if (t.Is("TRIGGER"))					// ALWAYS trigger
		{
			t.WhiteGet();
			st.type = TRIGGER_TRIGGER;

			if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
			{
				strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
				t.WhiteGet();
			}
		}
		else if (t.Is("DIALOG"))			// DIALOG trigger
		{
			t.WhiteGet();
			st.type = TRIGGER_DIALOG;
		}
		else if (t.Is("PROXIMITY"))			// CUBE trigger
		{
			t.WhiteGet();

			st.type = TRIGGER_PROXIMITY;
			st.dist = 256;

			if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
			{
				strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
				t.WhiteGet();
			}

			if (t.Type() == TKN_NUMBER)
				Parse(t, "%i", &st.dist);
		}
		else if (t.Is("CUBE"))				// CUBE trigger
		{
			t.WhiteGet();

			st.type = TRIGGER_CUBE;

			if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
			{
				strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
				t.WhiteGet();
			}

			int x1, y1, z1, x2, y2, z2;

			if (!Parse(t, "%i,%i,%i %i,%i,%i", &x1, &y1, &z1, &x2, &y2, &z2))
				ScriptError("Invalid cube trigger", t.LineNum());

			st.cube.beg.x = min(x1, x2);
			st.cube.beg.y = min(y1, y2);
			st.cube.beg.z = min(z1, z2);
			st.cube.end.x = max(x1, x2);
			st.cube.end.y = max(y1, y2);
			st.cube.end.z = max(z1, z2);
		}
		else if (t.Is("ACTIVATE"))			// ACTIVATE trigger
		{
			t.WhiteGet();
			st.type = TRIGGER_ACTIVATE;
		}
		else if (t.Is("USE"))				// USE trigger
		{
			t.WhiteGet();
			st.type = TRIGGER_USE;
			
			if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
			{
				strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
				t.WhiteGet();
			}
		}
		else if (t.Is("GIVE"))				// GIVE trigger
		{
			t.WhiteGet();
			st.type = TRIGGER_GIVE;
			
			if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
			{
				strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
				t.WhiteGet();
			}
		}
		else if (t.Is("GET"))				// GET trigger
		{
			t.WhiteGet();
			st.type = TRIGGER_GET;
			
			if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
			{
				strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
				t.WhiteGet();
			}
		}
		else if (t.Is("COMBAT"))
		{
			t.WhiteGet();
			st.type = TRIGGER_COMBAT;
		}
		else if (t.Is("DEAD"))
		{
			t.WhiteGet();
			st.type = TRIGGER_DEAD;
		}
		else 
		{
			sprintf(buf, "Unknown trigger %s", t.Text());
			ScriptError(buf, t.LineNum());
		}

		if (t.Type() != TKN_RETURN)
		{
			sprintf(buf, "Bad parameter for %s trigger", trigname);
			ScriptError(buf, t.LineNum());
			t.SkipLine();
		}

		st.pos = t.GetPos() - start;
	
		t.SkipBlanks();

		if (st.type != 0)
			triggers.Add(st);

		if (!t.Is("BEGIN"))
		{
			ScriptError("Trigger BEGIN expected", t.LineNum());
			t.SkipLine();
		}

		t.SkipBlock();

		t.SkipBlanks();
	}

	if (!t.Is("END"))
		ScriptError("Object block END expected", t.LineNum());

	int len = (int)(t.GetPos() - start - 4); // Get length of buffer - 4 for end token (END\n)
	text = new char[len+1];
	memcpy(text, (void *)start, len);
	text[len] = 0;

	t.LineGet();

	return len;
}

BOOL TScriptProto::WriteScript(FILE *fp)
{
	if (fprintf(fp, "OBJECT \"%s\"\r\n", name) < 0)
		return FALSE;

	if (fputs("begin\r\n", fp) == EOF || fputs(text, fp) == EOF || fputs("end\r\n\r\n", fp) == EOF)
		return FALSE;

	return TRUE;
}

// **************
// * TGameState *
// **************

BOOL TGameState::Load(char *filename)
{
	char fname[MAX_PATH];
	sprintf(fname, "%s%s", ClassDefPath, filename);

	FILE *fp = fopen(fname, "rb");
	if (!fp)
		FatalError("Unable to find game state file %s", filename);

	TFileParseStream s(fp, fname);
	TToken t(s);

	t.LineGet();

	numstates = 0;

	while (t.Type() != TKN_EOF)
	{
		t.SkipBlanks();
		if (t.Type() == TKN_IDENT)
		{
			statename[numstates] = new char[strlen(t.Text()) + 1];
			strcpy(statename[numstates], t.Text());
			t.WhiteGet();

			if (t.Is("="))
				t.WhiteGet();

			if (t.Type() != TKN_NUMBER)
				FatalError("Invalid init value for game state in %s", filename);

			state[numstates] = t.Index();

			numstates++;

			t.LineGet();
		}
		else
			FatalError("Expected gamestate identifier in %s", filename);

	}

	fclose(fp);

	return TRUE;
}

BOOL TGameState::Save(char *filename)
{
	char fname[MAX_PATH];
	sprintf(fname, "%s%s", ClassDefPath, filename);

	FILE *fp = popen(fname, "wt");
	if (!fp)
		return FALSE;

	fprintf(fp, "// ********* Revenant Game States Save File ********\n"
	            "// -------------------------------------------------\n\n"
				"// Revenant - Copyright 1998 Cinematix Studios, Inc.\n\n");

	for (int c = 0; c < numstates; c++)
	{
		fprintf(fp, "%s=%d\n", statename[c], state[c]);
	}

	fclose(fp);

	return TRUE;
}

// ******************
// * TScriptManager *
// ******************

BOOL TScriptManager::Initialize()
{
	scripts.Clear();

	if (!_CrtCheckMemory())
	{
		_CrtMemDumpAllObjectsSince(NULL);
		_RPT0(_CRT_ERROR, "Memory Error");
	}

	return (Load("master.s") && gamestate.Load("state.def"));
}

void TScriptManager::Close()
{
	if (Editor)
	{
		Save("master.s");
		gamestate.Save("state.def");
	}

	scripts.DeleteAll();
}

BOOL TScriptManager::Load(char *filename, void *owner)
{
	char fname[MAX_PATH];

	if (!_CrtCheckMemory())
	{
		_CrtMemDumpAllObjectsSince(NULL);
		_RPT0(_CRT_ERROR, "Memory Error");
	}

	sprintf(fname, "%s%s", ClassDefPath, filename);

	FILE *fp = TryOpen(fname, "rb");
	if (fp == NULL)
		FatalError("Unable to find game master script file %s", filename);

	BOOL retval = TRUE;
	int bufsize = _filelength(fileno(fp));
	char *buffer = new char[bufsize+1];

	if (fread(buffer, 1, bufsize, fp) < (size_t)bufsize)
		retval = FALSE;
	else
	{
		buffer[bufsize] = 0;			// null-terminate it
		ParseScripts(buffer, owner);
	}

	delete buffer;
	fclose(fp);

	if (MapPane.IsOpen())
		MapPane.Notify(N_SCRIPTADDED, NULL);

	scriptsdirty = FALSE;

	if (!_CrtCheckMemory())
	{
		_CrtMemDumpAllObjectsSince(NULL);
		_RPT0(_CRT_ERROR, "Memory Error");
	}

	return retval;
}

BOOL TScriptManager::Save(char *filename, void *owner)
{
//	if (!scriptsdirty)
		return TRUE;

	char fname[MAX_PATH];

	sprintf(fname, "%s%s", ClassDefPath, filename);

	FILE *fp = TryOpen(fname, "wb");
	if (fp == NULL)
		return FALSE;

	fprintf(fp, "// ************** Revenant Script File  ************\n"
	            "// -------------------------------------------------\n\n"
				"// Revenant - Copyright 1998 Cinematix Studios, Inc.\n\n");

	BOOL retval = TRUE;
	for (int c = 0; c < scripts.NumItems(); c++)
	{
		if (!scripts.Used(c))
			continue;

		if (scripts[c]->owner == owner)
		{
			if (!scripts[c]->WriteScript(fp))
				retval = FALSE;
		}
	}

	fclose(fp);

	scriptsdirty = FALSE;

	return retval;
}

void TScriptManager::Clear(void *owner)
{
	for (int c = 0; c < scripts.NumItems(); c++)
	{
		if (!scripts.Used(c))
			continue;

		if (scripts[c]->owner == owner)
		{
			MapPane.Notify(N_SCRIPTDELETED, scripts[c]);

			scripts.Delete(c);
		}
	}
}	

BOOL TScriptManager::ReloadStates()
{
	return gamestate.Load("state.def");
}

void TScriptManager::ParseScripts(char *buffer, void *owner)
{
	TStringParseStream s(buffer, strlen(buffer));
	TToken t(s);

	t.Get();

	while (t.Type() != TKN_EOF)
	{
		PTScriptProto script = new TScriptProto;
		script->ParseScript(t);
		script->owner = owner;
	
		for (int c = 0; c < scripts.NumItems(); c++) // Add into unused entries
		{
			if (!scripts.Used(c))  
			{
				scripts.Set(script, c);
				break;
			}
		}
		if (c >= scripts.NumItems())
			scripts.Add(script);
	}
}

// This finds all the scripts which apply to a given object instance and put pointers to
// them into the new script object (which is returned).
PTScript TScriptManager::ObjectScript(PTObjectInstance inst)
{
	for (int c = 0; c < scripts.NumItems(); c++)
	{
		if (!scripts.Used(c))
			continue;

		if (scripts[c]->FitsCriteria(inst))
		{
			PTScript newscript = new TScript(scripts[c]);
			return newscript;
		}
	}

	return NULL;
}

struct { char *name; int val; } LocalScriptVals[] =
{
	{ "Name",				0	},
	{ "IsShopObject",		0	},
	{ "Tab",				0	},

	{ NULL }		// terminator
};

int TScriptManager::FindLocalVal(char *name)
{
	for (int i = 0; LocalScriptVals[i].name; i++)
		if (stricmp(name, LocalScriptVals[i].name) == 0)
			return i;

	return -1;
}

void TScriptManager::SetLocalVal(int index, int val)
{
	if (index >= 0)
		LocalScriptVals[index].val = val;
}

int TScriptManager::GetLocalVal(int index)
{
	if (index >= 0)
		return LocalScriptVals[index].val;

	return 0;
}

int TScriptManager::GameState(char *name)
{
	// first check the constants set up by the currently executing script trigger
	int index = FindLocalVal(name);
	if (index >= 0)
		return GetLocalVal(index);

	// if it's not there, check the game state variables (globals)
	return gamestate.State(name);
}

