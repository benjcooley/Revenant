// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               parse.h - Low-level file parsing functions              *
// *************************************************************************

#ifndef _PARSE_H
#define _PARSE_H

#include <stdio.h>

#ifndef _REVENANT_H
#include "revenant.h"
#endif

// *******************************************
// * TParseStream - Parser stream base class *
// *******************************************

#define ENDOFSTREAM -1

_CLASSDEF(TParseStream)
class TParseStream
{
  public:
    virtual char *Name() { return ""; }
    virtual int GetChar() = 0;
    virtual void Reset() = 0;
    virtual DWORD GetPos() = 0;
    virtual void SetPos(DWORD newpos) = 0;
};

// ********************************************************
// * TStringParseStream - ASCII string parse stream class *
// ********************************************************

_CLASSDEF(TStringParseStream)
class TStringParseStream : public TParseStream
{
  public:
    TStringParseStream(char *buffer, int len = 0x7FFFFFFF)
        { buf = buffer; end = buf + len; ptr = buffer; }
    virtual char *Name() { return "String"; }
    virtual int GetChar() { if (!*ptr || ptr == end) return ENDOFSTREAM; else return *ptr++; }
    virtual void Reset() { ptr = buf; }
    virtual DWORD GetPos() { return (DWORD)ptr; }
    virtual void SetPos(DWORD newpos) { ptr = (char *)newpos; }

  private:

    char *buf;
    char *end;
    char *ptr;
};

// **********************************************
// * TFileParseStream - FILE parse stream class *
// **********************************************

_CLASSDEF(TFileParseStream)
class TFileParseStream : public TParseStream
{
  public:
    TFileParseStream(FILE *f, char *n = "File") { file = f; name = n; }
    virtual char *Name() { return name; }
    virtual int GetChar()
        { int ch = getc(file); if (ch == EOF) return ENDOFSTREAM; else return ch; }
    virtual void Reset() { fseek(file, 0, SEEK_SET); }
    virtual DWORD GetPos() { return (DWORD)ftell(file); }
    virtual void SetPos(DWORD newpos) { fseek(file, newpos, SEEK_SET); }

  private:

    FILE *file;
    char *name;
};

// *****************************
// * TIdent - Itentifier class *
// *****************************

struct Ident
{
    char ident[MAXIDENTLEN];
    int type;
    int value;
};

#define ISDEFINE -1
#define MAXIDENTS 128

_CLASSDEF(TIdent)
class TIdent
{
  public:
    TIdent();
    ~TIdent();
    int NumIdents() { return numidents; }
    Ident *Find(char *text, int type);
    Ident *Find(char *text) { return Find(text, ISDEFINE); }
    int FindIndex(char *text, int type);
    int FindIndex(char *text) { return FindIndex(text, ISDEFINE); }
    Ident *Get(int index) { return &(idents[index]); }
    int Add(char *ident, int type, int value);
    int Add(char *ident, int value) { return Add(ident, ISDEFINE, value); }

  private:
    int numidents;
    Ident *idents;
};

// ************************
// * TToken - Token class *
// ************************

#define TKN_ERROR       0
#define TKN_WHITESPACE  1
#define TKN_TEXT        2
#define TKN_KEYWORD     3
#define TKN_IDENT       4
#define TKN_DEFINE      5
#define TKN_SCENEID     6
#define TKN_SYMBOL      7
#define TKN_NUMBER      8
#define TKN_RETURN      9
#define TKN_EOF         10

#define KEY_BEGIN       0
#define KEY_END         1
#define KEY_DEFINE      2
#define KEY_INCLUDE     3

#define MAXTOKENTEXT    8192

class TToken
{
  public:
    TToken() { stream = NULL; type = TKN_ERROR;
      index = 0; code = 0; number = 0; text[0] = NULL; lastch = 0; linenum = 1; }
    TToken(TParseStream &s) { stream = &s; type = TKN_ERROR;
      index = 0; code = 0; number = 0; text[0] = NULL; lastch = 0; linenum = 1; }
    
    void SetStream(TParseStream &s) { stream = &s; }

    void Get();         // Gets next token
    void WhiteGet();    // Gets next non-whitespace token
    void LineGet();     // Gets next non-newline/whitespace token
    BOOL DefineGet();   // Gets all whitespace/newline/and #define lines up to next token
                        // for each define, creates a new identifier in idents list.  Returns
                        // FALSE if syntax error in #define or too many defines.

    void SkipLine();    // Skips a line
    void SkipBlanks();  // Skips whitespace and returns
    BOOL SkipBlock();   // Skips a BEGIN/END block (returns FALSE if EOF before END)

    int Type() { return type; }
    int Index() { return index; }
    int Code() { return code; }
    char *Text() { return text; }
    double Number() { return number; }

    BOOL Is(char *istext, int abbrevlen = 0);
    BOOL IsBegin() { return type == TKN_KEYWORD && code == KEY_BEGIN; }
    BOOL IsEnd() { return type == TKN_KEYWORD && code == KEY_END; }
    void DoBegin();                 // Call to compile BEGIN
    void DoEnd();                   // Call to compile END
    int LineNum() { return linenum; }

    DWORD GetPos() { return stream->GetPos(); }
    void SetPos(DWORD pos) { stream->SetPos(pos); }

    void Error(char *err, char *extra = NULL);
      // Fatal error at line number

  private:
    PTParseStream stream;
    int type;
    int index;
    int code;
    double number;
    char text[MAXTOKENTEXT];
    char lastch;
    int linenum;
    TIdent idents;
};

// ***************************************************
// * TScriptObject - Script parsing and interpreting *
// ***************************************************

#define SCRIPT_ERROR    0       // Script has encountered an error
#define SCRIPT_DONE     1       // Script is finished executing/parsing
#define SCRIPT_WAITING  2       // Script is waiting, call Parse() again next frame

class TScriptObject
{
  public:
    TScriptObject(TParseStream &stream) { t.SetStream(stream); }

    virtual void Reset() { s->Reset(); }        // Redefine to reset script state
    virtual int Parse();                    // Redefine to implement script processing

  protected:
    PTParseStream s;
    TToken t;
};

// *********************
// * Parsing Functions *
// *********************

int abbrevcmp(char *abbrev, char *string);

BOOL __cdecl Parse(TToken &t, char *format, ...);
BOOL __cdecl ParseString(char *string, char *format, ...);

#endif
