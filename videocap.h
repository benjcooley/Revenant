// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  videocap.h - Video Capture Class                     *
// *************************************************************************

#ifndef _VIDEOCAP_H
#define _VIDEOCAP_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _GRAPHICS_H
#include "graphics.h"
#endif

#ifndef _BITMAP_H
#include "bitmap.h"
#endif

#ifndef _BMSURFACE_H
#include "bmsurface.h"
#endif

_CLASSDEF(TVideoCapture)
class TVideoCapture
{
  public:
    BOOL Initialize(int bufmegs = 64, int fps = FRAMERATE);
      // Initializes the capture system (allocates buffer of frame * framerate * secs.. 
      // this buffer can be HUGE, so make sure 'secs' value is reasonable)
    void Close();
      // Closes the capture object and deletes all buffers
    void Start();
      // Start a capture session
    void Stop();
      // Stop capturing video
    void SaveFrame();
      // Called by system to save the current video frame
    void Flush();
      // Flushes the current video buffers
    BOOL IsCapturing() { return saving; }
      // Returns TRUE if the capture system is setup to capture
      
  private:
    int framespersecond, seconds;
    BOOL saving;
    BYTE *buffer;
    TBitmapSurface **bmsurface;
    int totframes;
    int frame;
    int bufframe;
    int bufsize;
    int framesize;
    int frameaccum;
    int frameadd;
};

#endif

