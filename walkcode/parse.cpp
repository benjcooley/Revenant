// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               parse.cpp - Low-level file parsing functions            *
// *************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "revenant.h"
#include "parse.h"

#ifdef _WINDOWS_
#error WINDOWS.H shouldn't be included here!
#endif

// **************************************
// * Identifiers - Array of identifiers *
// **************************************

TIdent::TIdent()
{
    idents = NULL;
    numidents = 0;
}

TIdent::~TIdent()
{
    if (idents)
        delete idents;
}

Ident *TIdent::Find(char *text, int type)
{
    for (int c = 0; c < numidents; c++)
    {
        if (!strcmp(text, idents[c].ident) && type == idents[c].type)
        return &(idents[c]);
    }
    return NULL;
}

int TIdent::FindIndex(char *text, int type)
{
    for (int c = 0; c < numidents; c++)
    {
        if (!strcmp(text, idents[c].ident) && type == idents[c].type)
        return c;
    }
    return -1;
}

int TIdent::Add(char *ident, int type, int value)
{
    if (!idents)
        idents = new Ident[MAXIDENTS];
    if (!idents)
        FatalError("FATAL ERROR: Couldn't allocate identifier buffer\n");

    if (numidents + 1 > MAXIDENTS)
        return -1;

    strcpy(idents[numidents].ident, ident);
    idents[numidents].type = type;
    idents[numidents].value = value;

    numidents++;

    return numidents - 1;
}

// ************************
// * TToken - Token class *
// ************************

// Defs for parsing the object scripts
#define NUMKEYWORDS     4

char *keywords[NUMKEYWORDS] =
{
    "BEGIN",
    "END",
    "#define",
    "#include"
};

