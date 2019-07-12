// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   dialog.h - TDialogPane module                       *
// *************************************************************************

#ifndef _DIALOG_H
#define _DIALOG_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _SCREEN_H
#include "screen.h"
#endif

// ****************************************************************************
// * TDialogList - Stores language specific dialog and message lines for game *
// ****************************************************************************

_STRUCTDEF(SDialogLine)
struct SDialogLine
{
    char *tag;
    char *line;
    SDialogLine() { tag = line = NULL; }
    ~SDialogLine() { if (tag) free(tag); if (line) free(line); }
    void Set(char *t, char *l) { tag = strdup(t); line = strdup(l); }
};
typedef TPointerArray<SDialogLine, 64, 64> TDialogLineArray;

class TDialogList
{
  public:
    BOOL Initialize();
      // Loads lines from LANGUAGE.DEF file (i.e. ENGLISH.DEF for english)
    void Close();
      // Closes dialog file
    int FindLine(char *tag);
      // Finds the dialog line for the given tag and returns id
    char *GetLine(int id);
      // Finds the dialog line for the given id and returns it
    char *GetTag(int id);
      // Finds the dialog line for the given id and returns it
    char *GetLine(char *tag);
      // Finds the dialog line for the given tag and returns it

  private:
    TDialogLineArray lines;         // Dialog line list
};

// ************************************************************
// * TDialogPane - Shows the dialog options for the character *
// ************************************************************

#define MAXCHOICES      4

// Global function for translating dialog lines

void SetDialogContext(PTObjectInstance context); // The script context
char *DialogLine(char *line, char *buf, int buflen);

// Dialog pane, for interacting with NPCs in conversation

_CLASSDEF(TDialogPane)
class TDialogPane : public TPane
{
  public:
    TDialogPane() : TPane(0, INVENTORYPANEY - 6, 404, 97, TRUE) { character = NULL; }
    
    virtual BOOL Initialize();
    virtual void Close();
    virtual void Show();
    virtual void Hide();
    virtual void DrawBackground();
    virtual void Animate(BOOL draw);
    virtual void KeyPress(int key, BOOL down);
    virtual void MouseClick(int button, int x, int y);
    virtual void MouseMove(int button, int x, int y);

    void SetCharacter(PTCharacter inst) { character = inst; }
        // Set the character that appears in the pane
    PTCharacter GetCharacter() { return character; }
        // Get the current character
    void AddChoice(char *lab, char *tag);
        // Add a dialog choice for the player to choose
    void SetChoice(int c) { choice = c; freshresponse = TRUE; highlighted = TRUE; SetDirty(TRUE); }
        // Set a choice as selected
    void Skip();
        // Skip current say command

    BOOL HasResponded() { return freshresponse; }
    char *GetResponseLabel() { if (choice >= 0) return label[choice]; else return NULL; }
    char *GetResponse() { if (choice >= 0) return choices[choice]; else return NULL; }
    void ResetResponses();

  protected:
    int OnSlot(int x, int y);
        // Find which dialog choice x, y is over

    PTMulti dialogdata;             // Background and misc
    PTCharacter character;          // Character doing the blabing

    char *choices[MAXCHOICES];      // Choice text
    char *label[MAXCHOICES];        // Label for each choice
    int numchoices;                 // Number of current choices
    int choice;                     // Their choice - update next frame
    BOOL freshresponse;             // Whether response has been handled yet

    int grabslot;                   // Slot they clicked on
    BOOL highlighted;               // For mouse stuff
};

#endif