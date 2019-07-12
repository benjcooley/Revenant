// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  command.cpp - Command interpreter                    *
// *************************************************************************

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "revenant.h"
#include "command.h"
#include "parse.h"
#include "editor.h"
#include "object.h"
#include "character.h"
#include "editor.h"
#include "script.h"
#include "mappane.h"
#include "spell.h"
#include "dialog.h"
#include "exit.h"
#include "playscreen.h"

/* externs */
extern TConsolePane Console;
extern char buf[];

/* Prototypes for command functions */
// editor and general object manipulation
COMMAND(CmdSelect);
COMMAND(CmdDeselect);
COMMAND(CmdAdd);
COMMAND(CmdAddInv);
COMMAND(CmdGive);
COMMAND(CmdUndo);
COMMAND(CmdMove);
COMMAND(CmdPos);
COMMAND(CmdDelete);
COMMAND(CmdDelInv);
COMMAND(CmdTake);
COMMAND(CmdStat);
COMMAND(CmdScript);
COMMAND(CmdCenterOn);
COMMAND(CmdScrollTo);
COMMAND(CmdMapPos);
COMMAND(CmdShow);
COMMAND(CmdAmbient);
COMMAND(CmdAmbColor);
COMMAND(CmdDirLight);
COMMAND(CmdMono);
COMMAND(CmdBaseLight);
COMMAND(CmdReplace);
COMMAND(CmdLight);
COMMAND(CmdZOffset);
COMMAND(CmdRegistration);
COMMAND(CmdAnimRegistration);
COMMAND(CmdAnimZ);
COMMAND(CmdName);
COMMAND(CmdFlip);
COMMAND(CmdHelp);
COMMAND(CmdLock);
COMMAND(CmdUnlock);
COMMAND(CmdImmobile);
COMMAND(CmdUnimmobile);
COMMAND(CmdLoad);
COMMAND(CmdSave);
COMMAND(CmdSaveTileBM);
COMMAND(CmdDXStats);
COMMAND(CmdMemory);
COMMAND(CmdGenerate);
COMMAND(CmdCalcWalkmap);
COMMAND(CmdWalkmap);
COMMAND(CmdState);
COMMAND(CmdTry);
COMMAND(CmdForce);
COMMAND(CmdReveal);
COMMAND(CmdLevel);
COMMAND(CmdTemplate);
COMMAND(CmdTileWalkmap);
COMMAND(CmdSmoothScroll);
COMMAND(CmdDLight);
COMMAND(CmdAddRC);
COMMAND(CmdDeleteRC);
COMMAND(CmdExit);
COMMAND(CmdResetScreenRect);
COMMAND(CmdBounds);
COMMAND(CmdToggle);
COMMAND(CmdGet);
COMMAND(CmdSectorCommand);
COMMAND(CmdSwap);
COMMAND(CmdToFront);
COMMAND(CmdToBack);
COMMAND(CmdGroup);
COMMAND(CmdPlay);
COMMAND(CmdTrigger);
COMMAND(CmdNewGame);
COMMAND(CmdCurPlayer);
COMMAND(CmdGetState);

// object-specific commands
COMMAND(CmdText);
COMMAND(CmdFollow);
COMMAND(CmdRestore);


// in-game control commands (for scripts)
COMMAND(CmdSay);
COMMAND(CmdGo);
COMMAND(CmdGoto);
COMMAND(CmdFace);
COMMAND(CmdPivot);
COMMAND(CmdUse);
COMMAND(CmdActivate);
COMMAND(CmdCombat);
COMMAND(CmdAttack);
COMMAND(CmdInvoke);
COMMAND(CmdStop);
COMMAND(CmdBlock);
COMMAND(CmdControl);
COMMAND(CmdPulp);
COMMAND(CmdBurn);
COMMAND(CmdSetVisibility);

// character dialog commands
COMMAND(CmdChoice);

// script conditionals, loops, gamestate control
COMMAND(CmdBegin);
COMMAND(CmdEnd);
COMMAND(CmdIf);
COMMAND(CmdElse);
COMMAND(CmdWhile);
COMMAND(CmdSet);
COMMAND(CmdWait);
COMMAND(CmdJump);

// Master command list, evaluated top-to-bottom
// --------------------------------------------
//
// "name", function, class1 (0 all, -1 none), class2, requiresparams, editoronly, "usage"