void TToken::Get()
{
    char ch;
    int c;
    int pos;

    if (lastch)
    {
        ch = lastch;
        lastch = 0;
    }
    else ch = stream->GetChar();

    while (ch == 0xD)               // skip CR altogether, \n will be line terminator
        ch = stream->GetChar();

  // Line continuation char
    if (ch == '\\')
    {
        while (ch != '\n' && ch != EOF)
            ch = stream->GetChar();
        if (ch == '\n')
        {
            linenum++;
            ch = stream->GetChar();
        }
    }

  // End of file
    if (ch == EOF)
    {
        text[0] = NULL;
        type = TKN_EOF;
        return;
    }

  // Remark
    else if (ch == '/')
    {
        lastch = stream->GetChar();
        if (lastch == '/')
        {
            while (lastch != '\n' && lastch != EOF)
                lastch = stream->GetChar();
            lastch = 0;
            type = TKN_RETURN;
            text[0] = '\n';
            text[1] = NULL;
            linenum++;
            return;
        }
        else if (lastch == '*')
        {
            lastch = stream->GetChar();
            do
            {
              while (lastch != '*' && lastch != EOF)
                lastch = stream->GetChar();
              lastch = stream->GetChar();
            } while (lastch != '/' && lastch != EOF);
            lastch = stream->GetChar();
        }
        else
        {
            type = TKN_SYMBOL;
            code = ch;
            text[0] = ch;
            text[1] = NULL;
            type = TKN_SYMBOL;
            return;
        }
        type = TKN_WHITESPACE;
        index = 0;
        pos = 0;
        return;
    }

  // Whitespace
    else if ((ch == ' ') || (ch == '\t'))
    {
        type = TKN_WHITESPACE;
        index = 0;
        pos = 0;
        text[pos++] = ch;
        do
        {
            ch = stream->GetChar();
            if (pos < MAXTOKENTEXT - 1)
                text[pos++] = ch;
        }
        while ((ch == ' ') || (ch == '\t'));
        text[pos] = 0;
        lastch = ch;
        return;
    }

  // Return
    else if (ch == '\n')
    {
        type = TKN_RETURN;
        text[0] = '\n';
        text[1] = NULL;
        linenum++;
        return;
    }


  // Text
    else if (ch == '\"')
    {
        pos = 0;
moretext:
        ch = stream->GetChar();
        while (ch != EOF && ch != '\"' && ch != '\n')
        {
            if (pos >= MAXTOKENTEXT - 1)
                Error("String too long");
            if (ch == '\\')
            {
                ch = stream->GetChar();
                if (ch == 'n')
                {
                    text[pos++] = '\n';
                    ch = stream->GetChar();
                }
                else
                    text[pos++] = '\\';
            }
            else
            {
                text[pos++] = ch;
                ch = stream->GetChar();
            }
        }
        /*
        if (ch != '\"')
            Error("ERROR: Unterminated string");
        */
        lastch = stream->GetChar();
        if (lastch == '\\')
        {
            while (lastch != '\n' && lastch != EOF)
                lastch = stream->GetChar();
            do
            {
                ch = stream->GetChar();  // Skip whitespace (note, will not return a whitespace token)
            }
            while ((ch == ' ') || (ch == '\t'));
            lastch = ch;
            if (ch == '\"')                  // Skip
                goto moretext;
        }
        text[pos] = NULL;
        type = TKN_TEXT;
        index = pos;
        return;
    }

  // Identifier or keyword
    else if ((toupper(ch) >= 'A' && toupper(ch) <= 'Z') || ch == '#')
    {
        char upch;
        pos = 0;
        if (ch == '#')
        {
            text[pos++] = ch;
            ch = stream->GetChar();
            if (toupper(ch) < 'A' || toupper(ch) > 'Z')
            {
                type = TKN_SYMBOL;
                code = '#';
                text[1] = NULL;
                lastch = ch;
                return;
            }
        }
        do
        {
            text[pos++] = ch;
            ch = stream->GetChar();
            upch = toupper(ch);
        } while ((upch >= 'A' && upch <= 'Z') ||
          (ch >= '0' && ch <= '9') ||
          (ch == '_'));
        text[pos] = NULL;
        lastch = ch;

      // Find keyword
        for (c = 0; c < NUMKEYWORDS; c++)
        {
            if (!stricmp(text, keywords[c]))
            {
                type = TKN_KEYWORD;
                code = c;
                return;
            }
        }

      // Find identifer
        c = idents.FindIndex(text);
        if (c >= 0)
        {
            type = TKN_DEFINE;
            code = idents.Get(c)->value;
            number = code;
            index = c;
            return;
        }
        else
        {
            type = TKN_IDENT;
            code = -1;
            number = 0;
            index = -1;
            return;
        }
    }

  // Number
    else if ((ch >= '0' && ch <= '9') || (ch == '-'))
    {
        pos = 0;
        int numdec = 0;
        BOOL ishex = FALSE;

      // Check for HEX
        if (ch == '0')
        {
            text[pos++] = ch;
            ch = stream->GetChar();
            if (ch == 'x')
            {
                ishex = TRUE;
                text[pos++] = ch;
                ch = stream->GetChar();
                number = 0;
                while ((ch >= '0' && ch <= '9') ||
                    (ch >= 'A' && ch <= 'F') ||
                    (ch >= 'a' && ch <= 'f'))
                {
                    ch = toupper(ch);
                    if (ch <=  '9') number = number * 16 + ch - '0';
                      else number = number * 16 + 10 + ch - 'A';
                    text[pos++] = ch;
                    ch = stream->GetChar();
                }
                if (pos <= 2)
                    Error("Invalid HEX constant"); 
                text[pos] = NULL;
            }
        }

      // Hey.. it's not HEX
        if (!ishex)
        {
            if (ch == '-')
            {
                text[pos++] = ch;
                ch = stream->GetChar();
                if (!(ch >= '0' && ch <= '9'))
                {
                    lastch = ch;
                    type = TKN_SYMBOL;
                    text[1] = NULL;
                    return;
                }
            }
            while ((ch >= '0' && ch <= '9') || (ch == '.' && numdec <= 1))
            {
                text[pos++] = ch;
                ch = stream->GetChar();
                if (ch == '.') numdec++;
            }
            text[pos] = NULL;
            number = atof(text);
        }

        lastch = ch;
        type = TKN_NUMBER;
        index = (int)number;
        return;
    }

  // Symbol
    else if ((ch > ' ') && (ch <= '~'))
    {
        type = TKN_SYMBOL;
        code = ch;
        text[0] = ch;
        text[1] = NULL;
        return;
    }

  // Invalid Character
    else
    {
        char chr[2];
        chr[0] = ch;
        chr[1] = NULL;
        Error("Invalid character '%s'", chr);

        return;
    }
}

