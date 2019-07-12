// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   command.h - Command interpreter                     *
// *************************************************************************

#ifndef _COMMAND_H
#define _COMMAND_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _PARSE_H
#include "parse.h"
#endif

#define COMMAND(x)  int (x)(PTObjectInstance context, TToken &t)

#define CMD_WAIT        (1 << 0)        // wait for command completion
#define CMD_BADCOMMAND  (1 << 1)        // bad command
#define CMD_BADPARAMS   (1 << 2)        // bad parameters
#define CMD_OUTOFMEM    (1 << 3)        // ran out of memory
#define CMD_USAGE       (1 << 4)        // show usage string
#define CMD_DELETED     (1 << 5)        // context is no longer valid
#define CMD_CONDTRUE    (1 << 6)        // conditional is true
#define CMD_CONDFALSE   (1 << 7)        // conditional is false
#define CMD_ELSE        (1 << 8)        // execute if conditional was false
#define CMD_SKIPBLOCK   (1 << 9)        // skip the next code block
#define CMD_LOOP        (1 << 10)       // mark this as a loop point
#define CMD_BEGIN       (1 << 11)       // begin new block
#define CMD_END         (1 << 12)       // end block
#define CMD_JUMP        (1 << 13)       // indicate a code jump occured

#define CMD_ERROR       (CMD_BADCOMMAND | CMD_BADPARAMS | CMD_OUTOFMEM)

_STRUCTDEF(SCommand);
struct SCommand
{
    char *name;                         // command identifier
    COMMAND(*cmdfunc);                  // function to call
    int classcontext;                   // 0 for all, -1 for none, else objclass
    int classcontext2;                  // A secondary class
    BOOL requiresparams;                // if the command requires parameters
    BOOL editoronly;                    // only for use in the editor
    char *usage;                        // help text
};

int CommandInterpreter(PTObjectInstance context, TToken &t, int abrevlen = 0);
void Output(char *fmt, ...);

int StringVal(char *string);

// *******************
// * Alias Functions *
// *******************

// Alias's are set with the "alias" command in the script or editor, and allow you to 
// create an alias that will represent a map object.  This is useful for when you
// are scripting a generic block which does something to an object, and you want to
// set the object that it works with somewhere else. 
//
// For Example:
//
//      alias holding "Flaming Torch"
//      jump useit
//
//      alias holding "Sword"
//      jump useit
//
//      :useit
//      holding.use
//      say "<holding>I'm currently holding a [name]"
// 
// The above aliases both "Flaming Torch" and "Sword" with "holding".  Everywhere you can
// use an object context (i.e. in a command, or in dialog text as above), you can use the
// alias instead.  In dialog text "<obj>" , aliases may also be a class name.
//
// The script system sets these aliases automatically when running scripts:
//
//      "chr"   The current player character
//      "me"    The current script context (who or what the script is associated with)
//      "obj"   The current object we're getting, dropping, using, etc.

/*int NumAliases();
void ClearAliases();
char *GetAliasName(int aliasnum);
char *GetAlias(int aliasnum);
int GetAliasNum(char *aliasname);
char *GetAlias(char *aliasname);
char *GetAliasContext(char *aliasname);
int AddAlias(char *aliasname, char *alias);
*/
#endif