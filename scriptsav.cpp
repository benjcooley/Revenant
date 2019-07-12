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
    topproto = curproto = prototype;
    ip = NULL;
    priority = 0;
    depth = 0;
    lastpriority = 0;
    newtrigger = 0;
    newtriggerstr[0] = NULL;
    block[0].conditional = COND_UNDEF;
    block[0].loopstart = NULL;
}

TScript() 
{ 
    proto = NULL; 
    ip = NULL; 
    priority = 0; 
    depth = 0; 
    newtrigger = 0; 
    lastpriority = 0;
    block[depth].conditional = COND_UNDEF; 
    block[depth].loopstart = NULL; 
    ScriptManager.AddScript(this); 
}

TScript::~TScript()
{
}

void TScript::SetText(char *buf)
{
    topproto->SetBuffer(buf);
    ScriptManager.SetScriptsDirty();
}
 
void TScript::Start(PTScriptProto proto, int pos, int newpriority)
{
    curproto = proto;

    if (pos < 0 || pos >= curproto->Length())
        return;

    ip = curproto->Text() + pos;
}

void TScript::StartTrigger(PTScriptProto proto, PSScriptTrigger st)
{
    Start(proto, st->pos, st->priority);
    trigger = st->type;         // Set current trigger we're going to do
    newtrigger = 0;             // Set manual new trigger (if any) to 0
    newtriggerstr[0] = NULL;    // Set manual new trigger key (if any) to NULL
}

BOOL TScript::Triggered(PSScriptTrigger st, int priority, PTObjectInstance context)
{
    BOOL retval = FALSE;

  // Are we already doing this trigger?
    if (curtrigger == st || st->priority < priority)
        return FALSE;

  // ******************
  // !!!!!REMEMBER!!!!!
  // ******************
  
  // This function is being called often, so 
  // make sure you don't do anything to intense
  // here.  KEEP IT SIMPLE AND FAST!

    switch (st->type)
    {
      case TRIGGER_ALWAYS:      // This trigger will always fire if it has a higherer priority
      {
        retval = TRUE;
        break;
      }
      case TRIGGER_TRIGGER:     // This is the manually fired trigger (triggered by script 'trigger' command)
      {
        if (newtrigger == TRIGGER_TRIGGER && 
          !stricmp(st->name, newtriggerstr))
            retval = TRUE;
        break;
      }
      case TRIGGER_DIALOG:      // This trigger is triggered by clicking on a friendly character
      {
        if (newtrigger == TRIGGER_DIALOG)
            retval = TRUE;
        break;
      }
      case TRIGGER_PROXIMITY:   // This allow you to set a floating proximity field for a character
      {                         // Slightly slow, so be careful where you use this
        if (newtrigger == TRIGGER_PROXIMITY)
            retval = TRUE;
        else 
        {
            if (Player && !stricmp(st->name, Player->GetName()))
                retval = context->Pos().InRange(Player->Pos(), st->dist);
            else
            {
//              PTObjectInstance inst = MapPane.ObjectInRange(context->Pos(), st->dist, OBJSET_CHARACTER);
//              if (st->name[0] == NULL || !stricmp(st->name, inst->GetName()))
//                  retval = TRUE;
            }
        }
        break;
      }
      case TRIGGER_CUBE:        // Nice and fast cube trigger
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
      case TRIGGER_ACTIVATE:    // This trigger is triggered when object is activated
      {
        if (newtrigger == TRIGGER_ACTIVATE)
            retval = TRUE;
        break;
      }
      case TRIGGER_USE:         // Triggered when character uses an object
      {
        if (newtrigger == TRIGGER_USE && 
          !stricmp(st->name, newtriggerstr))
            retval = TRUE;
        break;
      }
      case TRIGGER_GIVE:        // Triggered when this character give another char an object
      {
        if (newtrigger == TRIGGER_GIVE && 
          !stricmp(st->name, newtriggerstr))
            retval = TRUE;
        break;
      }
      case TRIGGER_GET:         // Triggered when this character gets an object
      {
        if (newtrigger == TRIGGER_GET && 
          !stricmp(st->name, newtriggerstr))
            retval = TRUE;
        break;
      }
      case TRIGGER_COMBAT:      // Triggered when this character enters combat mode
      {
        if (newtrigger == TRIGGER_COMBAT)
            retval = TRUE;
        break;
      }
      case TRIGGER_DEAD:        // Triggered when we die (Blahhh.. uuhhhh!!)
      {
        if (newtrigger == TRIGGER_DEAD)
            retval = TRUE;
        break;
      }
    }

    return retval;
}

