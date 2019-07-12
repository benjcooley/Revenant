// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  command.cpp - Command interpreter                    *
// *************************************************************************

#include <windows.h>

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "revenant.h"
#include "3dscene.h"
#include "directdraw.h"
#include "mappane.h"
#include "object.h"
#include "tile.h"
#include "editor.h"
#include "display.h"
#include "font.h"
#include "dls.h"
#include "chunkcache.h"
#include "playscreen.h"
#include "inventory.h"
#include "multictrl.h"
#include "command.h"
#include "cursor.h"
#include "script.h"
#include "multi.h"
#include "savegame.h"
#include "player.h"
#include "template.h"
#include "scroll.h"
#include "exit.h"
#include "textbar.h"
#include "statusbar.h"
#include "spell.h"
#include "file.h"
#include "3dimage.h"
#include "sound.h"
#include "dialog.h"
#include "effect.h"

/* externs */
extern TObjectClass TileClass;
extern TObjectClass HelperClass;
extern TConsolePane Console;
extern BOOL BlitHardware;
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
COMMAND(CmdRotate);
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
COMMAND(CmdLoad);
COMMAND(CmdSave);
COMMAND(CmdSaveTileBM);
COMMAND(CmdDXStats);
COMMAND(CmdMemory);
COMMAND(CmdGenerate);
COMMAND(CmdCalcWalkmap);
COMMAND(CmdWalkmap);
COMMAND(CmdState);
COMMAND(CmdFrame);
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
COMMAND(CmdExtents);
COMMAND(CmdBounds);
COMMAND(CmdToggle);
COMMAND(CmdGet);
COMMAND(CmdSectorCommand);
COMMAND(CmdSwap);
COMMAND(CmdToFront);
COMMAND(CmdToBack);
COMMAND(CmdGroup);
COMMAND(CmdPlay);
COMMAND(CmdPlay3D);
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
COMMAND(CmdStop);
COMMAND(CmdBlock);
COMMAND(CmdControl);
COMMAND(CmdPulp);
COMMAND(CmdBurn);
COMMAND(CmdDrip);
COMMAND(CmdSetVisibility);
COMMAND(CmdCast);
COMMAND(CmdKnockBack);

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
{ { "activate", CmdActivate, OBJCLASS_EXIT, 0, FALSE, FALSE, "usage: <object>.activate\n" },
  { "add", CmdAdd, -1, -1, TRUE, FALSE, "usage: add [<amt>] <typename>\n       add light [<intensity>]\n" },
  { "addinv", CmdAddInv, 0, 0, TRUE, FALSE, "usage: <object>.addinv [<amt>] <obj>\n" },
  { "addrc", CmdAddRC, -1, -1, FALSE, TRUE, "usage: addrc\n" },
  { "amb", CmdAmbient, -1, -1, TRUE, FALSE, "usage: ambient <intensity>\n" },
  { "ambcolor", CmdAmbColor, -1, -1, TRUE, FALSE, "usage: ambcolor <red> <green> <blue>\n" },
  { "animreg", CmdAnimRegistration, 0, 0, FALSE, TRUE, "usage: <object>.animregistration <deltax> <deltay>\n" },
  { "animz", CmdAnimZ, 0, 0, FALSE, TRUE, "usage: <object>.animz <zval>\n" },
  { "attack", CmdAttack, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <object>.attack <target>\n" },
  { "begin", CmdBegin, -1, -1, FALSE, FALSE, "usage: begin\n         <block>\n       end\n" },
  { "block", CmdBlock, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, FALSE, FALSE, "usage: <object>.block\n" },
  { "bounds", CmdBounds, 0, 0, TRUE, TRUE, "usage: <object>.bounds <regx> <regy> <width> <length>\n" },
  { "burn", CmdBurn, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, FALSE, FALSE, "usage: <character>.burn\n" },
  { "calcwalk", CmdCalcWalkmap, -1, -1, FALSE, TRUE, "usage: calcwalkmap\n" },
  { "cast", CmdCast, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <character>.cast <name or talisman list>\n" },
  { "centeron", CmdCenterOn, 0, 0, FALSE, FALSE, "usage: <object>.centeron OR\n\t centeron <object> OR\n\t centeron <x> <y> <z> [<level>]\n" },
  { "choice", CmdChoice, -1, -1, TRUE, FALSE, "usage: choice <label> <text string>\n" },
  { "combat", CmdCombat, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <object>.combat [off|on|<target>]\n" },
  { "control", CmdControl, -1, -1, FALSE, FALSE, "usage: control <on|off>\n" },
  { "curplayer", CmdCurPlayer, OBJCLASS_PLAYER, -1, FALSE, FALSE, "usage: <player>.curplayer\n" },
  { "delete", CmdDelete, 0, 0, FALSE, FALSE, "usage: <object>.delete\n" },
  { "delinv", CmdDelInv, 0, 0, FALSE, FALSE, "usage: <object>.delinv [<amt>] <obj>\n" },
  { "delrc", CmdDeleteRC, -1, -1, FALSE, TRUE, "usage: deleterc\n" },
  { "deselect", CmdDeselect, -1, -1, FALSE, TRUE, "usage: deselect [<selection number>]\n" },
  { "dxstats", CmdDXStats, -1, -1, FALSE, FALSE, "usage: dxstats\n" },
  { "else", CmdElse, -1, -1, FALSE, FALSE, "usage: if <condition>\n          <command block>\n"
                                  "         else <alternate command block>\n" },
  { "end", CmdEnd, -1, -1, FALSE, FALSE, "usage: begin\n         <block>\n       end\n" },
  { "exit", CmdExit, OBJCLASS_EXIT, -1, TRUE, TRUE, "usage: <object>.exit <name> [posonly]\n" },
  { "extents", CmdExtents, 0, 0, FALSE, TRUE, "usage: <object>.extents [<state>] [FRONT]\n" },
  { "face", CmdFace, 0, 0, TRUE, FALSE, "usage: <object>.face <angle>\n" },
  { "flip", CmdFlip, 0, 0, FALSE, FALSE, "usage: <object>.flip\n" },
  { "follow", CmdFollow, OBJCLASS_EXIT, -1, FALSE, TRUE, "usage: <exit>.follow\n" },
  { "force", CmdForce, 0, -1, TRUE, FALSE, "usage: force <state name>\n" },
  { "frame", CmdFrame, 0, -1, TRUE, FALSE, "usage: frame <frame num>\n" },
  { "generate", CmdGenerate, -1, -1, FALSE, TRUE, "usage: generate [<sizex> <sizey>] [from <startx> <starty>]\n" },
  { "get", CmdGet, 0, 0, FALSE, FALSE, "usage: <character>.get <object>\n" },
  { "getstate", CmdGetState, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, FALSE, FALSE, "usage: <character>.getstate\n" },
  { "give", CmdGive, 0, 0, TRUE, FALSE, "usage: <object>.give <to> [<amt>] <obj>\n" },
  { "go", CmdGo, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <character>.go <angle>\n" },
  { "goto", CmdGoto, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <character>.goto <x> <y>\n" },
  { "group", CmdGroup, 0, 0, FALSE, TRUE, "usage: group <groupnum>\n"},
  { "help", CmdHelp, -1, -1, FALSE, TRUE, "usage: help [<command>]\n" },
  { "if", CmdIf, -1, -1, TRUE, FALSE, "usage: if <condition>\n          <command block>\n"
                                  "         else <alternate command block>\n" },
  { "jump", CmdJump, -1, -1, TRUE, FALSE, "usage: jump <label>\n" },
  { "knockback", CmdKnockBack, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <character>.knockback <x> <y> <z>\n" },
  { "level", CmdLevel, -1, -1, TRUE, FALSE, "usage: level <level number>\n" },
  { "light", CmdLight, 0, 0, TRUE, FALSE, "usage: <object>.light <on/off>    <object>.light <intensity>\n"
                "       <object>.light color <red> <green> <blue>\n"
                "       <object>.light position <x> <y> <z>\n" },
  { "load", CmdLoad, -1, -1, TRUE, TRUE, "usage: load [game <game number> | sectors]\n" },
  { "lock", CmdLock, 0, 0, FALSE, TRUE, "usage: lock\n" },
  { "mappos",   CmdMapPos, -1, -1, TRUE, TRUE, "usage: mappos list [or] <name> [or] <x> <y> <z>\n" },
  { "memory", CmdMemory, -1, -1, FALSE, FALSE, "usage: memory\n" },
  { "mono", CmdMono, -1, -1, TRUE, FALSE, "usage: mono <percent>\n" },
  { "move", CmdMove, 0, 0, TRUE, FALSE, "usage: <object>.move <dx> <dy> [<dz>]\n" },
  { "name", CmdName, 0, 0, TRUE, FALSE, "usage: <object>.name [type/clear] <name>\n" },
  { "newgame", CmdNewGame, -1, -1, FALSE, FALSE, "usage: newgame\n" },
  { "pivot", CmdPivot, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <object>.pivot <angle>\n" },
  { "play", CmdPlay, -1, -1, TRUE, FALSE, "usage: play <sound>\n" },
  { "play3d", CmdPlay3D, 0, 0, TRUE, FALSE, "usage: <object>.play3d <sound>\n" },
  { "pos", CmdPos, 0, 0, TRUE, FALSE, "usage: <object>.pos <dx> <dy> [<dz> [<level>]]\n" },
  { "pulp", CmdPulp, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <character>.pulp <x> <y> <z>\n" },
  { "reg", CmdRegistration, 0, 0, FALSE, TRUE, "usage: <object>.registration <deltax> <deltay>\n" },
  { "replace", CmdReplace, 0, 0, TRUE, FALSE, "usage: <object>.replace [<class>] [<newobjtype>]\n" },
  { "restore", CmdRestore, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, FALSE, FALSE, "usage: <character>.restore\n" },
  { "reveal", CmdReveal, 0, -1, FALSE, FALSE, "usage: reveal\n" },
  { "rotate", CmdRotate, 0, 0, FALSE, TRUE, "usage: <object>.rotate <x> <y> <z>\n" },
  { "save", CmdSave, -1, -1, FALSE, TRUE, "usage: save [game <game number> | map | headers | classes | exits]\n" },
  { "savetilebm", CmdSaveTileBM, -1, -1, FALSE, TRUE, "usage: savetilebm\n" },
  { "say", CmdSay, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <character>.say [<nowait>] [choice|\"string\"]\n" },
  { "script", CmdScript, 0, 0, FALSE, TRUE, "usage: <object>.script\n" },
  { "scrollto", CmdScrollTo, 0, 0, FALSE, FALSE, "usage: <object>.scrollto OR\n\t scrollto <object> OR\n\t scrollto <x> <y> <z> [<level>]\n" },
  { "sectorcommand", CmdSectorCommand, -1, -1, TRUE, TRUE, "usage: sectorcommand <level> <command>\n" },
  { "select", CmdSelect, -1, -1, TRUE, TRUE, "usage: select [#.]<object name>\n       select <#/next/prev>\n" },
  { "set", CmdSet, -1, -1, TRUE, FALSE, "usage: set <state name> <new value>\n" },
  { "setdrip", CmdDrip, 0, 0, TRUE, FALSE, "usage: <drip object>.setdrip <ripplesize> <height> <period>\n" },
  { "smoothscroll", CmdSmoothScroll, -1, -1, TRUE, FALSE, "usage: smoothscroll <on/off>\n" },
  { "stat", CmdStat, 0, 0, FALSE, FALSE, "usage: <object>.stat [<amt>]<name> <value>]\n" },
  { "state", CmdState, 0, -1, TRUE, FALSE, "usage: state <state number>\n" },
  { "stop", CmdStop, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, FALSE, FALSE, "usage: <object>.stop\n" },
  { "show", CmdShow, -1, -1, TRUE, FALSE, "usage: show <class> [<type>]\n" },
  { "swap", CmdSwap, 0, 0, TRUE, TRUE, "usage: swap <obj to swap with>\n" },
  { "take", CmdTake, 0, 0, TRUE, FALSE, "usage: <object>.take <from> [<amt>] <obj>\n" },
  { "template", CmdTemplate, 0, -1, FALSE, TRUE, "usage: <object.>template\n" },
  { "text", CmdText, OBJCLASS_SCROLL, -1, FALSE, TRUE, "usage: <scroll>.text\n" },
  { "tilewalk", CmdTileWalkmap, OBJCLASS_TILE, -1, FALSE, TRUE, "usage: tilewalkmap\n" },
  { "toback", CmdToBack, 0, 0, FALSE, TRUE, "usage: toback\n" },
  { "tofront", CmdToFront, 0, 0, FALSE, TRUE, "usage: tofront\n" },
  { "toggle", CmdToggle, 0, 0, TRUE, FALSE, "usage: <object>.toggle <flagname>\n" },
  { "trigger", CmdTrigger, 0, 0, TRUE, FALSE, "usage: trigger <trigname>\n" },
  { "try", CmdTry, 0, -1, TRUE, FALSE, "usage: try <state name>\n" },
  { "undo", CmdUndo, -1, -1, FALSE, TRUE, "usage: undo\n" },
  { "unlock", CmdUnlock, 0, 0, FALSE, TRUE, "usage: unlock\n" },
  { "use", CmdUse, 0, 0, FALSE, FALSE, "usage: <object>.use [<with object>]\n" },
  { "visible", CmdSetVisibility, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, TRUE, FALSE, "usage: <character>.visible <state #>\n" },
  { "walkmap", CmdWalkmap, 0, 0, TRUE, TRUE, "usage: walkmap <delta z>\n" },
  { "wait", CmdWait, OBJCLASS_CHARACTER, OBJCLASS_PLAYER, FALSE, FALSE, "usage: <character>.wait [<wait type>]\n" },
  { "while", CmdWhile, -1, -1, TRUE, FALSE, "usage: while <condition>\n         <command block>\n" },
  { "zoffset", CmdZOffset, 0, 0, FALSE, TRUE, "usage: <object>.zoffset <zoffset>\n" },

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

char *sprintbit(int bits, char *enumbits[], char *buf)
{
    for (int i = 0; enumbits[i]; i++)
        if (bits & (1 << i))
            sprintf(buf, "%s %s", buf, enumbits[i]);

    return buf;
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

// *************************
// * Map Generation System *
// *************************

// NOTE: This should be moved to its own separate file!!!

extern PTTerrainTemplate TerrainTemplates;

#define TERRAIN_NOTHING     0
#define TERRAIN_GRASS       1
#define TERRAIN_DIRT        2
#define TERRAIN_MOUNTAIN    3
#define TERRAIN_LAKE        4
#define TERRAIN_RIVER       5
#define TERRAIN_OCEAN       6
#define TERRAIN_STREET      7

#define NUMTERRAINTYPES     8
#define TERRAINTYPESHIFT    4

#define OVERLAY_NOTHING     0
#define OVERLAY_PATH        1
#define OVERLAY_ROAD        2
#define OVERLAY_FORESTSTART 16
#define OVERLAY_FORESTEND   32

BYTE GetTerrainType(BYTE *map, int x, int y)
{
    if (x < 0 || x >= (MAXPLATESX-1) || y < 0 || y >= (MAXPLATESY-1))
        return (TERRAIN_OCEAN << TERRAINTYPESHIFT);

    return *(map + (y * MAXPLATESX) + x);
}

#define TERRAIN(dx, dy)     GetTerrainType(map, x + dx, y + dy)

int BaseTerrain(BYTE *map, int x, int y)
{
    int lowest = 1000000;

    for (int y0 = 0; y0 < 2; y0++)
        for (int x0 = 0; x0 < 2; x0++)
            if ((TERRAIN(x0, y0) & 0x0F) < lowest)
                lowest = TERRAIN(x0, y0) & 0x0F;

    return lowest;
}

int GetTileCode(BYTE *map, int x, int y)
{
    int base = BaseTerrain(map, x, y);

    long code = ((TERRAIN(0, 0) - base) << 24) | ((TERRAIN(1, 0) - base) << 16) |
                ((TERRAIN(0, 1) - base)  << 8) | ((TERRAIN(1, 1) - base));

    return code;
}

#define WORLD_ZSTART        64          // the minimum z coord for generated objects

int zstart[NUMTERRAINTYPES] = { WORLD_ZSTART, 16 + WORLD_ZSTART, 16 + WORLD_ZSTART, 16 + WORLD_ZSTART,
                                WORLD_ZSTART, WORLD_ZSTART - 8, 18 + WORLD_ZSTART, 16 + WORLD_ZSTART };

#define ELEVATIONCHANGE     46

int TerrainHeight(BYTE *map, int x, int y)
{
    int lowest = 1000000;
    int add;

    for (int y0 = 0; y0 < 2; y0++)
        for (int x0 = 0; x0 < 2; x0++)
            if ((TERRAIN(x0, y0) & 0x0F) < lowest)
            {
                lowest = TERRAIN(x0, y0) & 0x0F;
                add = zstart[TERRAIN(x0, y0) >> TERRAINTYPESHIFT];
            }
            else if ((TERRAIN(x0, y0) & 0x0F) == lowest)
            {
                add = min(add, zstart[TERRAIN(x0, y0) >> TERRAINTYPESHIFT]);
            }

    return (lowest * ELEVATIONCHANGE) + add;
}

struct { char ch; int code; } TerrainCodes[] = {
 { 'x', 0x00 }, { 'g', 0x10 }, { 'd', 0x20 }, { 'k', 0x11 },
 { 'i', 0x14 }, { 'm', 0x30 }, { 'r', 0x50 }, { 'o', 0x60 },
 { 0, 0 } // terminator
};

int TerrainToCode(char ch)
{
    if (ch >= 'A' && ch <= 'Z')
        ch += 'a' - 'A';

    for (int i = 0; TerrainCodes[i].ch != 0; i++)
        if (ch == TerrainCodes[i].ch)
            return TerrainCodes[i].code;

    return -1;
}

int FindSuperTile(BYTE *map, int x, int y, BYTE *filled, int *flux)
{
    if (flux)
        *flux = 0;

    for (int i = 0; i < TileClass.NumTypes(); i++)
    {
        if (TileClass.GetObjType(i) == NULL || !TileClass.GetStat(i, "supertile"))
            continue;

        int width = TileClass.GetStat(i, "width");
        int height = TileClass.GetStat(i, "height");
        char *name = TileClass.GetObjType(i)->name + 3;   // Added 3 for the "FOR" (forest) prefix

        if ((size_t)(width * height) != strlen(name))
        {
            Output("width*height != name length for supertile\n");
            continue;
        }

        int base = 1000;
        for (int y0 = 0; y0 < height; y0++)
            for (int x0 = 0; x0 < width; x0++)
                if (BaseTerrain(map, x+x0, y+y0) < base)
                    base = BaseTerrain(map, x+x0, y+y0);

        BOOL qualifies = TRUE;
        for (int h = 0; h < height && qualifies; h++)
            for (int w = 0; w < width && qualifies; w++)
            {
                int code = TerrainToCode(*name++);
                if (code < 0)
                {
                    Output("bad code character in supertile\n");
                    continue;
                }

                if (code != (TERRAIN((w+1)/2, (h+1)/2) - base))
                    qualifies = FALSE;
            }

        if (qualifies)
        {
            for (int h = 0; h < height/2; h++)
                for (int w = 0; w < width/2; w++)
                    *(filled+((x+w)*MAXPLATESY)+(y+h)) = 1;

            return i;
        }
    }

    return -1;
}

int overlay(BYTE *overmap, int x, int y)
{
    if (x < 0 || x >= MAXPLATESX || y < 0 || y >= MAXPLATESY)
        return OVERLAY_NOTHING;

    return *(overmap+(y*MAXPLATESX)+x);
}

#define OVERLAY(dx, dy) (overlay(overmap, x+(dx), y+(dy)))

int GetOverlayTile(BYTE *overmap, int x, int y, int code)
{
    if (OVERLAY(0, 0) == OVERLAY_PATH)
    {
        char pathname[80] = "path";

        if (code == 0x10101010)
        {
            if (OVERLAY(0, -1) == OVERLAY_PATH)
                strcat(pathname, "N");
            if (OVERLAY(1, 0) == OVERLAY_PATH)
                strcat(pathname, "E");
            if (OVERLAY(0, 1) == OVERLAY_PATH)
                strcat(pathname, "S");
            if (OVERLAY(-1, 0) == OVERLAY_PATH)
                strcat(pathname, "W");
        }
        else if (code == 0x20102010 && OVERLAY(1, 0) == OVERLAY_PATH)
            strcat(pathname, "BE");
        else if (code == 0x20201010 && OVERLAY(0, 1) == OVERLAY_PATH)
            strcat(pathname, "BS");
        else if (code == 0x10201020 && OVERLAY(-1, 0) == OVERLAY_PATH)
            strcat(pathname, "BW");
        else if (code == 0x10102020 && OVERLAY(0, -1) == OVERLAY_PATH)
            strcat(pathname, "BN");

        return TileClass.FindObjType(pathname);
    }

    return -1;
}

PTObjectInstance GenerateObj(PTSector sect, BYTE *map, BYTE *overmap, int type, int x, int y, int px, int py, int flux = 0)
{
    PTObjectInstance inst;
    SObjectDef def;

    memset(&def, 0, sizeof(SObjectDef));
    def.objclass = OBJCLASS_TILE;

    int tx = (x * PLATESPERSECTX) + px;
    int ty = (y * PLATESPERSECTY) + py;
    int tmp = GetOverlayTile(overmap, tx, ty, TileClass.GetStat(type, "Code"));
    if (tmp >= 0)
        type = tmp;

    def.objtype = type;
    def.flags = OF_GENERATED;
    def.pos.x = (x * SECTORWIDTH) + (px * PLATEWIDTH) + (PLATEWIDTH >> 1);
    def.pos.y = (y * SECTORHEIGHT) + (py * PLATEHEIGHT) + (PLATEHEIGHT >> 1);
    def.pos.z = TerrainHeight(map, (x * PLATESPERSECTX) + px, (y * PLATESPERSECTY) + py) - (flux * ELEVATIONCHANGE);
    def.level = sect->SectorLevel();

    inst = TObjectClass::GetClass(def.objclass)->NewObject(&def);
    if (!inst)
        return NULL;

    sect->AddObject(inst);

    int code = TileClass.GetStat(def.objtype, "Extra");

    if (code != 0xFFFFFFFF)
    {
        int nexttype = TileClass.FindRandStatVal(TileClass.FindStat("Code"), code);
        def.pos.z += (flux * ELEVATIONCHANGE);
        GenerateObj(sect, map, overmap, nexttype, x, y, px, py);
    }

    int over = overlay(overmap, tx, ty);
//  if (over >= OVERLAY_FORESTSTART && over < OVERLAY_FORESTEND)
        TerrainTemplates->ApplyTemplate(inst, sect, random(0, 10));//over - OVERLAY_FORESTSTART + 1);

    return inst;
}

#define QUAD(c, q)  (((c) >> ((q) * 8)) & 0xFF)

void GenerateMap(int startx, int starty, int sizex, int sizey)
{
    TObjectImagery::PauseLoader(); // NOTE: if resume isn't called, program will lock

    PTSector sect;
    BYTE filled[MAXPLATESX][MAXPLATESY];
    int x, y, px, py;
    FILE *fp;

    memset(filled, 0, MAXPLATESX*MAXPLATESY);

    if ((fp = popen("map.bmp", "r")) == NULL)
    {
        TObjectImagery::ResumeLoader();
        return;
    }

    BYTE *map = new BYTE[MAXPLATESX*MAXPLATESY];
    fread(map, 14, 1, fp);
    x = *((int *)(map + 10));
    fseek(fp, x, SEEK_SET);
    // fuckin' upsidedown bitmaps...
    for (y = MAXPLATESY-1; y >= 0; y--)
        fread(map+(y*MAXPLATESX), MAXPLATESX, 1, fp);
    fclose(fp);

    if ((fp = popen("overmap.bmp", "r")) == NULL)
    {
        TObjectImagery::ResumeLoader();
        return;
    }

    BYTE *overmap = new BYTE[MAXPLATESX*MAXPLATESY];
    fread(overmap, 14, 1, fp);
    x = *((int *)(overmap + 10));
    fseek(fp, x, SEEK_SET);
    for (y = MAXPLATESY-1; y >= 0; y--)
        fread(overmap+(y*MAXPLATESX), MAXPLATESX, 1, fp);
    fclose(fp);

    MapPane.FreeAllSectors();

    for (int sy = starty; sy < (starty + sizey); sy++)
        for (int sx = startx; sx < (startx + sizex); sx++)
        {
            sect = new TSector(0, sx, sy);
            sect->Load();

            for (TObjectIterator i(sect->ObjectArray()); i; i++)
                if (i.Item() && i.Item()->GetFlags() & OF_GENERATED)
                    sect->ObjectArray()->Remove(i);

            for (py = 0; py < PLATESPERSECTY; py++)
                for (px = 0; px < PLATESPERSECTX; px++)
                {
                    x = (sx * PLATESPERSECTX) + px;
                    y = (sy * PLATESPERSECTY) + py;

                    if (filled[x][y] != 0)
                        continue;

                    int type, flux = 0;
                    if ((type = FindSuperTile(map, x, y, &(filled[0][0]), &flux)) < 0)
                    {
                        DWORD code = GetTileCode(map, x, y);

                        // leave out the 'center' of the mountains
                        if ((code & 0xF0F0F0F0) == 0x30303030)
                            continue;

                        // use solid street tile for anything containing street
                        for (int i = 0; i < 4; i++)
                            if (QUAD(code, i) == 0x70)
                                code = 0x70707070;


                        if ((type = TileClass.FindRandStatVal(TileClass.FindStat("Code"), code, NULL)) < 0 &&
                            (type = TileClass.FindRandStatVal(TileClass.FindStat("Code"), code, &flux)) < 0)
                            continue;

                        PTObjectInstance inst = GenerateObj(sect, map, overmap, type, sx, sy, px, py, flux);

                        if (!inst)
                            continue;

                        DWORD oldcode = TileClass.GetStat(inst->ObjType(), "Code");

                        // build a mask based on the wildcards in the code
                        DWORD newcode = 0;
                        if (!QUAD(oldcode, 3))
                            newcode |= code & 0xFF000000;
                        if (!QUAD(oldcode, 2))
                            newcode |= code & 0xFF0000;
                        if (!QUAD(oldcode, 1))
                            newcode |= code & 0xFF00;
                        if (!QUAD(oldcode, 0))
                            newcode |= code & 0xFF;

                        if (newcode != 0)
                        {
                            if (!QUAD(newcode, 0))
                                if (QUAD(newcode, 1))
                                    newcode |= QUAD(newcode, 1);
                                else if (QUAD(newcode, 2))
                                    newcode |= QUAD(newcode, 2);
                                else
                                    newcode |= QUAD(newcode, 3);

                            if (!QUAD(newcode, 1))
                                if (QUAD(newcode, 0))
                                    newcode |= QUAD(newcode, 0) << 8;
                                else if (QUAD(newcode, 3))
                                    newcode |= QUAD(newcode, 3) << 8;
                                else
                                    newcode |= QUAD(newcode, 2) << 8;

                            if (!QUAD(newcode, 2))
                                if (QUAD(newcode, 0))
                                    newcode |= QUAD(newcode, 0) << 16;
                                else if (QUAD(newcode, 3))
                                    newcode |= QUAD(newcode, 3) << 16;
                                else
                                    newcode |= QUAD(newcode, 1) << 16;

                            if (!QUAD(newcode, 3))
                                if (QUAD(newcode, 1))
                                    newcode |= QUAD(newcode, 1) << 24;
                                else if (QUAD(newcode, 2))
                                    newcode |= QUAD(newcode, 2) << 24;
                                else
                                    newcode |= QUAD(newcode, 0) << 24;

                            flux = 1000;
                            for (int i = 0; i < 4; i++)
                                if ((QUAD(newcode, i) & 0x0F) < (DWORD)flux)
                                    flux = QUAD(newcode, i) & 0x0F;

                            for (i = 0; i < 4; i++)
                                newcode -= flux << (i * 8);

                            if ((type = TileClass.FindRandStatVal(TileClass.FindStat("Code"), newcode)) < 0)
                                continue;

                            flux *= -1;
                        }
                        else
                            continue;
                    }

                    GenerateObj(sect, map, overmap, type, sx, sy, px, py, flux);
                }

            delete sect;
        }

    delete map;
    delete overmap;

    TObjectImagery::ResumeLoader();

    MapPane.ReloadSectors();
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
    char anim[32];
    char sound[32];

    anim[0] = NULL;
    sound[0] = NULL;

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

    while (t.Is("anim") || t.Is("sound"))
    {
        if (t.Is("anim"))
        {
            t.WhiteGet();
            strncpy(anim, t.Text(), 31);
            t.WhiteGet();
        }
        else if (t.Is("sound"))
        {
            t.WhiteGet();
            strncpy(sound, t.Text(), 31);
            t.WhiteGet();
        }
    }

    char *text = NULL;
    int tagid = -1;
    if (t.Is("choice"))
    {
        if (DialogPane.GetResponse())
        {
            text = DialogPane.GetResponse();
            tagid = DialogList.FindLine(DialogPane.GetResponse());
        }

        t.WhiteGet();
    }
    else 
    {
        if (t.Type() == TKN_TEXT)
        {
            strcpy(buf, t.Text());
            text = buf;
            tagid = -1;

            t.WhiteGet();
        }
        else if (t.Type() == TKN_IDENT)
        {
            strcpy(buf, t.Text());
            text = buf;
            tagid = DialogList.FindLine(text);

            t.WhiteGet();
        }
        else
            return CMD_BADPARAMS;
    }

    if (tagid >= 0)
        ((PTCharacter)context)->SayTag(tagid, wait, (anim[0])?anim:NULL);
    else
        ((PTCharacter)context)->Say(buf, wait, (anim[0])?anim:NULL, (sound[0])?sound:NULL);

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

COMMAND(CmdStop)
{
    ((PTCharacter)context)->Stop();
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

// pulp a character
COMMAND(CmdKnockBack)
{
    S3DPoint pos;
    if(!Parse(t, "%d %d %d", &pos.x, &pos.y, &pos.z))
        return CMD_BADPARAMS;

    ((PTCharacter)context)->KnockBack(pos);
    
    return 0;
}

// burn a character
COMMAND(CmdBurn)
{
    ((PTCharacter)context)->Burn();
    
    return 0;
}

COMMAND(CmdDrip)
{
    int ripplesize, height, period;
    if (!Parse(t, "%d %d %d", &ripplesize, &height, &period))
        return CMD_BADPARAMS;

    ((PTDripEffect)context)->SetParams(ripplesize, height, period);
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

COMMAND(CmdCast)
{
    char string[80];

    if (!(t.Type() == TKN_IDENT || t.Type() == TKN_TEXT))
        return CMD_BADPARAMS;

    strcpy(string, t.Text());
    t.WhiteGet();

    BOOL cast = ((PTCharacter)context)->CastByName(string);
    if (!cast)
        cast = ((PTCharacter)context)->CastByTalismans(string);

    if (!cast)
        Output("Spell failed.");
    else
        Output("Spell cast.");

    return 0;
}

// ***************************
// * Various Editor Commands *
// ***************************

COMMAND(CmdSelect)
{
    if (t.Is("next"))
    {
        if (!StatusBar.Next())
            Output("Already at end of selection list.\n");
        return 0;
    }
    else if (t.Is("prev"))
    {
        if (!StatusBar.Prev())
            Output("Already at start of selection list.\n");
        return 0;
    }
    else if (t.Is("all"))
    {
        for (TMapIterator i; i; i++)
        {
            if (!StatusBar.Select(i.Item()->GetMapIndex(), TRUE))
            {
                Output("Selection overflowed, max objects selected.\n");
                break;
            }
        }

        return 0;
    }

    if (t.Type() == TKN_NUMBER)
    {
        // make the index number specified the current object
        int index;
        if (!Parse(t, "%d", &index))
            return CMD_BADPARAMS;

        if (!StatusBar.SetCurObj(index))
            Output("Invalid selection index.\n");

        return 0;
    }

    PTObjectInstance inst = MapPane.FindClosestObject(t.Text(), NULL, TRUE);

    if (!inst)
        Output("Can't find any object by that name.\n");
    else if (!StatusBar.Select(inst->GetMapIndex()))
        Output("Unable to select any more objects.\n");

    t.WhiteGet();
    return 0;
}

COMMAND(CmdDeselect)
{
    if (t.Is("all"))
        while (StatusBar.Deselect());
    else
        if (!StatusBar.Deselect())
            Output("Nothing to deselect.\n");

    t.WhiteGet();
    return 0;
}

COMMAND(CmdAdd)
{
    int objtype = -1;
    int number = 1;

    S3DPoint pos;
    MapPane.GetMapPos(pos);

    if (t.Type() == TKN_NUMBER)
    {
        number = t.Index();
        t.WhiteGet();
    }

    if (t.Is("next"))
    {
        S3DPoint zero;
        memset(&zero, 0, sizeof(S3DPoint));
        ClassPane.SelObjType(ClassPane.GetObjType() + 1);
        ClassPane.PutObject(zero);
        t.WhiteGet();
        return 0;
    }
    else if (t.Is("prev"))
    {
        S3DPoint zero;
        memset(&zero, 0, sizeof(S3DPoint));
        ClassPane.SelObjType(ClassPane.GetObjType() - 1);
        ClassPane.PutObject(zero);
        t.WhiteGet();
        return 0;
    }
    else if (t.Is("light", 1))
    {
        SObjectDef def;
        int intensity = 220, mult = 12;
        memset(&def, 0, sizeof(SObjectDef));

        def.objclass = OBJCLASS_TILE;
        def.objtype = TileClass.FindObjType("light");
        def.flags = OF_LIGHT | OF_EDITOR;

        def.pos = pos;
        def.pos.z = 60;
        def.level = MapPane.GetMapLevel();

        //t.WhiteGet();
        //if (!Parse(t, "<%d> <%d>", &intensity, &mult))
        //  return CMD_BADPARAMS;

        int index = MapPane.NewObject(&def);
        if (index < 0)
            Output("ERROR: Unable to add light\n");
        else
        {
            SColor color = { 255, 255, 255 };
            MapPane.GetInstance(index)->SetLightIntensity(intensity);
            MapPane.GetInstance(index)->SetLightColor(color);
            MapPane.GetInstance(index)->SetLightMultiplier(mult);

            S3DPoint lightpos(0, 0, 0);
            MapPane.GetInstance(index)->SetLightPos(lightpos);
            StatusBar.Select(index);
        }
    }
    else // Add an object
    {
        PTObjectClass cl;

        for (int i = 0; i < MAXOBJECTCLASSES; i++)
        {
            cl = TObjectClass::GetClass(i);
            if (cl && (objtype = cl->FindObjType(t.Text())) >= 0)
                break;
        }

        if (objtype >= 0)
        {
            SObjectDef def;
            memset(&def, 0, sizeof(SObjectDef));

            def.objclass = cl->ClassId();
            def.objtype = objtype;
            def.flags = 0;
            def.level = MapPane.GetMapLevel();
            def.pos = pos;

            int index = MapPane.NewObject(&def);
            if (index < 0)
                Output("ERROR: Creating object\n");
            else
            {
                StatusBar.Select(index);

                Output("%s:%s added at (%d, %d, %d).\n", cl->ClassName(),
                    cl->GetObjType(objtype)->name, def.pos.x, def.pos.y, def.pos.z);

                if (number != 1)
                    MapPane.GetInstance(index)->SetAmount(number);

                // If we added a player, add him to the player manager
                if (def.objclass == OBJCLASS_PLAYER)
                {
                    PTObjectInstance oi = MapPane.GetInstance(index);
                    if (oi)
                    {
                        PlayerManager.AddPlayer((PTPlayer)oi);
                        PlayerManager.SetMainPlayer((PTPlayer)oi);
                    }
                }
            }

            t.WhiteGet();
        }
        else
            Output("No object type named '%s'.\n", t.Text());
    }

    return 0;
}

COMMAND(CmdAddInv)
{
    int objtype = -1;
    int number = 1;

    if (t.Type() == TKN_NUMBER)
    {
        number = t.Index();
        t.WhiteGet();
    }
    
    if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
        return CMD_BADPARAMS;

    if (!context->AddToInventory(t.Text(), number))
    {
        Output("Unable to add %s", t.Text());
        t.SkipLine();
        return 0;
    }

    t.WhiteGet();

    return 0;
}

COMMAND(CmdDelInv)
{
    int number = 1;

    if (t.Type() == TKN_NUMBER)
    {
        number = t.Index();
        t.WhiteGet();
    }
    
    if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
        return CMD_BADPARAMS;

    int count = context->DeleteFromInventory(t.Text(), number);
    Output("%d %s deleted", count, t.Text());

    t.WhiteGet();
    
    return 0;
}

COMMAND(CmdGive)
{
    int number = 1;

    if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
        return CMD_BADPARAMS;

    PTObjectInstance inst = MapPane.FindClosestObject(t.Text(), context);
    if (inst < 0)
    {
        Output("Unable to find object %s\n", t.Text());
        t.SkipLine();
        return 0;
    }

    t.WhiteGet();

    if (t.Type() == TKN_NUMBER)
    {
        number = t.Index();
        t.WhiteGet();
    }
    
    if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
        return CMD_BADPARAMS;

    int count = context->GiveInventoryTo(inst, t.Text(), number);
    Output("%d %s given", count, t.Text());

    t.WhiteGet();
    
    return 0;
}

COMMAND(CmdTake)
{
    int number = 1;

    if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
        return CMD_BADPARAMS;

    PTObjectInstance inst = MapPane.FindClosestObject(t.Text(), context);
    if (!inst)
    {
        Output("Unable to find object %s\n", t.Text());
        t.SkipLine();
        return 0;
    }

    t.WhiteGet();

    if (t.Type() == TKN_NUMBER)
    {
        number = t.Index();
        t.WhiteGet();
    }
    
    if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
        return CMD_BADPARAMS;

    int count = inst->GiveInventoryTo(context, t.Text(), number);
    Output("%d %s taken", count, t.Text());

    t.WhiteGet();
    
    return 0;
}

COMMAND(CmdCalcWalkmap)
{
    MapPane.CalculateWalkmap();
    return 0;
}

COMMAND(CmdWalkmap)
{
    int deltaz;
    BOOL nonzero = FALSE, absolute = FALSE;

    while (t.Type() == TKN_IDENT)
    {
        if (t.Is("nonzero", 1))
            nonzero = TRUE;
        else if (t.Is("absolute", 1))
            absolute = TRUE;
        else if (t.Is("reset", 1))
        {
            MapPane.SetWalkmapReveal(0, 0, SECTORWIDTH >> WALKMAPSHIFT, SECTORHEIGHT >> WALKMAPSHIFT);
            t.WhiteGet();
            return 0;
        }
        else
            return CMD_BADPARAMS;

        t.WhiteGet();
    }

    if (!Parse(t, "%d", &deltaz))
        return CMD_BADPARAMS;

    MapPane.AdjustWalkmap(deltaz, nonzero);

    if (context)
        MapPane.SaveWalkmap(context);

    return 0;
}

COMMAND(CmdTileWalkmap)
{
    context->GetImagery()->SetWorldReg(context->GetState(), PLATEWIDTH >> (WALKMAPSHIFT+1), PLATEHEIGHT >> (WALKMAPSHIFT+1), 0);
    context->GetImagery()->SetWorldBoundBox(context->GetState(), PLATEWIDTH >> WALKMAPSHIFT, PLATEHEIGHT >> WALKMAPSHIFT, 0);
    return 0;
}

static void OutputState(PTObjectInstance context, int state)
{
    char flstr[32];
    int flags = context->GetImagery()->GetAniFlags(state);
    flstr[0] = NULL;
    if (flags & AF_LOOPING)
        strcat(flstr, "l");
    if (flags & AF_SYNCHRONIZE)
        strcat(flstr, "s");
    if (flags & AF_NOINTERPOLATION)
        strcat(flstr, "i");
    if (flags & AF_PINGPONG)
        strcat(flstr, "p");
    if (flags & AF_REVERSE)
        strcat(flstr, "r");
    if (flags & AF_ROOT)
        strcat(flstr, "r");
    if (flags & AF_FLY)
        strcat(flstr, "f");
    if (flags & AF_NOMOTION)
        strcat(flstr, "n");
    if (flags & AF_ACCURATEKEYS)
        strcat(flstr, "a");
    if (flags & AF_ROOT2ROOT)
        strcat(flstr, "o");

    Output("%d \"%s\" Len: %d Flags: %s\n", state, context->GetImagery()->GetAniName(state), 
        context->GetImagery()->GetAniLength(state), flstr);
}

COMMAND(CmdState)
{
    int st = -1;

    if (t.Type() == TKN_IDENT && (t.Is("list") || t.Is("find")))
    {
        BOOL find = t.Is("find");
        t.WhiteGet();

        BOOL match = FALSE;
        int matchlen = -1;
        char matchbuf[80];
        if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
        {
            strcpy(matchbuf, t.Text());
            t.Get();
            if (t.Is("*"))
            {
                strcat(matchbuf, "*"); 
                t.WhiteGet();
            }
            strlwr(matchbuf);
            matchlen = strlen(matchbuf);
            if (matchlen > 0 && matchbuf[matchlen - 1] == '*')
            {
                matchlen--;
                matchbuf[matchlen] = NULL; 
            }
            else
                matchlen = -1;
            match = TRUE;
        }
        else
        {
            if (find)
                return CMD_BADPARAMS;
        }

        for (int c = 0; c < context->GetImagery()->NumStates(); c++)
        {
            char *name = context->GetImagery()->GetAniName(c);

            if (find)       // Find the word in the state name for 'find'
            {
                strcpy(buf, name);
                strlwr(buf);
                if (!strstr(buf, matchbuf))
                    continue;
            }
            else if (match)     // Do pattern match for 'list'
            {
                if (matchlen > 0)
                {
                    if (strnicmp(name, matchbuf, matchlen))
                        continue;
                }
                else
                {
                    if (!stricmp(name, matchbuf))
                        continue;
                }
            }

            OutputState(context, c);
        }
    }
    else
    {
        if (t.Type() == TKN_RETURN)
        {
            st = context->GetState();
        }
        else
        {
            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                st = context->FindState(t.Text());
                t.WhiteGet();
            }
            else if (t.Type() == TKN_NUMBER)
            {
                st = t.Index();
                t.WhiteGet();
            }
            else
                return CMD_BADPARAMS;

            if (st < 0 || st >= context->GetImagery()->NumStates())
            {
                Output("Invalid state\n");
                return 0;
            }

            context->SetState(st);
        }

        OutputState(context, st);
    }

    return CMD_WAIT;
}

COMMAND(CmdFrame)
{
    int frame;

    if (!Parse(t, "%d", &frame))
        return CMD_BADPARAMS;

    context->SetFrame(frame);

    return 0;
}

COMMAND(CmdTry)
{
    char newstate[80];

    if (!context->IsComplex())
        return 0;

    if (!Parse(t, "%t", &newstate))
        return CMD_BADPARAMS;

    ((PTComplexObject)context)->Try(newstate);

    return CMD_WAIT;
}

COMMAND(CmdForce)
{
    char newstate[80];

    if (!context->IsComplex())
        return 0;

    if (!Parse(t, "%t", &newstate))
        return CMD_BADPARAMS;

    ((PTComplexObject)context)->Force(newstate);

    return CMD_WAIT;
}

COMMAND(CmdReveal)
{
    if (context && context->GetFlags() & OF_REVEAL)
        context->ResetFlags(context->GetFlags() & ~OF_REVEAL);

    else if (context)
        context->SetFlags(OF_REVEAL);

    return 0;
}

COMMAND(CmdLevel)
{
    int level;
    if (!Parse(t, "%d", &level))
        return CMD_BADPARAMS;

    MapPane.SetMapLevel(level);
    return 0;
}

COMMAND(CmdTemplate)
{
    int index = TerrainTemplates->NewTemplate(context->GetTypeName());
    if (index < 0)
    {
        Output("Max number of templates reached\n");
        return 0;
    }

    S3DPoint pos;
    context->GetPos(pos);

    int objlist[MAXOBJECTREFS];
    int numfound = MapPane.FindObjectsInRange(pos, objlist, PLATEWIDTH, PLATEHEIGHT, OBJCLASS_TILE, MAXOBJECTREFS);
    int numadded = 0;

    for (int i = 0; i < numfound; i++)
    {
        PTObjectInstance inst = MapPane.GetInstance(objlist[i]);
        if (inst && inst != context)
        {
            S3DPoint ipos;
            inst->GetPos(ipos);
            ipos.x -= pos.x;
            ipos.y -= pos.y;
            ipos.z -= pos.z;

            if (TerrainTemplates->AddObject(index, inst, ipos) < 0)
            {
                Output("Too many objects nearby, the last %d were left out.\n", numfound - i);
                break;
            }

            numadded++;
        }
    }

    Output("%d objects added to template for '%s'.\n", numadded, context->GetTypeName());

    return 0;
}

COMMAND(CmdSmoothScroll)
{
    int retval = 0;

    if (t.Is("on"))
    {
        if (!SmoothScroll)
        {
            SmoothScroll = TRUE;
            MapPane.RedrawAll();
        }
    }
    else if (t.Is("off"))
    {
        if (SmoothScroll)
        {
            SmoothScroll = FALSE;
            MapPane.RedrawAll();
        }
    }
    else
        retval = CMD_BADPARAMS;

    t.Get();

    return retval;
}

COMMAND(CmdDLight)
{
    int intensity;

    if (!Parse(t, "%d", &intensity))
        return CMD_BADPARAMS;

    MapPane.SetDLightIntensity(intensity);
    return 0;
}


COMMAND(CmdText)
{
    Console.Hide();
    ScrollEditor.Show();
    PlayScreen.SetExclusivePane(&ScrollEditor);
    ScrollEditor.SetScroll(context);

    return 0;
}

COMMAND(CmdExit)
{
    if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
        return CMD_BADPARAMS;

    strcpy(buf, t.Text());
    t.WhiteGet();

    BOOL getamb = TRUE;
    if (t.Type() == TKN_IDENT && t.Is("noambient", 1))
    {
        getamb = FALSE;
        t.WhiteGet();
    }

    TExit::AddExit(buf, context, getamb);

    Output("Exit from '%s' added.\n", buf);

    TExit::WriteExitList();     // write immediately just to be safe

    return 0;
}

COMMAND(CmdFollow)
{
    if (!((PTExit)context)->Activate())
        Output("Nothing defined for this exit, can't follow\n");

    return 0;
}

COMMAND(CmdRestore)
{
    ((PTCharacter)context)->RestoreHealth();
    return 0;
}

COMMAND(CmdGet)
{
    if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
        return CMD_BADPARAMS;

    int index = -1;
    PTObjectInstance inst = MapPane.FindClosestObject(t.Text(), NULL, TRUE);

    if (inst == NULL)
        Output("Can't find any object by that name.\n");
    else
    {
        if (inst->IsInInventory())
            inst->RemoveFromInventory();
        else
            inst->RemoveFromMap();

        context->AddToInventory(inst);
    }

    t.WhiteGet();

    return 0;
}

BOOL IsSectorCommand = FALSE;

COMMAND(CmdSectorCommand)
{
    char command[512];
    int level;

    strcpy(command, (char *)t.GetPos());

    if (!Parse(t, "%d", &level))
        return CMD_BADPARAMS;

    if (t.Type() == TKN_EOF || t.Type() == TKN_RETURN)
        return CMD_BADPARAMS;

    sprintf(buf, "Processing command \"%s\" on level %d...\n", command, level);
    Output(buf);

    if (MapPane.GetMapLevel() == level)
    {
        MapPane.FreeAllSectors();
        MapPane.RedrawAll();
    }

    IsSectorCommand = TRUE;

    for (int sy = 0; sy < MAXSECTORY; sy++)
        for (int sx = 0; sx < MAXSECTORX; sx++)
        {
            PTSector sector = new TSector(level, sx, sy);
            sector->Load(FALSE);

            for (int i = 0; i < sector->NumItems(); i++)
            {
                PTObjectInstance inst = sector->GetInstance(i);
                if (inst)
                {
                    TStringParseStream s(command, strlen(command));
                    TToken t0(s);
                    t0.Get();
                    CommandInterpreter(inst, t0, MINCMDABREV);
                }
            }

            sector->Save();
            delete sector;
        }

    while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
        t.WhiteGet();

    IsSectorCommand = FALSE;

    return 0;
}

COMMAND(CmdSwap)
{
    if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
        return CMD_BADPARAMS;

    PTObjectInstance inst = MapPane.FindClosestObject(t.Text(), NULL, TRUE);

    if (inst == NULL)
        Output("Can't find any object by that name.\n");
    else
        if (!MapPane.SwapDrawOrder(context, inst))
            Output("Hrm...the objects are in different sectors.  I guess you're screwed.\n");

    t.WhiteGet();

    return 0;
}

COMMAND(CmdToFront)
{
    MapPane.PushToFront(context);
    return 0;
}

COMMAND(CmdToBack)
{
    MapPane.PushToBack(context);
    return 0;
}

/*COMMAND(CmdAlias)
{
    if (t.Type() == TKN_IDENT)
    {
        // Get a list of aliases
        if (t.Is("list"))
        {
            t.WhiteGet();
            for (int c = 0; c < NumAliases(); c++)
            {
                sprintf(buf, "%s = %s\n", GetAliasName(c), GetAlias(c));
                Output(buf);
            }
            return 0;
        }
    
        // Set a new alias
        char aliasname[30], alias[30];
        if (!Parse(t, "%29s", aliasname))
            return CMD_BADPARAMS;

        if (t.Type() == TKN_IDENT)
            if (!Parse(t, "%30s", aliasname))
                return CMD_BADPARAMS;
        if (t.Type() == TKN_TEXT)
            if (!Parse(t, "%30t", aliasname))
                return CMD_BADPARAMS;
            
        if (SetAlias(aliasname, alias) >= 0)
            Output("Alias set");
        else
            Output("Too many aliases");
    }

    return 0;
}
*/
COMMAND(CmdGroup)
{
    int group;

    if (t.Type() == TKN_NUMBER)         // Set our group
    {
        if (!Parse(t, "%d", &group))
            return CMD_BADPARAMS;

        if (context)
            context->SetGroup(group);
    }
    else                                // Show what group we're in
    {
        if (context)
        {
            sprintf(buf, "Group: %d\n", context->GetGroup());
            Output(buf);
        }
    }
    return 0;
}

COMMAND(CmdPlay)
{
    if (t.Type() == TKN_TEXT || t.Type() == TKN_IDENT)
    {
        strcpy(buf, t.Text());
        t.WhiteGet();
    }
    else
        return CMD_BADPARAMS;

    PLAY(buf);

    return 0;
}

COMMAND(CmdPlay3D)
{
    if (t.Type() == TKN_TEXT || t.Type() == TKN_IDENT)
    {
        strcpy(buf, t.Text());
        t.WhiteGet();
    }
    else
        return CMD_BADPARAMS;

    if (!context)
        return CMD_BADPARAMS;

    context->PlayWave(buf);

    return 0;
}

COMMAND(CmdTrigger)
{
    if (t.Type() != TKN_TEXT && t.Type() != TKN_IDENT)
        return CMD_BADPARAMS;

    if (context->GetScript())
        context->GetScript()->Trigger(TRIGGER_TRIGGER, t.Text());

    t.WhiteGet();

    return 0;
}

COMMAND(CmdNewGame)
{
    PlayScreen.NewGame();

    return 0;
}

COMMAND(CmdCurPlayer)
{
    PlayerManager.SetMainPlayer((PTPlayer)context);
    
    Output("%s is now current player\n", context->GetName());

    return 0;
}

COMMAND(CmdUndo)
{
    StatusBar.Undo();
    return 0;
}

COMMAND(CmdExtents)
{
    if (!context->GetAnimator())
    {
        Output("Context has no animator\n");
        return 0;
    }

    int showstate = -1;
    if (t.Is("RESET"))
    {
        Output("Resetting state screen extents\n");

        t.WhiteGet();

        BOOL front = FALSE;
        int state = -1;

        if (t.Type() == TKN_NUMBER)
            Parse(t, "%d", &state);

        if (t.Is("FRONT"))
        {
            front = TRUE;
            t.WhiteGet();
        }
            
        ((PT3DAnimator)context->GetAnimator())->RecordNewExtents(context, state, front);
    }
    else if (t.Type() == TKN_NUMBER)
    {
        int snum, x, y, w, h;

        snum = context->GetState();

        if (!Parse(t, "%i %i %i %i", &x, &y, &w, &h))
            return CMD_BADPARAMS;
        
        PSImageryStateHeader st = context->GetImagery()->GetState(snum);
        st->regx = x; st->regy = y;
        st->width = w; st->height = h;

        showstate = snum;
    }
    else
    {
        showstate = context->GetState();
    }

    if (showstate >= 0)
    {
        PSImageryStateHeader st = context->GetImagery()->GetState(showstate);

        Output("Screen extents state %d: regx %d regy %d width %d height %d\n",
            showstate, st->regx, st->regy, st->width, st->height);
    }

    return 0;
}

COMMAND(CmdBounds)
{
    int regx, regy, width, length;

    if (!Parse(t, "%d %d %d %d", &regx, &regy, &width, &length))
        return CMD_BADPARAMS;

    int state = context->GetState();
    if (t.Type() == TKN_NUMBER)
        Parse(t, "%d", &state);

    if (!context->GetImagery())
        Output("Sorry, context has no imagery\n");
    else
    {
        context->GetImagery()->SetWorldBoundBox(state, 0, 0, 1);
        context->GetImagery()->SetWorldReg(state, 0, 0, 0);
        context->GetImagery()->SetWorldBoundBox(state, width, length, 1);
        context->GetImagery()->SetWorldReg(state, regx, regy, 0);
        if (StatusBar.EditWalkmap() && state == context->GetState())
            MapPane.SnapWalkDisplay(context->GetMapIndex());
    }

    return 0;
}

COMMAND(CmdToggle)
{
    if (t.Type() != TKN_IDENT)
        return CMD_BADPARAMS;

    char *flagname = NULL;

    for (int i = 0; i < context->GetNumFlags(); i++)
    {
        flagname = context->GetFlagName(i);
        if (t.Is(flagname, 3))
            break;
    }

    if (!flagname)
    {
        Output("No object flag by that name\n");
        while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
            t.Get();
        return 0;
    }
    else
    {
        t.WhiteGet();
        
        if (t.Is("="))
            t.WhiteGet();

        BOOL value;
        if (t.Is("on") || t.Is("1") || t.Is("true"))
        {
            value = 1;
            t.WhiteGet();
        }
        else if (t.Is("off") || t.Is("0") || t.Is("false"))
        {
            value = 0;
            t.WhiteGet();
        }
        else
        {
            value = !context->IsFlagSet(flagname);
        }

        context->SetFlag(flagname, value);
    }

    return 0;
}

COMMAND(CmdMove)
{
    int newx = 0, newy = 0, newz = 0;

    if (!Parse(t, "%i %i", &newx, &newy))
        return CMD_BADPARAMS;

    if (t.Type() == TKN_NUMBER)
        if (!Parse(t, "%i", &newz))
            return CMD_BADPARAMS;

    S3DPoint oldpos;
    context->GetPos(oldpos);
    S3DPoint newpos = oldpos;

    newpos.x += newx;
    newpos.y += newy;
    newpos.z += newz;

    MapPane.AddObjectUpdateRect(context->GetMapIndex());
    context->SetPos(newpos, -1, IsSectorCommand);
    MapPane.AddObjectUpdateRect(context->GetMapIndex());

    return 0;
}

COMMAND(CmdPos)
{
    S3DPoint newpos;
    context->GetPos(newpos);
    int newlevel = -1;

    BOOL add = FALSE;
    if (t.Is("add"))
    {
        add = TRUE;
        t.WhiteGet();
    }

    if (!Parse(t, "%i %i", &newpos.x, &newpos.y))
        return CMD_BADPARAMS;

    if (t.Type() == TKN_NUMBER)
        if (!Parse(t, "%i", &newpos.z))
            return CMD_BADPARAMS;

    if (t.Type() == TKN_NUMBER)
        if (!Parse(t, "%i", &newlevel))
            return CMD_BADPARAMS;
    
    if (add)
        newpos += context->Pos();

    MapPane.AddObjectUpdateRect(context->GetMapIndex());
    context->SetPos(newpos, newlevel, IsSectorCommand);
    MapPane.AddObjectUpdateRect(context->GetMapIndex());

    return 0;
}

COMMAND(CmdRotate)
{
    int x, y, z;

    BOOL add = FALSE;
    if (t.Is("add"))
    {
        add = TRUE;
        t.WhiteGet();
    }

    if (!Parse(t, "%i %i %i", &x, &y, &z))
        return CMD_BADPARAMS;

    if (add)
    {
        context->SetRotateX(x + context->GetRotateX());
        context->SetRotateY(y + context->GetRotateY());
        context->SetRotateZ(z + context->GetRotateZ());
    }
    else
    {
        context->SetRotateX(x);
        context->SetRotateY(y);
        context->SetRotateZ(z);
    }

    return 0;
}

COMMAND(CmdDelete)
{
    MapPane.DeleteObject(context);
    return CMD_DELETED;
}

COMMAND(CmdStat)
{
    /* enumeration of object flags */
    char *objflags[] = OBJFLAGNAMES;
    /* enumeration of light flags */
    char *lightflags[] = { "DIR", "SUN", "MOON", NULL };
    /* enumeration of container flags */
    char *contflags[] = { "LOCKED", NULL };

    PTObjectClass cl = TObjectClass::GetClass(context->ObjClass());

    if (t.Type() == TKN_IDENT)
    {
        if (t.Is("list"))
        {
            int c;
            t.WhiteGet();
            Output("Class stats\n");
            for (c = 0; c < cl->NumStats(); c++)
            {
                Output("%s = %d\n", cl->GetStatDefString(c, buf), cl->GetStat(context->ObjType(), c));
                if (c != 0 && (c % 5) == 0)
                {
                    Output("Press any key");
                    Console.GetChar();
                }
            } 
            Output("Object stats\n");
            Console.GetChar();
            for (c = 0; c < cl->NumObjStats(); c++)
            {
                Output("%s (%d) = %d\n", cl->GetObjStatDefString(c, buf), cl->GetObjStat(context->ObjType(), c), context->GetObjStat(c));
                if (c != 0 && (c % 5) == 0)
                {
                    Output("Press any key");
                    Console.GetChar();
                }
            } 
            return 0;
        }

        strcpy(buf, t.Text());
        t.WhiteGet();

        int statid = cl->FindStat(buf);
        int objstatid = cl->FindObjStat(buf);
        if (statid < 0 && objstatid < 0)
        {
            Output("No stat by that name in that object's class\n");
            return 0;
        }

        if (t.Is("delete"))
        {
            t.WhiteGet();

            if (statid >= 0)
                cl->DeleteStat(statid);
            if (objstatid >= 0)
                cl->DeleteObjStat(objstatid);

            Output("Stat Deleted\n");
            return 0;
        }

        if (t.Is("="))
        {
            t.WhiteGet();

            int val;
            if (!Parse(t, "%i", &val))
                return CMD_BADPARAMS;
    
            context->SetStat(buf, val);

            Output("Stat Set: ");
        }

        if (t.Is("reset"))
        {
            t.WhiteGet();

            if (statid >= 0)
                cl->ResetStat(context->ObjType(), statid);
            else if (objstatid >= 0)
                context->ResetStat(objstatid);

            Output("Stat Reset: ");
        }

        if (t.Type() != TKN_EOF)
            return CMD_BADPARAMS;

        if (statid >= 0)
            Output("%s = %d\n", cl->GetStatDefString(statid, buf), cl->GetStat(context->ObjType(), statid));
        else if (objstatid >= 0)
            Output("%s (%d) = %d\n", cl->GetObjStatDefString(objstatid, buf), cl->GetObjStat(context->ObjType(), objstatid), context->GetObjStat(objstatid));
        return 0;
    }

    // if no params, just display the object's stats
    S3DPoint pos;
    context->GetPos(pos);
    sprintf(buf, "\"%s\" %s:%s (%d, %d, %d)\n", context->GetName(),
            context->GetTypeName(), context->GetClassName(), pos.x, pos.y, pos.z);
    Output(buf);

    strcpy(buf, "Flags:");
    if (context->GetFlags())
        sprintbit(context->GetFlags(), objflags, buf);
    else
        strcat(buf, " (none)");
    strcat(buf, "\n");
    Output(buf);

    PTObjectImagery i = context->GetImagery();
    int s = context->GetState();
    sprintf(buf, "Registration (%d, %d, %d)  AnimReg (%d, %d, %d)\n",
            i->GetRegX(s), i->GetRegY(s), i->GetRegZ(s),
            i->GetAnimRegX(s), i->GetAnimRegY(s), i->GetAnimRegZ(s));
    Output(buf);

    if (context->GetFlags() & OF_LIGHT)
    {
        PSLightDef def;
        def = context->GetLightDef();

        sprintf(buf, "Light: intensity %d, multiplier %d, flags [", def->intensity, def->multiplier);

        if (context->GetLightFlags())
            sprintbit(context->GetLightFlags(), lightflags, buf);
        else
            strcat(buf, "--");

        sprintf(buf, "%s]\n       color (%d, %d, %d), pos (%d, %d, %d)\n"
                     "       3d index: %d lightid: %d\n",
                buf, def->color.red, def->color.green, def->color.blue,
                def->pos.x, def->pos.y, def->pos.z,
                def->lightindex, def->lightid);
        Output(buf);
    }

    return 0;
}

COMMAND(CmdScript)
{
    if (context->GetScript())
    {
        Console.Hide();
        ScriptEditor.Show();
        PlayScreen.SetExclusivePane(&ScriptEditor);
        ScriptEditor.LoadScript(context);
    }

    return 0;
}

int CenterOnFunc(PTObjectInstance context, TToken &t, BOOL scroll)
{
    S3DPoint pos;

    if (Editor && context && t.Type() != TKN_IDENT && t.Type() != TKN_NUMBER)
    {
        context->GetPos(pos);
        MapPane.SetMapPos(pos);
    }
    else
    {
        if (t.Type() == TKN_IDENT)
        {
            PTObjectInstance inst = MapPane.FindClosestObject(t.Text());
            if (!inst)
            {
                Output("Object not found");
                return 0;
            }
            if (Editor)
            {
                inst->GetPos(pos);
                MapPane.SetMapPos(pos);
            }
            else
                MapPane.CenterOnObj(inst, scroll);

            t.WhiteGet();
        }
        else if (t.Type() == TKN_NUMBER)
        {
            if (!Parse(t, "%i %i %i", &pos.x, &pos.y, &pos.z))
                return CMD_BADPARAMS;
            int level = MapPane.GetMapLevel();
            if (t.Type() == TKN_NUMBER)
                if (!Parse(t, "%i", &level))
                    return CMD_BADPARAMS;
            if (Editor)
            {
                MapPane.SetMapPos(pos);
            }
            else
                MapPane.CenterOnPos(pos, level, scroll);
        }
        else
            return CMD_BADPARAMS;
    }

    return 0;
}

COMMAND(CmdCenterOn)
{
    return CenterOnFunc(context, t, FALSE);
}

COMMAND(CmdScrollTo)
{
    return CenterOnFunc(context, t, TRUE);
}

#define MAX_MAP_LOCATIONS       32

struct { char name[RESNAMELEN]; struct { int x, y, z; } pos; int level; } MapLocations[MAX_MAP_LOCATIONS];
int numlocations = 0;

BOOL ReadMapLocationList()
{
    char fname[MAX_PATH];
    sprintf(fname, "%slocation.def", ClassDefPath);

    FILE *fp = TryOpen(fname, "rb");
    if (!fp)
        return FALSE;

    TFileParseStream s(fp);
    TToken t(s);

    t.Get();

    numlocations = 0;

    do
    {
        if (numlocations >= MAX_MAP_LOCATIONS)
            break;

        if (t.Type() == TKN_RETURN || t.Type() == TKN_WHITESPACE)
            t.LineGet();

        if (t.Type() == TKN_EOF)
            break;

        if (!Parse(t, "%t (%d, %d, %d) level %d", MapLocations[numlocations].name, &MapLocations[numlocations].pos.x, &MapLocations[numlocations].pos.y, &MapLocations[numlocations].pos.z, &MapLocations[numlocations].level))
            return FALSE;

        t.SkipLine();       // skip past any other garbage on the line, including the newline

        numlocations++;
    } while (t.Type() != TKN_EOF);

    fclose(fp);
    return TRUE;
}

COMMAND(CmdMapPos)
{
    S3DPoint pos;
    int level = MapPane.GetMapLevel();

    if (t.Type() == TKN_IDENT)
    {
        if (t.Is("list"))
        {
            Output("The following map locations are availible:\n");

            for (int i = 0; i < numlocations; i++)
            {
                sprintf(buf, "%20s (%5d, %5d, %3d) level %2d\n", MapLocations[i].name, MapLocations[i].pos.x, MapLocations[i].pos.y, MapLocations[i].pos.z, MapLocations[i].level);
                Output(buf);
            }

            t.WhiteGet();
            return 0;
        }

        for (int i = 0; i < numlocations; i++)
            if (t.Is(MapLocations[i].name, 3))
            {
                pos.x = MapLocations[i].pos.x;
                pos.y = MapLocations[i].pos.y;
                pos.z = MapLocations[i].pos.z;
                level = MapLocations[i].level;
                break;
            }

        t.WhiteGet();

        if (i >= numlocations)
        {
            Output("No map location by that name exists.  Try 'map list' for a listing.\n");
            return CMD_BADPARAMS;
        }
    }
    else if (!Parse(t, "%d %d %d", &pos.x, &pos.y, &pos.z))
        return CMD_BADPARAMS;

    MapPane.SetMapPos(pos);
    MapPane.SetMapLevel(level);
    return 0;
}

BOOL ShowObjList(PTObjectClass cl, char *match)
{
    BOOL ismatch = FALSE;
    int cnt = 0;

    for (int i = 0; i < cl->NumTypes(); i++)
    {
        if (cl->GetObjType(i) == NULL || (match != NULL && abbrevcmp(match, cl->GetObjType(i)->name) < 1))
            continue;

        ismatch = TRUE;

        sprintf(buf, "%s %-19s", buf, cl->GetObjType(i)->name);
        if (++cnt >= 3)
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
        Console.Output(buf);
    }

    return ismatch;
}

void ShowObjTypeInfo(PTObjectClass cl, int objtype)
{
    PSObjectInfo inf = cl->GetObjType(objtype);

    if (!cl || !inf)
        return;
/*
    sprintf(buf, "Name: %s, Class: %s, Imagery: %s, Usecount: %d\n",
            inf->name, cl->ClassName(), img->GetUseCount(), img->Get);
    Output(buf);
  */
    if (cl->NumStats())
    {
        Output("Statistics:\n");

        int len = 0, add;

        for (int i = 0; i < cl->NumStats(); i++)
        {
            sprintf(buf+len, "%s = %d, ", cl->StatName(i), cl->GetStat(objtype, i));
            add = strlen(buf) - len;
            if ((len+add) >= Console.winwidth && len != 0)
            {
                if (i == (cl->NumStats() - 1))
                    len -= 2;
                buf[len] = '\n';
                buf[len+1] = 0;
                Output(buf);
                strcpy(buf, buf+len+2);
                len = add;
            }
            else
                len += add;
        }

        if (len > 0)
        {
            len -= 2;
            buf[len] = '\n';
            buf[len+1] = 0;
            Output(buf);
        }
    }
}

COMMAND(CmdShow)
{
    PTObjectClass cl;
    buf[0] = 0;

    if (t.Is("classes"))
    {
        Output("Object Classes:\n");
        for (int i = 0, cnt = 0; i < TObjectClass::NumClasses(); i++)
        {
            cl = TObjectClass::GetClass(i);
            if (cl == NULL)
                continue;

            sprintf(buf, "%s %-19s", buf, cl->ClassName());
            if (++cnt >= 3)
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
    }
    else
    {
        if ((cl = TObjectClass::GetClass(TObjectClass::FindClass(t.Text()))))
        {
            t.WhiteGet();
            if (t.Type() == TKN_IDENT)
            {
                int objtype = cl->FindObjType(t.Text());
                if (objtype < 0)
                {
                    if (!ShowObjList(cl, t.Text()))
                    {
                        sprintf(buf, "%s contains no type named '%s'.\n", cl->ClassName(), t.Text());
                        Output(buf);
                    }
                }
                else
                    ShowObjTypeInfo(cl, objtype);
            }
            else
                ShowObjList(cl, NULL);
        }
        else
            Output("No class by that name.\n");
    }

    t.WhiteGet();
    return 0;
}

COMMAND(CmdAmbient)
{
    int amb;
    if (!Parse(t, "%d", &amb))
        return CMD_BADPARAMS;

    MapPane.SetAmbientLight(amb);
    MapPane.RedrawAll();

    return 0;
}

COMMAND(CmdAmbColor)
{
    SColor color;
    if (!Parse(t, "%b %b %b", &color.red, &color.green, &color.blue))
        return CMD_BADPARAMS;

    MapPane.SetAmbientColor(color);
    MapPane.RedrawAll();

    return 0;
}

void Mono(int percent, int dummy = -1)
{
    SetMonoPercent(percent);
    MapPane.RedrawAll();
}

COMMAND(CmdMono)
{
    int percent;
    if (!Parse(t, "%d", &percent))
        return CMD_BADPARAMS;

    Mono(percent);
    return 0;
}

COMMAND(CmdDirLight)
{
    return 0;
}

COMMAND(CmdBaseLight)
{
    SColor color;
    int id, mult = -1;

    if (!Parse(t, "%d %b %b %b <%d>", &id, &color.red, &color.green, &color.blue, &mult))
        return CMD_BADPARAMS;

    SetLightColor(id, color, mult);
    return 0;
}

COMMAND(CmdReplace)
{
    PTObjectClass cl = TObjectClass::GetClass(TObjectClass::FindClass(t.Text()));
    if (cl)
        t.WhiteGet();
    else
        cl = TObjectClass::GetClass(context->ObjClass());

    if (cl)
    {
        // Add a new instance of a given object type in the map pane
        int objtype = cl->FindObjType(t.Text());
        if (objtype < 0)
        {
            sprintf(buf, "%s has no object named '%s'.\n", cl->ClassName(), t.Text());
            Output(buf);
        }
        else
        {
            SObjectDef def;
            memset(&def, 0, sizeof(SObjectDef));

            def.objclass = cl->ClassId();
            def.objtype = objtype;
            def.level = MapPane.GetMapLevel();

            context->GetPos(def.pos);
            int index = MapPane.NewObject(&def);
            if (index < 0)
                Console.Output("ERROR: Creating object\n");
            else
            {
                int oldindex = context->GetMapIndex();
                StatusBar.Deselect(oldindex);
                PTObjectInstance oi = MapPane.GetInstance(oldindex);
                MapPane.DeleteObject(oi);
                StatusBar.Select(index, TRUE);
            }
        }
    }
    else
    {
        sprintf(buf, "No class named '%s'.\n", t.Text());
        Output(buf);
    }

    return 0;
}

COMMAND(CmdLight)
{
    if (t.Is("on"))
    {
        if (!(context->GetFlags() & OF_LIGHT))
        {
            context->SetFlags(OF_LIGHT);
            MapPane.AddObjectUpdateRect(context->GetMapIndex());
        }
        t.WhiteGet();
        return 0;
    }

    if (t.Is("off"))
    {
        if (context->GetFlags() & OF_LIGHT)
        {
            context->ResetFlags(context->GetFlags() & (~OF_LIGHT));
            MapPane.AddObjectUpdateRect(context->GetMapIndex());
        }
        t.WhiteGet();
        return 0;
    }

    if (t.Type() == TKN_NUMBER)
    {
        t.WhiteGet();
        int intensity;
        if (!Parse(t, "%d", &intensity))
            return CMD_BADPARAMS;

        context->SetLightIntensity(intensity);
        return 0;
    }

    if (t.Is("directional", MINCMDABREV))
    {
        t.WhiteGet();
        DWORD oldflags = context->GetFlags();

        if (t.Is("on"))
            context->SetLightFlags(LIGHT_DIR);
        else if (t.Is("off"))
            context->ResetLightFlags(context->GetLightFlags() & (~LIGHT_DIR));
        else
        {
            t.WhiteGet();
            return CMD_BADPARAMS;
        }

        if (oldflags != context->GetFlags())
            MapPane.AddObjectUpdateRect(context->GetMapIndex());

        t.WhiteGet();
        return 0;
    }

    if (t.Is("intensity", MINCMDABREV))
    {
        t.WhiteGet();
        int intensity;
        if (!Parse(t, "%d", &intensity))
            return CMD_BADPARAMS;

        context->SetLightIntensity(intensity);
        return 0;
    }

    if (t.Is("color", MINCMDABREV))
    {
        t.WhiteGet();
        SColor color;
        if (!Parse(t, "%b %b %b", &color.red, &color.green, &color.blue))
            return CMD_BADPARAMS;

        context->SetLightColor(color);
        return 0;
    }

    if (t.Is("position", MINCMDABREV))
    {
        t.WhiteGet();
        S3DPoint lightpos;
        if (!Parse(t, "%d %d %d", &lightpos.x, &lightpos.y, &lightpos.z))
            return CMD_BADPARAMS;

        context->SetLightPos(lightpos);
        return 0;
    }

    if (t.Is("multiplier", MINCMDABREV))
    {
        t.WhiteGet();
        int mult;
        if (!Parse(t, "%d", &mult))
            return CMD_BADPARAMS;

        context->SetLightMultiplier(mult);
        return 0;
    }

    return CMD_BADPARAMS;
}

void ZOffset(int zoffset, int dummy = -1)
{
    PTObjectInstance oi = MapPane.GetInstance(StatusBar.GetSelectedObj());

    if (oi)
    {
        PTObjectImagery img = oi->GetImagery();

        if (img)
        {
            int x = img->GetRegX(oi->GetState());
            int y = img->GetRegY(oi->GetState());
            img->SetReg(oi->GetState(), x, y, zoffset);
        }
    }
}

COMMAND(CmdZOffset)
{
    if (t.Type() == TKN_RETURN || t.Type() == TKN_EOF)
    {
        sprintf(buf, "Current zoffset: %d\n", context->GetImagery()->GetRegZ(context->GetState()));
        Output(buf);
        t.WhiteGet();
        return 0;
    }

    int zoff;
    if (!Parse(t, "%d", &zoff))
        return CMD_BADPARAMS;

    PTObjectImagery img = context->GetImagery();

    if (img)
    {
        int x = img->GetRegX(context->GetState());
        int y = img->GetRegY(context->GetState());
        img->SetReg(context->GetState(), x, y, zoff);
    }

    MapPane.AddObjectUpdateRect(context->GetMapIndex());
    return 0;
}

COMMAND(CmdRegistration)
{
    int dx, dy;
    if (!Parse(t, "%d %d", &dx, &dy))
        return CMD_BADPARAMS;

    PTObjectImagery img = context->GetImagery();
    if (!img)
        return 0;

    int x = img->GetRegX(context->GetState());
    int y = img->GetRegY(context->GetState());
    int z = img->GetRegZ(context->GetState());
    img->SetReg(context->GetState(), x - dx, y - dy, z);
    MapPane.AddObjectUpdateRect(context->GetMapIndex());
    return 0;
}

COMMAND(CmdAnimRegistration)
{
    int dx, dy;
    if (!Parse(t, "%d %d", &dx, &dy))
        return CMD_BADPARAMS;

    PTObjectImagery img = context->GetImagery();
    if (!img)
        return 0;

    int x = img->GetAnimRegX(context->GetState());
    int y = img->GetAnimRegY(context->GetState());
    int z = img->GetAnimRegZ(context->GetState());
    img->SetAnimReg(context->GetState(), x - dx, y - dy, z);
    return 0;
}

COMMAND(CmdAnimZ)
{
    int dz;
    if (!Parse(t, "%d", &dz))
        return CMD_BADPARAMS;

    PTObjectImagery img = context->GetImagery();
    if (!img)
        return 0;

    int x = img->GetAnimRegX(context->GetState());
    int y = img->GetAnimRegY(context->GetState());
    img->SetAnimReg(context->GetState(), x, y, dz);
    return 0;
}

COMMAND(CmdName)
{
    if (t.Type() != TKN_IDENT)
        return CMD_BADPARAMS;

    if (t.Is("type"))
    {
        t.WhiteGet();
        if (t.Type() != TKN_IDENT)
            return CMD_BADPARAMS;

        context->SetTypeName(t.Text());
    }
    else if (t.Is("clear"))
        context->SetName("");
    else
        context->SetName(t.Text());

    t.WhiteGet();
    return 0;
}

COMMAND(CmdFlip)
{
    MapPane.AddObjectUpdateRect(context->GetMapIndex());

    if (context->GetFlags() & OF_DRAWFLIP)
        context->ResetFlags(context->GetFlags() & ~(OF_DRAWFLIP));
    else
        context->SetFlags(OF_DRAWFLIP);

    MapPane.AddObjectUpdateRect(context->GetMapIndex());
    return 0;
}

COMMAND(CmdLock)
{
    context->SetFlags(OF_EDITORLOCK);
    return 0;
}

COMMAND(CmdUnlock)
{
    context->ResetFlags(context->GetFlags() & (~OF_EDITORLOCK));
    return 0;
}

COMMAND(CmdLoad)
{
    BOOL loadsector, loadgame;

    loadsector = loadgame = FALSE;
    if (t.Type() != TKN_IDENT)
        loadsector = TRUE;
    else
    {
        while (t.Type() == TKN_IDENT)
        {
            if (t.Is("game"))
                loadgame = TRUE;
            if (t.Is("sector"))
                loadsector = TRUE;
            t.WhiteGet();
        }
    }

    if(loadgame)
    {
        int gamenum = 0;
        if (t.Type() == TKN_NUMBER)
        {
            gamenum = t.Index();
            t.WhiteGet();
        }
        Output("Loading game...\n");
        PlayScreen.LoadGame(gamenum);
    }
            
    if(loadsector)
    {
        Output("Reloading...\n");
        MapPane.ReloadSectors();
    }

    return 0;
}

COMMAND(CmdSave)
{
    // suspend all activity while saving to avoid any nastiness
    TObjectImagery::PauseLoader();

    BOOL savegame, savemap, saveheaders, saveclasses, saveexits;

    savegame = FALSE; // This must be set specifically

    if (t.Type() != TKN_IDENT)
        savemap = saveheaders = saveclasses = saveexits = TRUE;
    else
    {
        savemap = saveheaders = saveclasses = saveexits = FALSE;

        while (t.Type() == TKN_IDENT)
        {
            if (t.Is("game"))
                savegame = TRUE;
            if (t.Is("map"))
                savemap = TRUE;
            if (t.Is("headers"))
                saveheaders = TRUE;
            if (t.Is("classes"))
                saveclasses = TRUE;
            if (t.Is("exits"))
                saveexits = TRUE;
            t.WhiteGet();
        }
    }

    if (saveheaders)
    {
        if (!Console.AdjustingBounds())
        {
            TObjectImagery::SaveAllHeaders();
            Output("Saving object headers...\n");
        }
        else
            Output(" (imagery headers not saved - currently being edited)\n");
    }

    if (saveclasses)
    {
        Output("Saving classes..\n");
        TObjectClass::SetClassesDirty(); // Force class def to save
        TObjectClass::SaveClasses();
        TerrainTemplates->Save();
    }

    if (saveexits)
    {
        Output("Saving exits...\n");
        TExit::WriteExitList();
    }
    
    if (savegame)
    {
        int gamenum = 0;
        if (t.Type() == TKN_NUMBER)
        {
            gamenum = t.Index();
            t.WhiteGet();
        }
        Output("Saving game...\n");
        PlayScreen.SaveGame(gamenum);
    }

    if (savemap)
    {
        Output("Saving map sectors...\n");
        MapPane.SaveAllSectors();
        MapPane.SaveCurMap();  // Copies new sector files to main game map dir
        MapPane.ClearCurMap(); // Clears all map sectors from the 'curmap' directory
    }

    TObjectImagery::ResumeLoader();

    return 0;
}

COMMAND(CmdDXStats)
{
    Output("Direct X Stats.\n");
    sprintf(buf, "Display bits per pixel: %d\n", Display->BitsPerPixel());
    Output(buf);
    sprintf(buf, "Video Memory Total    : %3.1f K\n",
        (float)(GetFreeVideoMem()) / 1024.0);
    Output(buf);

    sprintf(buf, "Video Memory Available: %3.1f K\n",
        (float)(GetFreeVideoMem()) / 1024.0);

    Output(buf);

    if (BlitHardware)
        Output("\nBlit hardware available.\n");
    else
        Output("\nBlit hardware not available.\n");

    sprintf(buf, "Number of 3D objects  : %d\n", Scene3D.GetNumAnimators());
    Output(buf);

    sprintf(buf, "Number of 3D lights   : %d\n", Scene3D.GetNumLights());
    Output(buf);

    if (GetColorMode() == MONO)
        Output("\nRAMP emulation mode on.\n");

    else if (GetColorMode() == COLOR)
        Output("\nRGB emulation mode on.\n");
    else
        Output("\nEmulation mode currently undefined.\n");

    if (IsUsingHardware())
        Output("Hardware 3D support.\n");
    else
        Output("Software 3D support.\n");

    return 0;
}

COMMAND(CmdMemory)
{
    extern MEMORYSTATUS StartMemory;
    extern DWORD ImageryMemUsage;
    extern TotalAllocated, MaxAllocated;

    MEMORYSTATUS mem;
    mem.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&mem);

    Output("Starting memory usage:\n");
    sprintf(buf, "   Percent used: %d%%\n", StartMemory.dwMemoryLoad);
    Output(buf);
    sprintf(buf, "   Physical: %4.1fM  Free: %4.1fM\n", (float)StartMemory.dwTotalPhys / 1024 / 1024, (float)StartMemory.dwAvailPhys / 1024 / 1024);
    Output(buf);
    sprintf(buf, "   Paged:    %4.1fM  Free: %4.1fM\n", (float)StartMemory.dwTotalPageFile / 1024 / 1024, (float)StartMemory.dwAvailPageFile / 1024 / 1024);
    Output(buf);
    sprintf(buf, "   Virtual:  %4.1fM  Free: %4.1fM\n", (float)StartMemory.dwTotalVirtual / 1024 / 1024, (float)StartMemory.dwAvailVirtual / 1024 / 1024);
    Output(buf);

    Output("Current memory usage:\n");
    sprintf(buf, "   Percent used: %d%%\n", mem.dwMemoryLoad);
    Output(buf);
    sprintf(buf, "   Physical: %4.1fM  Free: %4.1fM\n", (float)mem.dwTotalPhys / 1024 / 1024, (float)mem.dwAvailPhys / 1024 / 1024);
    Output(buf);
    sprintf(buf, "   Paged:    %4.1fM  Free: %4.1fM\n", (float)mem.dwTotalPageFile / 1024 / 1024, (float)mem.dwAvailPageFile / 1024 / 1024);
    Output(buf);
    sprintf(buf, "  Virtual:  %4.1fM  Free: %4.1fM\n", (float)mem.dwTotalVirtual / 1024 / 1024, (float)mem.dwAvailVirtual / 1024 / 1024);
    Output(buf);

    sprintf(buf, "Memory usaged by object imagery: %4.1fM\n", (float)ImageryMemUsage / 1024 / 1024);
    Output(buf);
    sprintf(buf, "Memory usaged by chunk cache: %4.1fM\n", (float)ChunkCache.MemUsed() / 1024 / 1024);
    Output(buf);
    sprintf(buf, "Memory allocated:  %4.1fM  Max: %4.1fM\n", (float)TotalAllocated / 1024 / 1024, (float)MaxAllocated / 1024 / 1024);
    Output(buf);

    return 0;
}

COMMAND(CmdGenerate)
{
    int sizex = 2;
    int sizey = 2;

    S3DPoint pos;
    MapPane.GetMapPos(pos);

    if (!Parse(t, "%d %d", &sizex, &sizey))
    {
        int x = pos.x & (SECTORWIDTH - 1);
        int y = pos.y & (SECTORHEIGHT - 1);

        if (x < 256)
            pos.x -= 256;
        else if (x >= (SECTORWIDTH - 256))
            pos.x += 256;
        else
            sizex = 1;

        if (y < 256)
            pos.y -= 256;
        else if (y >= (SECTORHEIGHT - 256))
            pos.y += 256;
        else
            sizey = 1;

    }

    pos.x >>= SECTORWSHIFT;
    pos.y >>= SECTORHSHIFT;

    GenerateMap(pos.x, pos.y, sizex, sizey);

    return 0;
}

