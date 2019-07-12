// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              animation.h - Animation Definition File                  *
// *************************************************************************

#ifndef _ANIMATION_H
#define _ANIMATION_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _ANIMDATA_H
#include "animdata.h"
#endif

class TAnimation : public TAnimationData
{
  public:

    TAnimation();
    static PTAnimation Load(int resource);
      // Loads animation resource
    int  GetDecBufSize(int frame) {return frames[frame].decbufsize;}
    BOOL Decompress(PSDrawBlock db, int frame, BOOL Transparent);
      // Decompresses frame to surface
    void Put(PTSurface surface, void *decbuf, int frame, PSDrawParam dp);
      // Draws a frame to a surface
    void Stretch(PTSurface surface, PTSurface decbuf, int x, int y, int size, int frame);
      // Stretches a frame of animation to a surface
    PTBitmap GetFrame(int frame);
      // Returns bitmap for the given frame
    int NumFrames();
      // Returns number of frames in the animation
    ~TAnimation();

  private:
    void Move(int &x, int &y, int frame, int orientation = NULL);
      // Returns new X/Y position based on frame dx/dy.
    void SizeMove(int &x, int &y, int size, int frame, int orientation = NULL);
      // Returns new X/Y position based on frame dx/dy. Scales movement based on size.
};

#endif
