// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 videocap.cpp - Video capture module                   *
// *************************************************************************

#include <windows.h>

#include "revenant.h"
#include "graphics.h"
#include "bitmap.h"
#include "bmsurface.h"
#include "display.h"
#include "videocap.h"

// ***********************************
// * Video Sequence Saving Functions *
// ***********************************


BOOL TVideoCapture::Initialize(int bufmegs, int fps)
{
    if (buffer)
        return TRUE;

    framespersecond = fps;

    framesize = sizeof(TBitmap) + WIDTH * HEIGHT * 2 - 4;
    totframes = (bufmegs * 1024 * 1024) / framesize;
    bufsize =  framesize * totframes;
    seconds = totframes / framespersecond;

    buffer = (BYTE *)malloc(bufsize);
    VirtualLock(buffer, bufsize);

    BYTE *ptr = buffer;
    bmsurface = new PTBitmapSurface[totframes];
    for (int c = 0; c < totframes; c++, ptr += framesize)
    {
        memset(ptr, 0, sizeof(TBitmap) - 4);
        PTBitmap bm = (PTBitmap)ptr;
        bm->width = WIDTH;
        bm->height = HEIGHT;
        bm->flags = (Display->BitsPerPixel() == 15) ? BM_15BIT : BM_16BIT;
        bm->datasize = WIDTH * HEIGHT * 2;
        bmsurface[c] = new TBitmapSurface(bm);
    }

    frame = 0;
    bufframe = 0;
    frameaccum = 0;
    frameadd = (framespersecond << 16) / FRAMERATE;
    saving = FALSE;

    return TRUE;
}   

void TVideoCapture::Close()
{
    if (!buffer)
        return;

    Stop();

    VirtualUnlock(buffer, bufsize);
    free(buffer);
    buffer = NULL;
    for (int c = 0; c < totframes; c++)
    {
        delete bmsurface[c];
    }
    delete bmsurface;
    bmsurface = NULL;
}

void TVideoCapture::Start()
{
    if (!buffer)
        return;

    frame = 0;
    bufframe = 0;
    frameaccum = 0x10000;
    frameadd = (framespersecond << 16) / FRAMERATE;
    saving = TRUE;
}   

void TVideoCapture::Stop()
{
    if (!buffer || !saving)
        return;

    Flush();
    saving = FALSE;
}

void TVideoCapture::SaveFrame()
{
    if (!buffer || !saving)
        return;

  // Do framerate stuff
    frameaccum += frameadd;
    if (frameaccum < 0x10000)
        return;
    frameaccum = frameaccum & 0xFFFF;

  // If buffer full, flush it
    if (bufframe >= totframes)
        Flush();

  // Save eo buffer
    Display->Reset();
    bmsurface[bufframe]->Blit(0, 0, Display);
    bufframe++;
}

void TVideoCapture::Flush()
{
    if (!buffer || !saving)
        return;

    char buf[80];

    for (int c = 0; c < bufframe; c++)
    {
        sprintf(buf, "video%03d.bmp", frame + c);
        bmsurface[c]->GetBitmap()->SaveBMP(buf);
    }

    frame += bufframe;
    bufframe = 0;
}




            





