// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       timer.h - Timer objects                         *
// *************************************************************************

#ifndef _TIMER_H
#define _TIMER_H

#include <windows.h>
#include <mmsystem.h>

#ifndef _REVENANT_H
#include "revenant.h"
#endif

// ***************************************
// * TTimer Class Varible/Function List *
// ***************************************

_CLASSDEF(TTimer)

class TTimer
{
  static DWORD    clockticks;   // Timer clock tick count
  MMRESULT        timerID;      // Stores timerID 
  
  // Public TBitmap Functions
  public:
    TTimer();
    ~TTimer();

    BOOL Initialize();
      // Initializes Timer
    void Close();
      // Releases Timer

    int  ClockTicks() {return clockticks;}
      // Return current clocktick count
    static void PASCAL TimerCallback(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);
      // Timer CallBack Routine
    void WaitForTick();
      // Checks if tick has occured, if not, waits for one
    void ResetTick();
      // Resets the tick occured flag so we can wait for another tick.  WaitForTick will
      // immediately return until this is called
};

#endif