BOOL TToken::DefineGet()
{
    LineGet();
    while (type == TKN_KEYWORD && code == KEY_DEFINE)
    {
        WhiteGet();
        char name[MAXIDENTLEN];
        int val;
        if (!Parse(*this, "%32t %d", name, &val)) 
            return FALSE;
        
        if (idents.Add(name, val) < 0)
            return FALSE;

        LineGet();
    }

    return TRUE;
}

void TToken::WhiteGet()
{
    Get();
    if (type == TKN_WHITESPACE) Get();
}

void TToken::LineGet()
{
    Get();
    while (type == TKN_WHITESPACE || type == TKN_RETURN)
        Get();
}

void TToken::SkipLine()
{
    while (type != TKN_RETURN && type != TKN_EOF)
        Get();
    Get();
}

BOOL TToken::SkipBlock()
{
    int skipdepth = 0;

    if (Type() == TKN_EOF)
        return FALSE;

    if (Type() == TKN_KEYWORD)
    {
        if (Code() == KEY_BEGIN)
            skipdepth++;
        else if (Code() == KEY_END)
            skipdepth--;
    }

    while (skipdepth)
    {
        Get();
        SkipBlanks();

        if (Type() == TKN_EOF)
        {
            if (skipdepth)
                return FALSE;
            else
                break;
        }

        if (Type() == TKN_KEYWORD)
        {
            if (Code() == KEY_BEGIN)
                skipdepth++;
            else if (Code() == KEY_END)
                skipdepth--;
        }

        while (Type() != TKN_RETURN && Type() != TKN_EOF)
            Get();

        if (Type() == TKN_EOF)
        {
            if (skipdepth)
                return FALSE;
            else
                break;
        }
    }

    return TRUE;
}

void TToken::SkipBlanks()
{
    while (type == TKN_WHITESPACE || type == TKN_RETURN)
        Get();
}

int abbrevcmp(char *abbrev, char *string)
// A string compare which allows abbreviation, ie abbrev = "fors" and
// string = "forsaken" would return 4.
{
    int i;

    for (i = 0; *abbrev && *string; string++, abbrev++, i++)
        if (tolower(*abbrev) != tolower(*string))
            return 0;

    if (*abbrev && *string == 0)
        return 0;

    return i;
}

BOOL TToken::Is(char *istext, int abbrevlen)
{
    if (abbrevlen)
        return (abbrevcmp(text, istext) >= abbrevlen);

    return (stricmp(istext, text) == 0);
}

void TToken::DoBegin()
{
    if (Type() != TKN_KEYWORD || Code() != KEY_BEGIN)
        Error("BEGIN Expected!");
    WhiteGet();
    if (Type() != TKN_RETURN)
        Error("RETURN Expected!");
    LineGet();
}

void TToken::DoEnd()
{
    if (Type() != TKN_KEYWORD || Code() != KEY_END)
        Error("END Expected!");
    WhiteGet();
    if (Type() != TKN_RETURN)
        Error("RETURN Expected!");
    LineGet();
}

void TToken::Error(char *err, char *extra)
{
    char buf[100];
    sprintf(buf, "Error in file %s line %d: ", stream->Name(), linenum);
    int len = strlen(buf);
    sprintf(buf + len, err, extra);
    FatalError(buf);
}

// **************************************************
// * Parse - Parses and sets variables (like scanf) *
// **************************************************

