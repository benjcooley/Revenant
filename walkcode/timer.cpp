// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   timer.cpp - Timer oject module                      *  
// *************************************************************************

#include <windows.h>
#include <mmsystem.h>

#include "revenant.h"
#include "screen.h"
#include "timer.h"

DWORD TTimer::clockticks;
DWORD period = (DWORD)(1000 / FRAMERATE);
HANDLE tickevent;

void WINAPI TTimer::TimerCallback(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    clockticks++;
    TickOccured = TRUE;
    PulseEvent(tickevent);
}

TTimer::TTimer()
{
    clockticks = 0;
    timerID = NULL;
}

TTimer::~TTimer()
{
    if (timerID) 
    {
        timeEndPeriod(period);
        DWORD Result = timeKillEvent(timerID);
    }
}

BOOL TTimer::Initialize()
{
    TIMECAPS tc;
    DWORD period = 1000 / FRAMERATE;

    tickevent = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (timerID) return TRUE;

    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
        return FALSE;

    period = min(max(tc.wPeriodMin, period), tc.wPeriodMax);

    timeBeginPeriod(period);

    timerID = timeSetEvent(period, RESOLUTION, TimerCallback, NULL, TIME_PERIODIC);

    if (!timerID) return FALSE;

    return TRUE;
}

void TTimer::Close()
{
    if (timerID == NULL) 
        return;
    
    timeEndPeriod(period);
    timeKillEvent(timerID);
    timerID = NULL;
    CloseHandle(tickevent);
}
  
void TTimer::WaitForTick()
{
    if (TickOccured)
        return;

    WaitForSingleObject(tickevent, INFINITE);
}

void TTimer::ResetTick()
{
    TickOccured = FALSE;
}
