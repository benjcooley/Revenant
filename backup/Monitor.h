// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    monitor.h  - Monitor globals                       *
// *************************************************************************

#ifndef _MONITOR_H
#define _MONITOR_H

#ifndef SM_CMONITORS
#include "multimon.h"
#endif

// Multi-Monitor Variables
extern int MonitorNum;          // Monitor game will run on (default is 1, primary)
extern int MonitorX, MonitorY;  // Relative position of monitor in desktop coordinates
extern int MonitorW, MonitorH;  // Width and height of monitor (before changing video modes)
extern HMONITOR Monitor;        // Windows monitor handle
extern MONITORINFOEX MonitorInfo; // Windows monitor info structure
                                   // i.e. use if string is "permidia" and driver desc is "Glint Permidia 2 3D"
#endif