#define MAXITERATIONS   6000            // for catching endless loops

void TScript::Continue(PTObjectInstance context)
{
    if (priority & SCRIPT_PAUSED || pauseall)
        return;

  // Setup stream and token
    TStringParseStream s(curproto->text, curproto->len);
    TToken t(s);

    // Check Triggers For Interruption
    // *******************************

    PTScriptPrototype proto = topproto;
    BOOL foundtrigger = FALSE;
    while (proto)
    {
        for (int c = 0; c < proto->NumTriggers(); c++)
        {
            if (Triggered(st, priority, context))   // Check to see if script is triggered
            {
                foundtrigger = TRUE;
                StartTrigger(proto, st);
                break;
            }
        }
        if (foundtrigger)
            break;
        proto = proto->ParentProto();
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

        DWORD thisline = s.GetPos();            // hang onto it in case of loop
        if (thisline > (DWORD)ip && !isspace(*((char *)thisline)) &&
                                    !isspace(*((char *)thisline - 1)))
            thisline--;                         // token code jacks the pointer sometimes

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
                return;                 // ack!

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
    if (ip == NULL) // Script done
    {
        lastpriority = 0;
        if (!DialogPane.IsHidden() &&               // Dialog pane is still up and...
          DialogPane.GetCharacter() == context)     // This script was using the dialog pane
            DialogPane.Hide();
    }
}

void TScript::Jump(PTObjectInstance context, char *label)
{
    TStringParseStream s(curproto->text, strlen(curproto->text));
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
            Start();        // reset the script
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

    ip = (char *)s.GetPos();        // save the new position
    depth = 1;          // a bit hacky - probably needs to count the begin/end pairs..
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

TScriptProto::TScriptProto(PTScriptProto pparent, void *powner, char *pfilename, char *pbuffer)
{
    parent = pparent;
    owner = powner;
    filename = new char[strlen(pfilename) + 1];
    strcpy(filename, pfilename);
    name = NULL;
    text = NULL;
    textlen = 0;
    if (buffer)
        SetBuffer(buffer);
}

TScriptProto::~TScriptProto()
{
    if (name)
        free(name);
    if (filename)
        free(filename);
    if (text)
        free(text);
}

BOOL TScriptProto::FitsCriteria(PTObjectInstance inst)
{
    if (name && *name && stricmp(name, inst->GetName()) == 0)
        return TRUE;

    return FALSE;
}

void TScriptProto::SetBuffer(char *buffer)
{
    if (!buffer || strlen(buffer) < 1)
        return;

    if (text)
        free(text);

    text = strdup(buffer);
    textlen = strlen(text);

    TStringParseStream s(text, textlen);
    TToken t(s);

    t.LineGet();
    ParseScript(t); 
}

void TScriptProto::GetBuffer(char *buffer, int buflen)
{
    if (!buffer || buflen <= 0)
        return;

    strncpyz(buffer, text, buflen);
}

BOOL TScriptProto::ParseCriteria(TToken &t)
{
    if (t.Is("CONTEXT") || t.Is("OBJTYPE") || t.Is("OBJECT"))
    {
        t.WhiteGet();
        if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
            ScriptError("Expected object context identifier", t.LineNum());
        name = strdup(t.Text());
        t.WhiteGet();

        if (t.Is("PARENT"))
        {
            t.WhiteGet();

            if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
                ScriptError("Expected object parent identifier", t.LineNum());

            parent = ScriptManager.FindScriptProto(t.Text());

            t.WhiteGet();
        }

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

  // Clear current list of triggers before we begin
    triggers.Clear();

  // Skip initial blanks
    t.SkipBlanks();

  // Parse script header line
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
        if (t.Is("ALWAYS"))                 // ALWAYS trigger
        {
            t.WhiteGet();
            st.type = TRIGGER_ALWAYS;
        }
        else if (t.Is("TRIGGER"))                   // ALWAYS trigger
        {
            t.WhiteGet();
            st.type = TRIGGER_TRIGGER;

            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
                t.WhiteGet();
            }
        }
        else if (t.Is("DIALOG"))            // DIALOG trigger
        {
            t.WhiteGet();
            st.type = TRIGGER_DIALOG;
        }
        else if (t.Is("PROXIMITY"))         // CUBE trigger
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
        else if (t.Is("CUBE"))              // CUBE trigger
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
        else if (t.Is("ACTIVATE"))          // ACTIVATE trigger
        {
            t.WhiteGet();
            st.type = TRIGGER_ACTIVATE;
        }
        else if (t.Is("USE"))               // USE trigger
        {
            t.WhiteGet();
            st.type = TRIGGER_USE;
            
            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
                t.WhiteGet();
            }
        }
        else if (t.Is("GIVE"))              // GIVE trigger
        {
            t.WhiteGet();
            st.type = TRIGGER_GIVE;
            
            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
                t.WhiteGet();
            }
        }
        else if (t.Is("GET"))               // GET trigger
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
        buffer[bufsize] = 0;            // null-terminate it
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
//  if (!scriptsdirty)
        return TRUE;

    char fname[MAX_PATH];

    sprintf(fname, "%s%s", ClassDefPath, filename);

    FILE *fp = TryOpen(fname, "wt");
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

void TScriptManager::ParseScripts(char *buffer, char *filename, void *owner)
{
    TStringParseStream s(buffer, strlen(buffer));
    TToken t(s);

    t.Get();

    while (t.Type() != TKN_EOF)
    {
        PTScriptProto script = new TScriptProto(owner, filename);
        script->ParseScript(t);

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
    { "Name",               0   },
    { "IsShopObject",       0   },
    { "Tab",                0   },

    { NULL }        // terminator
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

class TScriptArray
{
  public:
    virtual int Size() = 0;
      // Returns size of array
    virtual int NumItems() = 0;
      // Returns number of items in array
    virtual int GetData(PTScriptVar index, void* &data, int &len) = 0;
      // Gets data to element in array or returns error
    virtual int SetData(PTScriptVar index, void *data, int len) = 0;
      // Sets data element in array or returns error
};

enum {
    VAR_NIL,
    VAR_INT,
    VAR_BOOL,
    VAR_SET,
    VAR_FLOAT,
    VAR_STRING,
    VAR_STRUCT,
    VAR_ARRAY,
    VAR_OBJECT
} VARTYPE;
#define NUMVARS ((int)VAR_ARRAY + 1)

enum {
    OPER_ARRAY,
    OPER_INC,
    OPER_DEC,
    OPER_NEGATE,
    OPER_NOT,
    OPER_BNOT,
    OPER_MULT,
    OPER_DIV,
    OPER_MOD,
    OPER_PLUS,
    OPER_MINUS,
    OPER_LSHIFT,
    OPER_RSHIFT,
    OPER_LESSTHAN,
    OPER_GREATERTHAN,
    OPER_LESSTHANEQUAL,
    OPER_GREATERTHANEQUAL,
    OPER_EQUAL,
    OPER_NOTEQUAL,
    OPER_AND,
    OPER_OR,
    OPER_XOR,
    OPER_BAND,
    OPER_BOR,
    OPER_BXOR,
    OPER_ASSIGN
};

struct SOperator
{
    int prec;
};

_CLASSDEF(TVar)
class TVar
{
    friend class TType;

  public:

  // Create variables of different types
    TVar() { type = VAR_NIL; i = 0; }
    TVar(int pi) { type = VAR_INT; i = pi; }
    TVar(float pf) { type = VAR_FLOAT; f = pf; }
    TVar(char *ps) { type = VAR_STRING; s = strdup(ps); }
    TVar(PTObjectInstance inst) { type = VAR_OBJECT; p = inst; }

  // Deconstructing harry..
    virtual ~TVar() { Clear(); }

  // Get variable type crapola
    VARTYPE Type() { return type; }
      // Returns type of variable
    WORD Index() { return index; }
      // Returns the variables index value
    char *TypeName() { return types[type].name; }
      // Returns name of type

  // Clear this variable
    void Clear() { if (types[type].Allocated()) types[type]->Clear(this); }

  // Get value functions
    BOOL GetBool() 
        { return types[type]->GetBool(this); }
    int GetInt()
        { return types[type]->GetInt(this); }
    float GetFloat()
        { return types[type]->GetFloat(this); }
    char *DupString()
        { return types[type]->DupString(this); }
    char *CopyString(char *buf, int buflen)
        { return types[type]->CopyString(this, buf, buflen); }
    void *GetPointer()
        { return types[type]->GetPointer(this); }
    PTVar GetItem(PTVar index)
        { return types[type]->GetItem(this, index); }

  // Set value functions
    void SetBool(int pb) { Clear(); type = TYPE_BOOL; i = (pi != 0); }
    void SetInt(int pi) { Clear(); type = TYPE_INT; i = pi; }
    void SetFloat(float pf) { Clear(); type = TYPE_FLOAT; f = pf; }
    void SetString(char *ps, int len = -1);
    void SetPointer(char *ptr, VARTYPE ptype) { Clear(); type = ptype; p = ptr; }

  // Set array element
    int SetItem(PTVar item, PTVar index) { return types[type].SetItem(this, item, index); }

  // Evaluation
    int Evaluate(OPERATOR oper, RTVar d, PTVar v2 = NULL)
        { return types[type]->Evaluate(oper, d, this, v2); }
      // Evaluates the given operation given the operator 'oper', the destination 'd', and
      // the optional second operand 'v2'.  

  private:
    VARTYPE type;       // Basic type id of object
    WORD index;         // Multipurpose type index 
    union               // Union of basic types
    {
        PTScriptArray array;
        char *s;
        int i;
        float f;
        void *p;
    };
};

void TVar::SetString(char *ps, int len = -1)
{
    Clear();
    type = TYPE_STRING;
    if (len >= 0)
        index = len;
    else
        index = strlen(ps);
    index = min(index, MAXSTRING);
    s = malloc(index + 1);           // Evil hackers will try to use string overflows.. 
    strncpyz(s, ps, index + 1);      // but we won't let them!
}

_CLASSDEF(TType)
class TType
{
  public:
    TType(char *pname, BOOL palloc) { name = pname; alloc = palloc; }

  // Returns the name of this type
    char *Name() { return name; }
    BOOL Allocated() { return alloc; }

  // Clears contents of variable
    virtual void Clear(PTVar v)  = 0;

  // Conversion functions for type
    virtual int GetInt(PTVar v) = 0;        // Attempts to convert to an int
    virtual float GetFloat(PTVar v) = 0;    // Attempts to convert to a float
    virtual char *GetString(PTVar v) = 0;   // Returns string only if type is a string type
    virtual char *DupString(PTVar v) = 0;   // Duplicates string, or creates a new string
    virtual char *CopyString(PTVar v, char *buf, int buflen) = 0;  // Copies string to buf
    virtual void *GetPointer(PTVar v) = 0;  // Returns a pointer if type points to object   
    virtual PTVar GetItem(PTVar v, PTVar index) = 0;  // Returns an item if this is an array

  // Set array element
    virtual int SetItem(PTVar v, PTVar item, PTVar index) = 0;

  // Evaluates the given operation given the operator 'oper', the destination 'd', and
  // the operands v1 and v2
    virtual int Evaluate(OPERATOR oper, RTVar d, PTVar v1, PTVar v2 = NULL) = 0;

  private:
    char *name;
    BOOL alloc;
}

_CLASSDEF(TNilType)
class TNilType : TType
{
  public:
    TNilType() : TType("nil", FALSE) {}

  // Clears contents of variable
    virtual void Clear(PTVar v) {}

  // Conversion functions for type
    virtual int GetInt(PTVar v) { return 0; }
    virtual float GetFloat(PTVar v) { return 0.0f; }
    virtual char *DupString(PTVar v) { return strdup(""); }
    virtual char *CopyString(PTVar v, char *buf, int buflen) { buf[0] = NULL; }
    virtual void *GetPointer(PTVar v) { return NULL; }
    virtual PTVar GetItem(PTVar v, PTVar index) { return NULL; }

  // Set array element
    virtual void SetItem(PTVar v, PTVar item, PTVar index) { return CMD_NO_ARRAY; }

  // Evaluates the given operation given the operator 'oper', the destination 'd', and
  // the operands v1 and v2
    virtual int Evaluate(OPERATOR oper, RTVar d, PTVar v1, PTVar v2 = NULL);
};

// Evaluates the given operation given the operator 'oper', the destination 'd', and
// the optional second operand 'v'.  
int TNilType::Evaluate(OPERATOR oper, RTVar d, PTVar, PTVar v2)
{
    switch (oper)
    {
      case OPER_EQUAL:
        d.SetInt(v2->type == VAR_NIL);
        break;
      case OPER_NOTEQUAL:
        d.SetInt(v2->type != VAR_NIL);
        break;
      case OPER_ASSIGN:
        d.SetNil();
        break;
      default:
        return CMDERR_BAD_OPERATOR;
    }

    return CMDERR_OK;
};

_CLASSDEF(TIntType)
class TIntType : TType
{
    TIntType(char *name, BOOL alloc) : TType(name, alloc) {}

  // Clears contents of variable
    virtual void Clear(PTVar v) {}

  // Conversion functions for type
    virtual int GetInt(PTVar v) { return v->i; }
    virtual float GetFloat(PTVar v) { return (float)v->i; }
    virtual char *GetString(PTVar v) { return NULL; }
    virtual char *DupString(PTVar v); 
    virtual char *CopyString(PTVar v, char *buf, int buflen); 
    virtual void *GetPointer(PTVar v) { return NULL; }
    virtual PTVar GetItem(PTVar v, PTVar index) { return NULL; }

  // Set array element
    virtual void SetItem(PTVar v, PTVar item, PTVar index) { return CMD_NO_ARRAY; }

    virtual int Evaluate(OPERATOR oper, RTVar d, PTVar v1, PTVar v2 = NULL);
      // Evaluates the given operation given the operator 'oper', the destination 'd', and
      // the operands v1 and v2
};

char *TIntType::DupString(PTVar v)
{
    char buf[13];

    itoa(v->i, buf, 10);
    return strdup(buf);
}

char *TIntType::CopyString(PTVar v, char *buf, int buflen) 
{
    char temp[13];

    if (buflen >= 12)
    {
        itoa(v->i, buf, 10);
    } 
    else
    {
        itoa(v->i, temp, 10);
        strncpyz(buf, temp, buflen);
    }

    return buf;
}

// Evaluates the given operation given the operator 'oper', the destination 'd', and
// the operands v1 and v2
int TIntType::Evaluate(OPERATOR oper, RTVar d, PTVar v1, PTVar v2)
{
    int t;

    switch (oper)
    {
      case OPER_INC:
        d.SetInt(++(v1->i));
        break;
      case OPER_DEC:
        d.SetInt(--(v1->i));
        break;
      case OPER_NEGATE:
        d.SetInt(-(v1->i));
        break;
      case OPER_NOT:
        d.SetInt(!(v1->i));
        break;
      case OPER_BNOT:
        d.SetInt(~(v1->i));
        break;
      case OPER_MULT:
        d.SetInt(v1->i * v2->GetInt());
        break;
      case OPER_DIV:
        t = v2->GetInt();
        if (t == 0)
            return CMDERR_DIVIDE_BY_ZERO;
        else
            d.SetInt(v1->i / t);
        break;
      case OPER_MOD:
        t = v2->GetInt();
        if (t == 0)
            return CMDERR_DIVIDE_BY_ZERO;
        else
            d.SetInt(v1->i % t);
        break;
      case OPER_PLUS:
        d.SetInt(v1->i + v2->GetInt());
        break;
      case OPER_MINUS:
        d.SetInt(v1->i - v2->GetInt());
        break;
      case OPER_LSHIFT:
        d.SetInt(v1->i << v2->GetInt());
        break;
      case OPER_RSHIFT:
        d.SetInt(v1->i >> v2->GetInt());
        break;
      case OPER_LESSTHAN:
        d.SetBool(v1->i < v2->GetInt());
        break;
      case OPER_GREATERTHAN:
        d.SetBool(v1->i > v2->GetInt());
        break;
      case OPER_LESSTHANEQUAL:
        d.SetBool(v1->i <= v2->GetInt());
        break;
      case OPER_GREATERTHANEQUAL:
        d.SetBool(v1->i >= v2->GetInt());
        break;
      case OPER_EQUAL:
        d.SetBool(v1->i == v2->GetInt());
        break;
      case OPER_NOTEQUAL:
        d.SetBool(v1->i != v2->GetInt());
        break;
      case OPER_AND:
        d.SetBool(v1->i && v2->GetInt());
        break;
      case OPER_OR:
        d.SetBool(v1->i || v2->GetInt());
        break;
      case OPER_XOR:
        d.SetBool((v1->i != 0) ^ (v2->GetInt() != 0));
        break;
      case OPER_BAND:
        d.SetInt(v1->i & v2->GetInt());
        break;
      case OPER_BOR:
        d.SetInt(v1->i | v2->GetInt());
        break;
      case OPER_BXOR:
        d.SetInt(v1->i ^ v2->GetInt());
        break;
      case OPER_ASSIGN:
        v1->SetInt(v2->GetInt());
        d.SetInt(v1->i);
        break;
      default:
        return CMDERR_BAD_OPERATOR;
    }

    return CMDERR_OK;
};

_CLASSDEF(TBoolType)
class TBoolType : TIntType
{
    TBoolType() : TIntType("bool", FALSE) {}
};

_CLASSDEF(TSetType)
class TSetType : TIntType
{
    TSetType() : TIntType("set", FALSE) {}
};

_CLASSDEF(TFloatType)
class TFloatType : TType
{
    TFloatType() : TType("float", FALSE) {}

  // Clears contents of variable
    virtual void Clear(PTVar v) {}

  // Conversion functions for type
    virtual int GetInt(PTVar v) { return (int)v->f; }
    virtual float GetFloat(PTVar v) { return v->f; }
    virtual char *GetString(PTVar v) { return NULL; }
    virtual char *DupString(PTVar v); 
    virtual char *CopyString(PTVar v, char *buf, int buflen); 
    virtual void *GetPointer(PTVar v) { return NULL; }
    virtual PTVar GetItem(PTVar v, PTVar index) { return NULL; }

  // Set array element
    virtual void SetItem(PTVar v, PTVar item, PTVar index) { return CMD_NO_ARRAY; }

    virtual int Evaluate(OPERATOR oper, RTVar d, PTVar v1, PTVar v2 = NULL);
      // Evaluates the given operation given the operator 'oper', the destination 'd', and
      // the operands v1 and v2
};

char *TFloatType::DupString(PTVar v)
{
    char buf[40];
    sprintf(buf, "%f", (double)v->f);
    return strdup(buf);
}

char *TFloatType::CopyString(PTVar v, char *buf, int buflen) 
{
    char temp[40];

    if (buflen >= 40)
    {
        sprintf(buf, "%f", (double)v->f);
    } 
    else
    {
        sprintf(temp, "%f", (double)v->f);
        strncpyz(buf, temp, buflen);
    }

    return buf;
}

// Evaluates the given operation given the operator 'oper', the destination 'd', and
// the operands v1 and v2
int TFloatType::Evaluate(OPERATOR oper, RTVar d, PTVar v1, PTVar v2)
{
    float t;
    int it;

    switch (oper)
    {
      case OPER_INC:
        d.SetFloat(v1->f += 1.0f));
        break;
      case OPER_DEC:
        d.SetFloat(v1->f -= 1.0f));
        break;
      case OPER_NEGATE:
        d.SetFloat(-(v1->f));
        break;
      case OPER_NOT:
        d.SetFloat(!(v1->f));
        break;
      case OPER_BNOT:
        d.SetFloat(~(v1->f));
        break;
      case OPER_MULT:
        d.SetFloat(v1->f * v2->GetFloat());
        break;
      case OPER_DIV:
        t = v2->GetFloat();
        if (t == 0.0f)
            return CMDERR_DIVIDE_BY_ZERO;
        else
            d.SetFloat(v1->f / t);
        break;
      case OPER_MOD:
        it = v2->GetInt();
        if (it == 0)
            return CMDERR_DIVIDE_BY_ZERO;
        else
            d.SetFloat((int)v1->f % it);
        break;
      case OPER_PLUS:
        d.SetFloat(v1->f + v2->GetFloat());
        break;
      case OPER_MINUS:
        d.SetFloat(v1->f - v2->GetFloat());
        break;
      case OPER_LSHIFT:
        d.SetFloat((int)v1->f << v2->GetInt());
        break;
      case OPER_RSHIFT:
        d.SetFloat((int)v1->f >> v2->GetInt());
        break;
      case OPER_LESSTHAN:
        d.SetBool(v1->f < v2->GetFloat());
        break;
      case OPER_GREATERTHAN:
        d.SetBool(v1->f > v2->GetFloat());
        break;
      case OPER_LESSTHANEQUAL:
        d.SetBool(v1->f <= v2->GetFloat());
        break;
      case OPER_GREATERTHANEQUAL:
        d.SetBool(v1->f >= v2->GetFloat());
        break;
      case OPER_EQUAL:
        d.SetBool(v1->f == v2->GetFloat());
        break;
      case OPER_NOTEQUAL:
        d.SetBool(v1->f != v2->GetFloat());
        break;
      case OPER_AND:
        d.SetBool((int)v1->f && v2->GetInt());
        break;
      case OPER_OR:
        d.SetBool((int)v1->f || v2->GetInt());
        break;
      case OPER_XOR:
        d.SetBool((int)(v1->f != 0) ^ (v2->GetInt() != 0));
        break;
      case OPER_BAND:
        d.SetFloat((int)v1->f & v2->GetInt());
        break;
      case OPER_BOR:
        d.SetFloat((int)v1->f | v2->GetInt());
        break;
      case OPER_BXOR:
        d.SetFloat((int)v1->f ^ v2->GetInt());
        break;
      case OPER_ASSIGN:
        v1->SetFloat(v2->GetFloat());
        d.SetFloat(v1->f);
        break;
      default:
        return CMDERR_BAD_OPERATOR;
    }

    return CMDERR_OK;
};

_CLASSDEF(TStringType)
class TStringType : TType
{
    TStringType() : TType("string", TRUE) {}

  // Clears contents of variable
    virtual void Clear(PTVar v) { if (v->s) free(v->s); v->s = NULL; v->type = VAR_NIL; v->index = 0; }

  // Conversion functions for type
    virtual int GetInt(PTVar v) { return (int)atoi(v->s); }
    virtual float GetFloat(PTVar v) { return (float)atod(v->s); }
    virtual char *GetString(PTVar v) { return v->s; }
    virtual char *DupString(PTVar v) { return strdup(v->s); } 
    virtual char *CopyString(PTVar v, char *buf, int buflen) { strncpyz(buf, v->s, buflen); return buf; }
    virtual void *GetPointer(PTVar v) { return NULL; }
    virtual PTVar GetItem(PTVar v, PTVar index) { return NULL; }

  // Set array element
    virtual void SetItem(PTVar v, PTVar item, PTVar index) { return CMD_NO_ARRAY; }

    virtual int Evaluate(OPERATOR oper, RTVar d, PTVar v1, PTVar v2 = NULL);
      // Evaluates the given operation given the operator 'oper', the destination 'd', and
      // the operands v1 and v2
};

// Evaluates the given operation given the operator 'oper', the destination 'd', and
// the operands v1 and v2
int TStringType::Evaluate(OPERATOR oper, RTVar d, PTVar v1, PTVar v2)
{
    char *s;
    int r;

    switch (oper)
    {
      case OPER_PLUS:
        s = v2->GetString();
        if (s)
            
            r = strcmp(v1->s, s);
        strcpy(v1->f);
        d.SetString(v1->f + v2->GetFloat(), );
        break;
      case OPER_LESSTHAN:
      case OPER_GREATERTHAN:
      case OPER_LESSTHANEQUAL:
      case OPER_GREATERTHANEQUAL:
      case OPER_EQUAL:
      case OPER_NOTEQUAL:
        s = v2->GetString();
        if (s)
            r = strcmp(v1->s, s);
        else
        {
            s = v2->DupString();
            r = strcmp(v1->s, s);
            free(s);
        }
        switch (oper)
        {
          case OPER_LESSTHAN:
            d.SetBool(r < 0);
            break;
          case OPER_GREATERTHAN:
            d.SetBool(r > 0);
            break;
          case OPER_LESSTHANEQUAL:
            d.SetBool(r <= 0);
            break;
          case OPER_GREATERTHANEQUAL:
            d.SetBool(r >= 0);
            break;
          case OPER_EQUAL:
            d.SetBool(r == 0);
            break;
          case OPER_NOTEQUAL:
            d.SetBool(r != 0);
            break;
        }
        break;
      case OPER_ASSIGN:
        s = v2->GetString();
        if (v1
        v1->SetString(v2->GetFloat());
        d.SetFloat(v1->f);
        break;
      default:
        return CMDERR_BAD_OPERATOR;
    }

    return CMDERR_OK;
};



_STRUCTDEF(SVStackEntry)
struct SVStackEntry
{
    int oper;
    int prec;
}

_STRUCTDEF(SOStackEntry)
struct SOStackEntry
{
    OPERATOR oper;
    VARTYPE type;
    int prec;
}

_CLASSDEF(TExpressionStack)
class TExpressionStack
{
  public:

    int Evaluate();
      // Evaluates all items currently on the stack
    int BeginParen();
      // Adds a begin parentheses to the stack
    int EndParen();
      // Adds an end parentheses to the stack

  private
    int ostackptr, vstackptr;           // Current stack pointers
    int numparen, parenprec;            // Parentheses number and current precedence level
    SOStackEntry ostack[MAXOSTACK];     // Operator stack
    TVar vstack[MAXVSTACK];             // Variable stack
};

// Start at top of stack, and evaluate all operators and operands on stack downwards.
int TExpressionStack::Evaluate()
{
    int err;
 
  // Evaluate all items currently on the stack
    while (ostackptr >= 0)
    {
        PSOStackEntry os = &ostack[ostackptr - 1];

      // If unary operation, use current operand as source and destination
        if (operators[oper[ostackptr - 1]].unary)
        {
            err = vstack[vstackptr - 1].Evaluate(os->oper, os->type, 
                vars[vstackptr - 1], NULL);
            if (err)
                return err;
        }
        else  // If binary, use second to last as dest, and reduce stack size by 1
        {
            err = vstack[vstackptr - 2].Evaluate(os->oper, os->type, 
                vars[vstackptr - 2], &vars[vstackptr - 1]);
            if (err)
                return err;
            vstackptr--;
        }
        ostackptr--;  // Reduce number of operations on stack by 1
    }

    return CMDERR_OK;
}

int TExpressionStack.BeginParen()
{
    numparen++;
    parenprec += PARENPREC;

    return CMDERR_OK; 
}

int TExpressionStack.EndParen()
{
    if (numparen <= 0)
        return CMDERR_UNMATCHED_PARENTHESES;

    numparen--;
    parenprec -= PARENPREC;
    
    return Evaluate();
}

int TExpressionStack.AddOperator(OPERATOR oper, VARTYPE type)
{
    int err;

    int prec = operators[oper].prec + parenprec;

    if (ostackptr > 0)
    {
        PSOStackEntry os = &ostack[ostackptr - 1];

        if (os->prec >= prec)
        {
            err = Evaluate();
            if (err)
                return err;
        }
    }
    
    if (ostackptr >= MAXOSTACK)
        return CMDERR_STACK_OVERFLOW;

    ostack[ostackptr].oper = oper;
    ostack[ostackptr].precedence = precedence;

    return CMDERR_OK;
}



int TScriptManager::ParseExpression(TToken &t, TExpressionStack &expstack)
{
    int topstackptr = expstackptr;

    switch (expstate)
    {
        case EXPSTATE_OPERAND:
        {
            if (t.Is("("))
            {
                err = expstack.AddParen();
                if (err)
                    return err;
                t.WhiteGet();
            }
            else if (t.Is("-"))
            {
                err = expstack.AddOperator(OPER_NEGATE);
                if (err)
                    return err;
                t.WhiteGet();
                expstate = EXPSTATE_BASICOPERAND;
            }
            else if (t.Is("NOT"))
            {
                err = expstack.AddOperator(OPER_NOT);
                if (err)
                    return err;
                t.WhiteGet();
                expstate = EXPSTATE_BASICOPERAND;
            }
            else
                expstate = EXPSTATE_BASICOPERAND;
            break;
        }
        case EXPSTATE_BASICOPERAND:
        {
            if (t.Type() == TKN_IDENT)
            {
                PTScriptVar v = GlobalVars.FindVar(t.Text());
                if (v)
                {   
                    err = expstack.AddVar(v);
                    if (err)
                        return err;
                }
                else
                    return CMDERR_UNRECOGNIZED_IDENTIFIER;
            }   
            else if (t.Type() == TKN_NUMBER)
            {
                err = expstack.AddNumber(t.Number());
            }
            else if (t.Type() == TKN_TEXT)
            {
                err = expstack.AddText(t.Text());
            }       
            else
                return CMDERR_EXPRESSION_SYNTAX;



        





    }










}
                
            








    }









}