BOOL ParseAnything(BOOL stack, TToken &t, char *format, va_list ap)
{
    char *f = format;
    char keyword[41];
    int i, len;
    char *p;
    char **from = &p;

    if (!stack)
        from = va_arg(ap, char **);

    while (*f)
    {
        switch (*f)
        {

        // Spaces
          case ' ':
          case '\t':
          {
            f++;
            break;
          }

        // Continue to next line without using backslash (WHITESPACE INCLUDING RETURNS)
          case '\\':
          {
            f++;
            while (t.Type() == TKN_RETURN)
                t.LineGet();
            break;
          }

        // Return
          case '\n':
          {
            if (t.Type() != TKN_RETURN)
                return FALSE;
            f++;
            t.LineGet();
            break;
          }

        // Optional
          case '<':
          {
            f++;
            i = 0;
            while (*f && isalnum(*f))
                keyword[i++] = toupper(*f++);
            keyword[i] = NULL;
            if ((t.Type() != TKN_KEYWORD && t.Type() != TKN_IDENT)
              || stricmp(keyword, t.Text()))
            {
                if (*f == '>') // If no parameters to keyword, treat it as a boolean
                {
                    if (stack)
                        *va_arg(ap,BOOL *) = (BOOL)FALSE;
                    else
                        *((BOOL *)(*from)++) = (BOOL)FALSE;

                    f++;
                }
                else
                {
                    while (*f && *f != '>')
                    {
                        if (*f == '%')
                        {

                          // Get Len
                            f++;
                            if (isdigit(*f))
                            {
                                len = 0;
                                while (isdigit(*f))
                                    len = len * 10 + *f++ - '0';
                            } else len = 0;

                            switch (*f)
                            {
                              case 'b':
                                if (stack)
                                    *va_arg(ap,unsigned char *) = (BYTE)t.Index();
                                else
                                    *(unsigned char **)from += 1;
                                break;
                              case 'h':
                                if (stack)
                                    va_arg(ap,short *);
                                else
                                    *(short **)from += 1;
                                break;
                              case 'w':
                                if (stack)
                                {
                                    *va_arg(ap,WORD *) = (WORD)t.Index();
                                    t.WhiteGet();
                                }
                                else
                                    *(WORD **)from += 1;
                                break;
                              case 'i':
                                if (stack)
                                {
                                    *va_arg(ap,long *) = t.Index();
                                    t.WhiteGet();
                                }
                                else
                                    *(long **)from += 1;
                                break;
                              case 'd':
                                if (stack)
                                {
                                    *va_arg(ap,DWORD *) = t.Index();
                                    t.WhiteGet();
                                }
                                else
                                    *(DWORD **)from += 1;
                                break;
                              case 'f':
                                if (stack)
                                    va_arg(ap,double *);
                                else
                                    *(double **)from += 1;
                                break;
                              case 's':
                              case 't':
                                if (stack)
                                    va_arg(ap,char *);
                                else
                                    *from += len + 1;
                                break;
                              case '#':
                                if (stack)
                                    va_arg(ap,BYTE *);
                                else
                                    *from += 4;
                                break;
                              case '$':
                                if (stack)
                                    va_arg(ap,BYTE *);
                                else
                                    *from += 2;
                                break;
                            }
                        }
                        f++;
                    }
                }
            }
            else
            {
                if (*f == '>') // If no parameters to keyword, treat it as a boolean
                {
                    if (stack)
                        *va_arg(ap,BOOL *) = (BOOL)TRUE;
                    else
                    {
                        **(BOOL **)from = (BOOL)TRUE;
                        *(BOOL **)from += 1;
                    }
                    f++;
                }

                t.WhiteGet();
            }

            break;
          }

        // End Optional
          case '>':
          {
            f++; // Ignore end optional
            break;
          }

        // Value
          case '%':
          {
            f++;
            if (isdigit(*f))
            {
                len = 0;
                while (isdigit(*f))
                    len = len * 10 + *f++ - '0';
            } else len = 0;

            char *str;
            switch (*f)
            {

            // Number
              case 'b':
              case 'h':
              case 'w':
              case 'i':
              case 'd':
              case 'f':
              {
                if (t.Type() != TKN_NUMBER && t.Type() != TKN_DEFINE && t.Type() != TKN_SCENEID)
                    return FALSE;
                double n = t.Number();
                t.WhiteGet();

                DWORD d;
                if (*f != 'f')
                {
                    d = (DWORD)n;
                    while (t.Type() == TKN_SYMBOL && (t.Text())[0] == '|')
                    {
                        t.WhiteGet();
                        if (t.Type() != TKN_NUMBER && t.Type() != TKN_DEFINE && t.Type() != TKN_SCENEID)
                            return FALSE;
                        d |= (DWORD)t.Number();
                        t.WhiteGet();
                    }
                }

                switch (*f)
                {
                  case 'c':
                    if (stack)
                        *va_arg(ap,signed char *) = (signed char)d;
                    else
                    {
                        **(signed char **)from = (signed char)d;
                        *(signed char **)from += 1;
                    }
                    break;
                  case 'b':
                    if (stack)
                        *va_arg(ap,unsigned char *) = (unsigned char)d;
                    else
                    {
                        **(unsigned char **)from = (unsigned char)d;
                        *(unsigned char **)from += 1;
                    }
                    break;
                  case 'h':
                    if (stack)
                        *va_arg(ap,short *) = (short)d;
                    else
                    {
                        **(short **)from = (short)d;
                        *(short **)from += 1;
                    }
                    break;
                  case 'w':
                    if (stack)
                        *va_arg(ap,WORD *) = (WORD)d;
                    else
                    {
                        **(WORD **)from = (WORD)d;
                        *(WORD **)from += 1;
                    }
                    break;
                  case 'i':
                    if (stack)
                        *va_arg(ap,long *) = (long)d;
                    else
                    {
                        **(long **)from = (long)d;
                        *(long **)from += 1;
                    }
                    break;
                  case 'd':
                    if (stack)
                        *va_arg(ap,DWORD *) = (DWORD)d;
                    else
                    {
                        **(DWORD **)from = (DWORD)d;
                        *(DWORD **)from += 1;
                    }
                    break;
                  case 'f':
                    if (stack)
                        *va_arg(ap,double *) = n;
                    else
                    {
                        **(double **)from = n;
                        *(double **)from += 1;
                    }
                    break;
                }
                f++;
                break;
              }

              // Text
              case 't':
              {
                f++;
                if (t.Type() != TKN_IDENT && t.Type() != TKN_DEFINE && t.Type() != TKN_SCENEID)
                    return FALSE;
                if (stack)
                    str = va_arg(ap,char *);
                else
                    str = (*from);
                if (len)
                {
                    memset(str, 0, len);
                    strncpy(str, t.Text(), len - 1);
                }
                else
                {
                    strcpy(str, t.Text());
                    len = strlen(t.Text()) + 1;
                }
                if (!stack) (*from) += len;
                t.WhiteGet();
                break;
              }

              // String Constant
              case 's':
              {
                f++;
                if (t.Type() != TKN_TEXT)
                    return FALSE;
                if (stack)
                    str = va_arg(ap,char *);
                else
                    str = (*from);
                if (len)
                {
                    memset(str, 0, len);
                    strncpy(str, t.Text(), len - 1);
                }
                else
                {
                    strcpy(str, t.Text());
                    len = strlen(t.Text()) + 1;
                }
                if (!stack) (*from) += len;
                t.WhiteGet();
                break;
              }

              default:
                Error("Invalid '%' field in GetLine()");
                break;

            } // End of '%' value parse
            break;
          }

        // Keyword
          default:
            if (isalpha(*f))
            {
                i = 0;
                while (*f && isalnum(*f))
                    keyword[i++] = toupper(*f++);
                keyword[i] = NULL;
                if ((t.Type() != TKN_KEYWORD && t.Type() != TKN_IDENT)
                  || stricmp(keyword, t.Text()))
                    return FALSE;
                t.WhiteGet();
            }
            else // Default
            {
                if (t.Type() != TKN_SYMBOL || t.Code() != *f)
                {
                    f[1] = NULL;
                    return FALSE;
                }
                f++;
                t.WhiteGet();
            }
            break;
        }
    }

    return TRUE;
}

BOOL __cdecl Parse(TToken &t, char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    BOOL retval = ParseAnything(TRUE, t, format, ap);
    va_end(ap);
    return retval;
}

BOOL __cdecl ParseString(char *string, char *format, ...)
{
    TStringParseStream s(string);
    TToken t(s);

    va_list ap;
    va_start(ap, format);
    BOOL retval = ParseAnything(TRUE, t, format, ap);
    va_end(ap);
    return retval;
}
    