SCommand Commands[] =
{ { "select", CmdSelect, -1, -1, TRUE, TRUE, "usage: select [#.]<object name>\n"
                                   "       select <#/next/prev>\n" },
  { "deselect", CmdDeselect, -1, -1, FALSE, TRUE, "usage: deselect [<selection number>]\n" },
  { "add", CmdAdd, -1, -1, TRUE, FALSE, 
            "usage: add [<amt>] <typename>\n"
            "       add light [<intensity>]\n"
            "       add <class> stat <statname> [<default> <min> <max>]\n" },
  { "addinv", CmdAddInv, 0, 0, TRUE, FALSE, "usage: <object>.addinv [<amt>] <obj>\n" },
  { "give", CmdGive, 0, 0, TRUE, FALSE, "usage: <object>.give <to> [<amt>] <obj>\n" },
  { "undo", CmdUndo, -1, -1, FALSE, TRUE, "usage: undo\n" },
  { "move", CmdMove, 0, 0, TRUE, FALSE, "usage: <object>.move <dx> <dy> [<dz>]\n" },
  { "pos", CmdPos, 0, 0, TRUE, FALSE, "usage: <object>.pos <dx> <dy> [<dz> [<level>]]\n" },
  { "delete", CmdDelete, 0, 0, FALSE, FALSE, "usage: <object>.delete\n" },
  { "delinv", CmdDelInv, 0, 0, FALSE, FALSE, "usage: <object>.delinv [<amt>] <obj>\n" },
  { "take", CmdTake, 0, 0, TRUE, FALSE, "usage: <object>.take <from> [<amt>] <obj>\n" },
  { "stat", CmdStat, 0, 0, FALSE, FALSE, "usage: <object>.stat [<amt>]<name> <value>]\n" },
  { "script", CmdScript, 0, 0, FALSE, TRUE, "usage: <object>.script\n" },
  { "centeron", CmdCenterOn, 0, 0, FALSE, FALSE, "usage: <object>.centeron OR\n\t centeron <object> OR\n\t centeron <x> <y> <z> [<level>]\n" },
  { "scrollto", CmdScrollTo, 0, 0, FALSE, FALSE, "usage: <object>.scrollto OR\n\t scrollto <object> OR\n\t scrollto <x> <y> <z> [<level>]\n" },
  { "mappos",   CmdMapPos, -1, -1, TRUE, TRUE, "usage: mappos list [or] <name> [or] <x> <y> <z>\n" },
  { "show", CmdShow, -1, -1, TRUE, FALSE, "usage: show <class> [<type>]\n" },
  { "ambient", CmdAmbient, -1, -1, TRUE, FALSE, "usage: ambient <intensity>\n" },
  { "ambcolor", CmdAmbColor, -1, -1, TRUE, FALSE, "usage: ambcolor <red> <green> <blue>\n" },
  { "mono", CmdMono, -1, -1, TRUE, FALSE, "usage: mono <percent>\n" },
  { "dirlight", CmdDirLight, -1, -1, TRUE, FALSE, "usage: dirlight <intensity> [<hnormal> <vnormal>]\n" },
  { "replace", CmdReplace, 0, 0, TRUE, FALSE, "usage: <object>.replace [<class>] [<newobjtype>]\n" },
  { "light", CmdLight, 0, 0, TRUE, FALSE, "usage: <object>.light <on/off>    <object>.light <intensity>\n"
                "       <object>.light directional <on/off>\n"
                "       <object>.light color <red> <green> <blue>\n"
                "       <object>.light position <x> <y> <z>\n" },
  { "zoffset", CmdZOffset, 0, 0, FALSE, TRUE, "usage: <object>.zoffset <zoffset>\n" },
  { "registration", CmdRegistration, 0, 0, FALSE, TRUE, "usage: <object>.registration <deltax> <deltay>\n" },
  { "animregistration", CmdAnimRegistration, 0, 0, FALSE, TRUE, "usage: <object>.animregistration <deltax> <deltay>\n" },
  { "animz", CmdAnimZ, 0, 0, FALSE, TRUE, "usage: <object>.animz <zval>\n" },
  { "name", CmdName, 0, 0, TRUE, FALSE, "usage: <object>.name [type/clear] <name>\n" },
  { "flip", CmdFlip, 0, 0, FALSE, FALSE, "usage: <object>.flip\n" },
  { "help", CmdHelp, -1, -1, FALSE, TRUE, "usage: help [<command>]\n" },
  { "lock", CmdLock, 0, 0, FALSE, TRUE, "usage: lock\n" },
  { "unlock", CmdUnlock, 0, 0, FALSE, TRUE, "usage: unlock\n" },
  { "immobile", CmdImmobile, 0, 0, FALSE, TRUE, "usage: immobile\n" },
  { "unimmobile", CmdUnimmobile, 0, 0, FALSE, TRUE, "usage: unimmobile\n" },
  { "load", CmdLoad, -1, -1, FALSE, TRUE, "usage: load\n" },
  { "save", CmdSave, -1, -1, FALSE, TRUE, "usage: save [game | map | headers | classes | exits]\n" },
  { "savetilebm", CmdSaveTileBM, -1, -1, FALSE, TRUE, "usage: savetilebm\n" },
  { "dxstats", CmdDXStats, -1, -1, FALSE, FALSE, "usage: dxstats\n" },
  { "memory", CmdMemory, -1, -1, FALSE, FALSE, "usage: memory\n" },
  { "generate", CmdGenerate, -1, -1, FALSE, TRUE, "usage: generate [<sizex> <sizey>] [from <startx> <starty>]\n" },
  { "calcwalkmap", CmdCalcWalkmap, -1, -1, FALSE, TRUE, "usage: calcwalkmap\n" },
  { "walkmap", CmdWalkmap, 0, 0, TRUE, TRUE, "usage: walkmap <delta z>\n" },
  { "tilewalkmap", CmdTileWalkmap, OBJCLASS_TILE, -1, FALSE, TRUE, "usage: tilewalkmap\n" },
  { "state", CmdState, 0, -1, TRUE, FALSE, "usage: state <state number>\n" },
  { "try", CmdTry, 0, -1, TRUE, FALSE, "usage: try <state name>\n" },
  { "force", CmdForce, 0, -1, TRUE, FALSE, "usage: force <state name>\n" },
  { "reveal", CmdReveal, 0, -1, FALSE, FALSE, "usage: reveal\n" },
  { "level", CmdLevel, -1, -1, TRUE, FALSE, "usage: level <level number>\n" },
  { "template", CmdTemplate, 0, -1, FALSE, TRUE, "usage: <object.>template\n" },
  { "smoothscroll", CmdSmoothScroll, -1, -1, TRUE, FALSE, "usage: smoothscroll <on/off>\n" },
  { "dlight", CmdDLight, -1, -1, TRUE, FALSE, "usage: dlight <intensity>\n" },
  { "addrc", CmdAddRC, -1, -1, FALSE, TRUE, "usage: addrc\n" },
  { "deleterc", CmdDeleteRC, -1, -1, FALSE, TRUE, "usage: deleterc\n" },
  { "exit", CmdExit, OBJCLASS_EXIT, -1, TRUE, TRUE, "usage: <object>.exit <name> [posonly]\n" },
  { "resetscreenrect", CmdResetScreenRect, 0, 0, FALSE, TRUE, "usage: <object>.resetscreenrect [<state>]\n" },
  { "bounds", CmdBounds, 0, 0, TRUE, TRUE, "usage: <object>.bounds <regx> <regy> <width> <length>\n" },
  { "toggle", CmdToggle, 0, 0, TRUE, FALSE, "usage: <object>.toggle <flagname>\n" },
  { "get", CmdGet, 0, 0, FALSE, FALSE, "usage: <character>.get <object>\n" },
  { "sectorcommand", CmdSectorCommand, -1, -1, TRUE, TRUE, "usage: sectorcommand <level> <command>\n" },
  { "swap", CmdSwap, 0, 0, TRUE, TRUE, "usage: swap <obj to swap with>\n" },
  { "tofront", CmdToFront, 0, 0, FALSE, TRUE, "usage: tofront\n" },
  { "toback", CmdToBack, 0, 0, FALSE, TRUE, "usage: toback\n" },
  { "group", CmdGroup, 0, 0, FALSE, TRUE, "usage: group <groupnum>\n"},
  { "play", CmdPlay, 0, 0, TRUE, FALSE, "usage: play <sound>\n" },
  { "trigger", CmdTrigger, 0, 0, TRUE, FALSE, "usage: trigger <trigname>\n" },
  { "newgame", CmdNewGame, -1, -1, FALSE, FALSE, "usage: newgame\n" },
  { "curplayer", CmdCurPlayer, OBJCLASS_PLAYER, -1, FALSE, FALSE, "usage: <player>.curplayer\n" },

  { "text", CmdText, OBJCLASS_SCROLL, -1, FALSE, TRUE, "usage: <scroll>.text\n" },
  { "follow", CmdFollow, OBJCLASS_EXIT, -1, FALSE, TRUE, "usage: <exit>.follow\n" },
  { "restore", CmdRestore, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, FALSE, FALSE, "usage: <character>.restore\n" },
  
  { "say", CmdSay, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <character>.say [<nowait>] [choice|\"string\"]\n" },
  { "go", CmdGo, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <character>.go <angle>\n" },
  { "goto", CmdGoto, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <character>.goto <x> <y>\n" },
  { "face", CmdFace, 0, 0, TRUE, FALSE, "usage: <object>.face <angle>\n" },
  { "pivot", CmdPivot, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <object>.pivot <angle>\n" },
  { "use", CmdUse, 0, 0, FALSE, FALSE, "usage: <object>.use [<with object>]\n" },
  { "activate", CmdActivate, OBJCLASS_EXIT, 0, FALSE, FALSE, "usage: <object>.activate\n" },
  { "combat", CmdCombat, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <object>.combat [off|on|<target>]\n" },
  { "attack", CmdAttack, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <object>.attack <target>\n" },
  { "invoke", CmdInvoke, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <object>.invoke <spellname>\n" },
  { "stop", CmdStop, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, FALSE, FALSE, "usage: <object>.stop\n" },
  { "block", CmdBlock, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, FALSE, FALSE, "usage: <object>.block\n" },
  { "control", CmdControl, -1, -1, FALSE, FALSE, "usage: control <on|off>\n" },

  { "choice", CmdChoice, -1, -1, TRUE, FALSE, "usage: choice <label> <text string>\n" },

  { "begin", CmdBegin, -1, -1, FALSE, FALSE, "usage: begin\n         <block>\n       end\n" },
  { "end", CmdEnd, -1, -1, FALSE, FALSE, "usage: begin\n         <block>\n       end\n" },
  { "if", CmdIf, -1, -1, TRUE, FALSE, "usage: if <condition>\n          <command block>\n"
                                  "         else <alternate command block>\n" },
  { "else", CmdElse, -1, -1, FALSE, FALSE, "usage: if <condition>\n          <command block>\n"
                                  "         else <alternate command block>\n" },
  { "while", CmdWhile, -1, -1, TRUE, FALSE, "usage: while <condition>\n         <command block>\n" },
  { "set", CmdSet, -1, -1, TRUE, FALSE, "usage: set <state name> <new value>\n" },
  { "wait", CmdWait, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, FALSE, FALSE, "usage: <character>.wait [<wait type>]\n" },
  { "jump", CmdJump, -1, -1, TRUE, FALSE, "usage: jump <label>\n" },
  { "pulp", CmdPulp, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <character>.pulp <x> <y> <z>\n" },
  { "burn", CmdBurn, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <character>.burn\n" },
  { "getstate", CmdGetState, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, FALSE, FALSE, "usage: <character>.getstate\n" },
  { "visible", CmdSetVisibility, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <character>.visible <state #>\n" },

  { NULL, NULL }            // list terminator
};


inline BOOL CheckOneContext(PTObjectInstance context, int classid)
{
    if (classid >= 0)
    {
        if (context == NULL)
            return FALSE;
    }
    else
    {
        if (context != NULL)
            return FALSE;
    }

    if (classid > 0 && context && context->ObjClass() != classid)
        return FALSE;

    return TRUE;
}

inline BOOL CheckContext(PTObjectInstance context, int classid, int classid2)
{
    if (classid < 0 && classid2 < 0)
        return TRUE;

    return (CheckOneContext(context, classid) || (classid2 >= 0 && CheckOneContext(context, classid2)));
}

int CommandInterpreter(PTObjectInstance context, TToken &t, int abrevlen)
{
    BOOL skip = FALSE;
    BOOL nowait = FALSE;

    PTObjectInstance orgcontext = context; // Save original context

    t.SkipBlanks();
    if (t.Type() == TKN_EOF)
        return 0;               // don't bother if there's nothing there

    strcpy(buf, t.Text());

    int groupnum = -1; // Set to valid group number if we're doing groups

    SetDialogContext(context); // Who's script is running?

    // check for <context>.<command> syntax
    if (t.Type() == TKN_IDENT)
    {
        if (t.Is("nowait"))
        {
            nowait = TRUE;
            t.WhiteGet();
        }

        t.Get();
        if (t.Is(".")) // Houston we have a context...
        {
            if (!strnicmp(buf, "group", 5))
            {
                groupnum = atoi(buf + 5);
                t.Get();
                if (t.Type() == TKN_IDENT)
                    strcpy(buf, t.Text());
                else
                {
                    Output("Specify command following group context\n");
                    return CMD_BADPARAMS;
                }
            }
            else
            {               
                PTObjectInstance inst = MapPane.FindClosestObject(buf);
                if (inst)
                {
                    context = inst;
                    t.Get();
                    if (t.Type() == TKN_IDENT)
                        strcpy(buf, t.Text());
                    else
                    {
                        Output("Specify command following object context\n");
                        return CMD_BADPARAMS;
                    }
                }
                else
                {
                    sprintf(buf, "%s: Context not found\n", buf);
                    Output(buf);
                    return 0;
                }
            }
        }
    }

    int retval = CMD_BADCOMMAND;

    for (int cmd = 0; Commands[cmd].name; cmd++)
    {
        if ((abrevlen && abbrevcmp(buf, Commands[cmd].name) >= abrevlen) ||
            !stricmp(buf, Commands[cmd].name))
        {
            // check editor-only commands
            if (Commands[cmd].editoronly && !Editor)
            {
                Output("Command can only be used in the editor\n");
                skip = TRUE;
            }

            // validate object context (we do it later if doing a command for a group though)
            if (groupnum < 0 && !CheckContext(context, Commands[cmd].classcontext, Commands[cmd].classcontext2))
            {
                skip = TRUE;

                if (context == NULL)
                    Output("Object context required for command\n");
                else
                    Output("Command not availible for context's class\n");
            }

            // Save start of command for group stuff later      
            DWORD startpos = t.GetPos(); // Go through map, and do command for all objs which fit group#

            // check params
            if (t.Type() != TKN_RETURN)
                t.WhiteGet();
            if (Commands[cmd].requiresparams && (t.Type() == TKN_RETURN || t.Type() == TKN_EOF))
            {
                Output(Commands[cmd].usage);
                skip = TRUE;
            }

            // execute the command
            if (skip)
                t.SkipLine();           // Skip the command if 'skip' is true
            else
            {

              // Group command
                if (groupnum >= 0)      // If context is group, do command for all objects in group
                {
                    for (TMapIterator i; i; i++)
                    {
                        if (i.Item()->GetGroup() == groupnum)
                        {
                            t.SetPos(startpos);
                            t.WhiteGet();

                            context = i.Item();

                          // Make sure this object is correct class for command 
                            if (CheckContext(context, Commands[cmd].classcontext, Commands[cmd].classcontext2))
                            {
                                retval = (*(Commands[cmd].cmdfunc))(context, t); // Do it
                                if (retval & CMD_ERROR)
                                    break;
                            }
                        }
                    }
                }
                else                    // Do command for current context
                    retval = (*(Commands[cmd].cmdfunc))(context, t);
            }

            break;
        }
    }

  // Command wasn't on list, try current context
    if (retval == CMD_BADCOMMAND && context)
    {
        retval = context->ParseCommand(t);
    }

    if (retval & CMD_BADPARAMS)
        Output("Bad parameters.\n");
    if (retval & CMD_OUTOFMEM)
        Output("Couldn't complete command due to low memory.\n");
    if (retval & (CMD_USAGE | CMD_BADPARAMS))
        Output(Commands[cmd].usage);
    if (retval & CMD_BADCOMMAND)
        Output("Unrecognized command.");
    else if (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
    {
        if (!(retval & CMD_ERROR))
            Output("(extra parameters ignored)\n");
        while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
            t.Get();
    }

    if (nowait && retval == CMD_WAIT)
        retval = 0;

  // If we're  character, and our script just caused another character to do something, 
  // wait for that character to finish what he's doing
    if (orgcontext && context && // Both not null
        orgcontext != context && // Doing something to somebody other than ourselves
        retval == CMD_WAIT &&    // Script caused a wait (and we are both chars below)
        (context->ObjClass() == OBJCLASS_CHARACTER || context->ObjClass() == OBJCLASS_PLAYER) &&
        (orgcontext->ObjClass() == OBJCLASS_CHARACTER || orgcontext->ObjClass() == OBJCLASS_PLAYER))
    {
        ((PTCharacter)orgcontext)->WaitChar(context);
    }

    return retval;
}

// Defined in editor.cpp
static char buf[1024]; // Temporary buf for output

void Output(char *fmt,...)
{
    va_list marker;
    va_start(marker, fmt);
    vsprintf(buf, fmt, marker);

    if (Editor && !Console.IsHidden())
        Console.Output(buf);
}

COMMAND(CmdHelp)
{
    if (t.Type() == TKN_IDENT)
    {
        for (int cmd = 0; Commands[cmd].name; cmd++)
            if (t.Is(Commands[cmd].name, MINCMDABREV))
            {
                Output(Commands[cmd].usage);
                break;
            }

        if (Commands[cmd].name == NULL)
            Output("Command not found.\n");

        t.WhiteGet();
        return 0;
    }

    Output("The following commands are currently availible:\n");

    int cnt = 0;
    char buf[256] = "";

    for (int cmd = 0; Commands[cmd].name; cmd++)
    {
        sprintf(buf, "%s %-10s", buf, Commands[cmd].name);
        if (++cnt >= 5)
        {
            strcat(buf, "\n");
            Output(buf);
            buf[0] = 0;
            cnt = 0;
        }
    }

    if (cnt)
    {
        strcat(buf, "\n");
        Output(buf);
    }

    return 0;
}

// *******************
// * Alias Functions *
// *******************

/*_STRUCTDEF(SAliasTag)
struct SAliasTag
{
    char name[MAXCONTEXTNAME];  // Name of this alias
    char alias[MAXCONTEXTNAME]; // What we are aliasing
};

typedef TVirtualArray<SAliasTag, 16, 16> TAliasArray;
static TAliasArray AliasArray;

int NumAliases()
{
    return AliasArray.NumItems();
}

void ClearAliases()
{
    AliasArray.Clear();
}

char *GetAliasName(int aliasnum)
{
    return AliasArray[aliasnum].name;
}

char *GetAlias(int aliasnum)
{
    return AliasArray[aliasnum].alias;
}

int GetAliasNum(char *aliasname)
{
    for (int c = 0; c < NumAliases(); c++)
    {
        if (!stricmp(AliasArray[c].name, aliasname))
            return c;
    }
    return -1;
}

char *GetAlias(char *aliasname)
{
    int aliasnum = GetAliasNum(aliasname);
    if (aliasnum < 0)
        return NULL;

    return GetAlias(aliasnum);
}

int AddAlias(char *aliasname, char *alias)
{
    SAliasTag a;
    memset(a, 0, sizeof(SAliasTag));
    strcpy(a.name, aliasname);
    strcpy(a.alias, alias);
    int aliasnum = GetAliasNum(aliasname);
    if (aliasnum < 0)
        aliasnum = NumAliases();
    return AliasArray.Set(a, aliasnum);
}
*/
// *******************
// * Object Commands *
// *******************

COMMAND(CmdUse)
{
    PTObjectInstance inst = NULL;
    PTObjectInstance with = NULL;

    if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
    {
        inst = MapPane.FindClosestObject(t.Text(), context);
        t.WhiteGet();
    }

    if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
    {
        with = MapPane.FindClosestObject(t.Text(), context);
        t.WhiteGet();
    }

    if (context && inst)
    {
        if (with)
            inst->Use(context, with->GetMapIndex());
        else
            inst->Use(context, -1);
    }
    else
        context->Use(context, -1);

    return 0;
}

COMMAND(CmdActivate)
{
    ((PTExit)context)->Activate();

    return 0;
}

COMMAND(CmdSay)
{
    char sayanim[20];
    sayanim[0] = NULL;

    BOOL nowait = FALSE;
    if (t.Is("nowait"))
    {
        nowait = TRUE;
        t.WhiteGet();
    }

    int wait = -1;
    if (t.Type() == TKN_NUMBER)
    {
        wait = t.Index();
        t.WhiteGet();
    }

    buf[0] = '\"';

    if (t.Is("choice"))
    {
        if (DialogPane.GetResponseText())
            strcpy(buf + 1, DialogPane.GetResponseText());
        else
            buf[1] = NULL;
        t.WhiteGet();
    }
    else if (t.Type() == TKN_IDENT)
    {
        strcpy(sayanim, t.Text());
        t.WhiteGet();

        if (!Parse(t, "%s", buf+1))
            return CMD_BADPARAMS;
        strcat(buf, "\"");
    }
    else
    {
        if (!Parse(t, "%s", buf+1))
            return CMD_BADPARAMS;
        strcat(buf, "\"");
    }

    if (sayanim[0] != NULL)
        ((PTCharacter)context)->Say(buf, wait, sayanim);
    else
        ((PTCharacter)context)->Say(buf, wait, NULL);

    if (nowait)
        return 0;

    return CMD_WAIT;
}

COMMAND(CmdGo)
{
    int angle;
    if (!Parse(t, "%d", &angle))
        return CMD_BADPARAMS;

    ((PTCharacter)context)->Go(angle);

    return 0;
}

COMMAND(CmdGoto)
{
    int x, y;
    if (!Parse(t, "%d %d", &x, &y))
        return CMD_BADPARAMS;

    ((PTCharacter)context)->Goto(x, y);

    return CMD_WAIT;
}

COMMAND(CmdFace)
{
    int angle;
    if (!Parse(t, "%d", &angle))
        return CMD_BADPARAMS;

    if (context)
        context->Face(angle);

    return CMD_WAIT;
}

COMMAND(CmdPivot)
{
    int angle;
    if (!Parse(t, "%d", &angle))
        return CMD_BADPARAMS;

    if (context)
        ((PTCharacter)context)->Pivot(angle);

    return CMD_WAIT;
}

COMMAND(CmdCombat)
{
    int index = -1;

    if (t.Type() == TKN_IDENT)
    {
        if (t.Is("off"))
            ((PTCharacter)context)->EndCombat();
        else
        {
            PTObjectInstance inst = NULL;
            if (!t.Is("on"))
            {
                inst = MapPane.FindClosestObject(t.Text(), context);
                if (!inst)
                    return CMD_BADPARAMS;
            }
            ((PTCharacter)context)->BeginCombat((PTCharacter)inst);
        }
        t.WhiteGet();
    }

    return 0;
}

COMMAND(CmdAttack)
{
    int index = -1;

    if (t.Type() == TKN_IDENT)
    {
        PTObjectInstance inst = MapPane.FindClosestObject(t.Text(), context);
        t.WhiteGet();
    }

    return 0;
}

COMMAND(CmdBlock)
{
    ((PTCharacter)context)->Block();
    return CMD_WAIT;
}

COMMAND(CmdControl)
{
    if (t.Type() != TKN_IDENT)
        return CMD_BADPARAMS;

    if ((PTPlayer)context == Player)
    {
        if (t.Is("on"))
            PlayScreen.SetDemoMode(FALSE);
        else if (t.Is("off"))
            PlayScreen.SetDemoMode(TRUE);
        else
            return CMD_BADPARAMS;
    }
    else
    {
        if (t.Is("on"))
            PlayScreen.SetControlOn(TRUE);
        else if (t.Is("off"))
            PlayScreen.SetControlOn(FALSE);
        else
            return CMD_BADPARAMS;
    }

    t.WhiteGet();

    return 0;
}

COMMAND(CmdInvoke)
{
    extern int spellvals[];
    extern char *spellnames[];

    if (t.Type() != TKN_IDENT)
        return CMD_BADPARAMS;

/*
    for (int i = 0; i < NUM_SPELLS; i++)
        if (t.Is(spellnames[i]))
            break;

    t.WhiteGet();

    if (i >= NUM_SPELLS)
        return CMD_BADPARAMS;

    ((PTCharacter)context)->Invoke(&(spellvals[i]));
*/
    return CMD_WAIT;
}

COMMAND(CmdStop)
{
    ((PTCharacter)context)->Stop();
    return CMD_WAIT;
}

// ******************************
// * Script Language Components *
// ******************************

char *Operators[] = { "=", "<>", ">", "<", ">=", "<=", "and", "or", "not", "+", "-", "*", "/", NULL };

int ApplyOperation(int &lval, int op, int rval)
{
    int newval = 0;

    switch (op)
    {
        case 0: newval = (lval == rval); break;
        case 1: newval = (lval != rval); break;
        case 2: newval = (lval > rval); break;
        case 3: newval = (lval < rval); break;
        case 4: newval = (lval >= rval); break;
        case 5: newval = (lval <= rval); break;
        case 6: newval = (lval && rval); break;
        case 7: newval = (lval || rval); break;
        case 8: newval = !rval; break;              // boolean not is a special case
        case 9: newval = (lval + rval); break;
        case 10: newval = (lval - rval); break;
        case 11: newval = (lval * rval); break;
        case 12: if (rval == 0) newval = 0; else newval = (lval / rval); break;
    }

    // adjust for boolean or arithmatic operations
    if (op <= 5)
        lval = rval;
    else
        lval = newval;

    return newval;
}

int FindElement(char *elem, char *list[])
{
    for (int i = 0; list[i] && i < 64; i++)
        if (stricmp(elem, list[i]) == 0)
            return i;

    return -1;
}

// Finds a (semi-)unique value for a given string for expression parsing
int StringVal(char *string)
{
    int val = 0;

    for (int i = 0; string[i]; i++)
        val |= (int)(string[i] - 'A') << i;

    return val;
}

BOOL ParseExpression(TToken &t, int *value)
{
    int totalval = STATE_INVALID;
    int lval = STATE_INVALID;
    int optype = -1;
    char buf[60];

    while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
    {
        if (t.Type() == TKN_SYMBOL)
        {
            if (optype != -1)
                return FALSE;

            strcpy(buf, t.Text());      // First char of operator
            t.Get();
            if (t.Type() == TKN_SYMBOL)
                strcat(buf, t.Text());  // Add second char to operator

            int ot = FindElement(buf, Operators);
            if (ot >= 0 && (lval != STATE_INVALID || ot == 8))  // boolean "not" is 7
                optype = ot;
            else
                return FALSE;

            t.WhiteGet();
        }
        else if (t.Type() == TKN_IDENT || t.Type() == TKN_NUMBER || t.Type() == TKN_TEXT)
        {
            int rval;
            BOOL isop = FALSE;

            if (t.Type() == TKN_IDENT)
            {
                int ot = FindElement(t.Text(), Operators);
                if (ot >= 0) // and, or, not...
                {
                    if (lval != STATE_INVALID || ot == 8)
                        optype = ot;
                    else
                        return FALSE;

                    isop = TRUE;

                    t.WhiteGet();
                }
                else         // Game id
                {
                    strcpy(buf, t.Text());
                    t.Get();
                    if (t.Is("."))
                    {
                        t.Get();
                        if (t.Type() != TKN_IDENT)
                            return FALSE;

                        PTObjectInstance inst = MapPane.FindClosestObject(buf);;
                        if (inst)
                        {
                            if (t.Is("state"))
                                rval = inst->GetState();
                            else
                                rval = inst->GetStat(t.Text());
                        }
                        t.WhiteGet();
                    }
                    else
                        rval = ScriptManager.GameState(buf);
                    if (t.Type() == TKN_WHITESPACE)
                        t.Get();
                }
            }
            else if (t.Type() == TKN_TEXT)
            {
                rval = StringVal(t.Text());
                t.WhiteGet();
            }
            else
            {
                rval = t.Index();
                t.WhiteGet();
            }

            if (!isop)
            {
                if (rval == STATE_INVALID)
                    return FALSE;

                if (lval == STATE_INVALID)
                {
                    if (optype == 7)        // boolean not
                        totalval = ApplyOperation(lval, optype, rval);
                    else
                        totalval = lval = rval;
                }
                else
                {
                    if (optype == -1)
                        return FALSE;
                    else
                    {
                        totalval = ApplyOperation(lval, optype, rval);
                        optype = -1;
                    }
                }
            }
        }
    }

    if (totalval == STATE_INVALID)
        return FALSE;

    *value = totalval;
    return TRUE;
}

COMMAND(CmdBegin)
{
    return CMD_BEGIN;
}

COMMAND(CmdEnd)
{
    return CMD_END;
}

COMMAND(CmdIf)
{
    int cond;
    if (!ParseExpression(t, &cond))
        return CMD_BADPARAMS;

    return (cond ? CMD_CONDTRUE : CMD_CONDFALSE);
}

COMMAND(CmdElse)
{
    // The hideously complex algorithm contained in this function
    // is enough to drive a programmer insane, so don't bother
    // trying to comprehend its greatness.  Just accept the fact
    // that it works.

    return CMD_ELSE;
}

COMMAND(CmdWhile)
{
    int cond;
    if (!ParseExpression(t, &cond))
        return CMD_BADPARAMS;

    return cond ? CMD_LOOP : CMD_SKIPBLOCK;
}

COMMAND(CmdSet)
{
    char buf[40];

    if (t.Type() != TKN_IDENT)
        return CMD_BADPARAMS;

    strcpy(buf, t.Text());
    t.WhiteGet();

    if (t.Type() == TKN_RETURN || t.Type() == TKN_EOF)
    {
        Output("%s = %d\n", buf,    ScriptManager.GameState(buf));
        return 0;
    }
        
    if (t.Is("="))
        t.WhiteGet();

    int value;
    if (t.Is("on") || t.Is("true"))
    {
        value = 1;
        t.WhiteGet();
    }
    else if (t.Is("off") || t.Is("false"))
    {
        value = 0;
        t.WhiteGet();
    }
    else if (t.Type() == TKN_NUMBER)
        value = t.Index();
    else
        return CMD_BADPARAMS;

    ScriptManager.SetGameState(buf, value);

    return 0;
}

COMMAND(CmdWait)
{
    if (t.Type() == TKN_NUMBER)
    {
        int wait = t.Index();

        ((PTCharacter)context)->Wait(wait);

        t.WhiteGet();
    }
    else if (t.Is("response"))
    {
        DialogPane.SetCharacter((PTCharacter)context);  // Waiting for dialog.. show dialog pane
        DialogPane.Show();

        ((PTCharacter)context)->WaitResponse();

        t.WhiteGet();
    }
    else if (t.Is("char"))
    {
        t.WhiteGet();
        if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
            return CMD_BADPARAMS;

        PTObjectInstance inst = MapPane.FindClosestObject(t.Text(), context);
        if (!inst)
            Output("Couldn't find wait char\n");

        ((PTCharacter)context)->WaitChar(inst);

        t.WhiteGet();
    }

    return CMD_WAIT;
}

COMMAND(CmdJump)
{
    if (t.Type() != TKN_IDENT && t.Type() != TKN_KEYWORD)
        return CMD_BADPARAMS;

    if (context)
        context->ScriptJump(t.Text());

    return CMD_JUMP;
}

// pulp a character
COMMAND(CmdPulp)
{
    S3DPoint vel;
    if(!Parse(t, "%d %d %d", &vel.x, &vel.y, &vel.z))
        return CMD_BADPARAMS;

    int x = random(6, 16);

    ((PTCharacter)context)->Pulp(vel, x, x * 30);
    
    return 0;
}

// burn a character
COMMAND(CmdBurn)
{
    ((PTCharacter)context)->Burn();
    
    return 0;
}

// return state info
COMMAND(CmdGetState)
{
    char buf[1024];
    sprintf(buf, "state = \"%s\"", ((PTCharacter)context)->GetState());
    if(((PTCharacter)context)->IsInRoot())
        strcat(buf, " <root>");
    Output(buf);

    return 0;
}

COMMAND(CmdSetVisibility)
{
    int state;
    if(!Parse(t, "%d", &state))
        return CMD_BADPARAMS;

    if(!state)
        ((PTCharacter)context)->MakeInvisible();
    else
        ((PTCharacter)context)->MakeVisible();

    return 0;
}
