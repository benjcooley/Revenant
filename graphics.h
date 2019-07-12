// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   graphics.h - Graphics Routines                      *
// *************************************************************************

#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

// ************************
// * Draw Block Structure *
// ************************

// Draw Block Structure is passed to each routine with the appropriate
// variables filled in.

_STRUCTDEF(SDrawBlock)
_STRUCTDEF(SDrawParam)

typedef BOOL (*DRAWFUNCTION)(PSDrawBlock, PSDrawParam);
  // Defines a standard draw function type
typedef void (*DRAWCALLBACK)(PSDrawBlock db, PSDrawParam dp);
  // Defines a callback function which is called for each draw rectangle

struct SDrawBlock
{
    DWORD srcbitmapflags;           // Bitmap Flags
    DWORD dstbitmapflags;           // Bitmap Flags
    void  *dest;                    // Pointer to Dest, Dest Z, and Dest Normal buffers
    WORD  *dzbuffer;
    WORD  *dnormals;
    int   dbufwidth;                // Dest buffer width
    int   dbufheight;               // Dest buffer height
    int   dstride;                  // Dest stride
    int   dzstride;                 // Dest zbuffer stride
//  int   dnstride;                 // Dest normal stride
    void  *source;                  // Pointer to Src, Src Z, and Src Normal buffers
    WORD  *szbuffer;
    WORD  *snormals;
    int   sbufwidth;                // Src buffer width
    int   sbufheight;               // Src buffer height
    int   sstride;                  // Src stride
    int   szstride;                 // Src zbuffer stride
//  int   snstride;                 // Src normal stride

    WORD  *palette;                 // Palette
    BYTE  *alpha;                   // Alpha buffer pointer
    BYTE  *alias;                   // Alpha buffer pointer

    DWORD keycolor;                 // Color to use as transparent
    
    BOOL operator= (PSDrawBlock drawblock)
        { memcpy(drawblock, this, sizeof(this)); return TRUE; };
      // Redefines '=' to allow assigning strings.
};

struct SDrawParam
{
    DWORD drawmode;                 // Drawing mode. See Exiledef.h for description
    DRAWFUNCTION func;              // Drawing function

    DRAWCALLBACK callback;          // Drawing callback function (for adding dirty rectangles, etc.)    

    void *data;                     // Other data (such as text for draw tex function)

    int   originx;                  // Upper left of drawing region
    int   originy;

    int   clipx;                    // Clipping rectangle 
    int   clipy;
    int   clipwidth;
    int   clipheight;
                                    
    int   dx;                       // Rectangle inside dest buffer to use
    int   dy;                       
    int   dwidth;   
    int   dheight;  

    int   sx;                       // Rectangle inside src buffer to use
    int   sy;   
    int   swidth;   
    int   sheight;  
    
    DWORD color;                    // color used to draw image
    DWORD intensity;                // intensity of translucent/alpha draw

    WORD  zpos;                     // Z position of image
    WORD  normal;                   // Normal position for image

    BOOL operator= (PSDrawParam drawparam)
        { memcpy(drawparam, this, sizeof(this)); return TRUE; };
      // Redefines '=' to allow assigning strings.
};

_STRUCTDEF(SColor)

struct SColor
{
    BYTE red;
    BYTE green;
    BYTE blue;
};

// Data pointer points to this when drawing lines
_STRUCTDEF(SLineParam)
struct SLineParam
{
    int x1, y1, x2, y2;
};

// Data pointer points to this when drawing text
_STRUCTDEF(STextParam)
struct STextParam
{
    int startline;
    int numlines;
    char *text;
    TFont *font;
    int wrapwidth;
    int justify;
    BOOL draw;
    int length;
    BOOL noclip;
    int linespace;
};
    
// ******************************
// * Low Level Graphic Routines *
// ******************************

// All routines return FALSE if Dest. Buffer is Invalid.

BOOL Draw(PSDrawBlock db, PSDrawParam dp);
  // General drawing function. Clips and calls the 'func' function in the drawparam
  // structure, then calls the 'callback' function in the drawparam structure.  If
  // 'func' is NULL, looks up the correct draw function by calling GetPutFunction().
 
DRAWFUNCTION GetPutFunction(PSDrawBlock db, PSDrawParam dp);
  // Returns the pointer to the appropriate put function given the current drawmode state

BOOL ClipRect(RSRect dst, RSRect src, RSRect result);
  // General purpose function to clip src rectangle in dest.
  // Returns FALSE if rectangles don't overlap.  Src and result can be same rect

inline BOOL ClipRect(int x, int y, int w, int h, RSRect src, RSRect result)
  { return ClipRect(SRect(x, y, x + w - 1, y + h - 1), src, result); }
  // General purpose function to clip src rectangle with dest rect.
  // Returns FALSE if rectangles don't overlap.  Src and result can be same rect

BOOL SubtractRect(RSRect dst, RSRect src, PSRect rects, int &numrects);
  // Subtracts src rect from dest and returns results in 'rects'.  Returns
  // false if rects don't intersect.  Returns a maximum of 4 rects.
  // The Subtract rect function basically calculates an inverse of the rect intersection

inline BOOL SubtractRect(int x, int y, int w, int h, RSRect src, PSRect rects, int &numrects)
  { return SubtractRect(SRect(x, y, x + w - 1, y + h - 1), src, rects, numrects); }
  // Subtracts src rect from dest and returns results in 'rects'.  Returns
  // false if rects don't intersect.  Returns a maximum of 4 rects.
  // The Subtract rect function basically calculates an inverse of the rect intersection

BOOL Clip(PSDrawBlock db, PSDrawParam dp, PSDrawParam dparray, 
    int &numrects);
  // Clipping Routine..  Takes one draw rectangle, clips it, and optionally wraps clips it to 4 other rects.

BOOL StretchClip(PSDrawBlock db, PSDrawParam dp, PSDrawParam dparray, 
    int &numrects);
  // Stretch Clipping Routine.

// Put Routines
BOOL Put(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine.
BOOL Put8(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine for 8 bit bitmaps. Draws to 32 bit surface
BOOL Put88(PSDrawBlock db, PSDrawParam dp); 
  // Draws 8 bit bitmap to 8 bit surface.
BOOL Put816(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine for 8 bit bitmaps. Draws to 16 bit surface.
BOOL Put32(PSDrawBlock db, PSDrawParam dp);
  // 32 bit Draw Routine.

// Shutter Put Routines
BOOL ShutterPut(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine.
BOOL ShutterPut8(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine for 8 bit bitmaps. Draws to 32 bit surface
BOOL ShutterPut816(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine for 8 bit bitmaps. Draws to 16 bit surface.
BOOL ShutterPut32(PSDrawBlock db, PSDrawParam dp);
  // 32 bit Draw Routine.

BOOL PutChunk8(PSDrawBlock db, PSDrawParam dp);
  // Draw Routine for 8 bit bitmaps. Draws to 32 bit surface. Handles Z and Normal Internally
BOOL TransPut(PSDrawBlock db, PSDrawParam dp);
  // Normal Transparent Draw Routine.
BOOL TransPut8(PSDrawBlock db, PSDrawParam dp);
  // Normal Transparent Draw Routine for 8 bit bitmap. Draws to 32 bit surface
BOOL TransPut816(PSDrawBlock db, PSDrawParam dp);   
  // Normal Transparent Draw Routine for 8 bit bitmap. Draws to 16 bit surface
BOOL TransPut88(PSDrawBlock db, PSDrawParam dp);    
  // Normal Transparent Draw Routine for 8 bit bitmap. Draws to 8 bit surface
BOOL TransPutKey(PSDrawBlock db, PSDrawParam dp);
  // Normal Transparent Draw Routine with nonzero keycolor.
BOOL TransPutColor(PSDrawBlock db, PSDrawParam dp);
  // Normal Transparent Draw Routine in given color.
BOOL PutHueChange(PSDrawBlock db, PSDrawParam dp);
  // Draw routine with hue change to the given color.
BOOL TransPutSVChange(PSDrawBlock db, PSDrawParam dp);
  // Transparent draw routine with saturation/brightness change to the given color.
BOOL TransPut32(PSDrawBlock db, PSDrawParam dp);
  // 32 bit Transparent Draw Routine.

BOOL Stretch(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine.
BOOL TransStretch(PSDrawBlock db, PSDrawParam dp);
  // Normal Transparent Draw Routine.

BOOL Mask(PSDrawBlock db, PSDrawParam dp);
  // Normal Masked Draw Routine.
BOOL MaskStretch(PSDrawBlock db, PSDrawParam dp);
  // Normal Masked Draw Routine.

BOOL Translucent(PSDrawBlock db, PSDrawParam dp);
  // Draws bitmap translucently to screen
BOOL TransTranslucent(PSDrawBlock db, PSDrawParam dp);
  // Draws bitmap transparently and translucently to screen
BOOL TranslucentStretch(PSDrawBlock db, PSDrawParam dp);
  // Draws bitmap translucently stretched to screen

BOOL AlphaLighten(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info. Lighten only.
BOOL Alpha(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info.
BOOL Alpha8(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info. Converts 8 bit bitmap to 16 bit.
BOOL AlphaZ(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info and ZBuffer.
BOOL AlphaZ8(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info and ZBuffer. Converts 8 bit bitmap to 16 bit
BOOL AlphaDim(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info, dimming the alpha by dp->color.
BOOL AlphaDimZ(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info, dimming the alpha by dp->color.
BOOL Alpha32(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using alpha channel info for 32 bit buffers.
BOOL AlphaDimZNoBitmap(PSDrawBlock db, PSDrawParam dp);
  // Draws an alpha dimming z image with no bitmap data (single color)

BOOL Alias(PSDrawBlock db, PSDrawParam dp);
  // Draws an aliased edge around graphics.
BOOL AliasColor(PSDrawBlock db, PSDrawParam dp);
  // Draws an aliased edge around graphics with set color.
BOOL Alias32(PSDrawBlock db, PSDrawParam dp);
  // Draws an aliased edge around graphics for 32 bit buffers.

BOOL DrawSelected8(PSDrawBlock db, PSDrawParam dp);
  // Draw a selection highlight around bitmap's edges for 8 bit bitmaps.
BOOL DrawSelected(PSDrawBlock db, PSDrawParam dp);
  // Draw a selection highlight around bitmap's edges.

// Z Buffer Put routines
BOOL ZPut(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using zbuffer to control drawing
BOOL ZPut8(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing Draws to 16 bit surface
BOOL ZPut816(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing. Draws to 16 bit surface.
BOOL ZPut32(PSDrawBlock db, PSDrawParam dp);
  // Draws to a 32 bit bitmap using zbuffer to control drawing

// Z Buffer Put Shutter routines
BOOL ShutterZPut(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using zbuffer to control drawing
BOOL ShutterZPut8(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing Draws to 16 bit surface
BOOL ShutterZPut816(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing. Draws to 16 bit surface.
BOOL ShutterZPut32(PSDrawBlock db, PSDrawParam dp);
  // Draws to a 32 bit bitmap using zbuffer to control drawing

// Z Buffer Normal Put routines
BOOL ZNormalPut(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using zbuffer to control drawing, also transfer normal info.
BOOL ZNormalPut8(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing, also transfer normal info.
  // Draws to 32 bit surface
BOOL ZNormalPut816(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing, also transfer normal info.
  // Draws to 16 bit surface
BOOL ZNormalPut32(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using zbuffer to control drawing, also transfer normal info.

// Z Buffer Normal Put Shutter routines
BOOL ShutterZNormalPut(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using zbuffer to control drawing, also transfer normal info.
BOOL ShutterZNormalPut8(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing, also transfer normal info.
  // Draws to 32 bit surface
BOOL ShutterZNormalPut816(PSDrawBlock db, PSDrawParam dp);
  // Draws an 8 bit bitmap using zbuffer to control drawing, also transfer normal info.
  // Draws to 16 bit surface
BOOL ShutterZNormalPut32(PSDrawBlock db, PSDrawParam dp);
  // Draws a bitmap using zbuffer to control drawing, also transfer normal info.

BOOL TransZStaticPut8(PSDrawBlock db, PSDrawParam dp);
  // Draw an 8-bit bitmap to a 32-bit surface at a static z position, also drawing zbuffer info
BOOL TransZStaticPut816(PSDrawBlock db, PSDrawParam dp);
  // Draw an 8-bit bitmap to a 16-bit surfaceat a static z position, also drawing zbuffer info

BOOL ZFind(PSDrawBlock db, PSDrawParam dp);
  // Return the first uncliped zbuffer point if it is highest in zbuf

BOOL ZStretch(PSDrawBlock db, PSDrawParam dp);
  // Normal Draw Routine.
BOOL ZMaskStretch(PSDrawBlock db, PSDrawParam dp);
  // Normal Masked Draw Routine.

BOOL Box(PSDrawBlock db, PSDrawParam dp);
  // Fill the area specified by db and dp

BOOL Convert15to16(PTBitmapData bitmap);
  // Convert 15/24 bit buffer to 16 bit buffer.
BOOL ConvertPal15to16(PTBitmapData bitmap);
  // Convert a 15 bit palette to 16 bit

BOOL Convert16to15(PTBitmapData bitmap);
  // Convert 16/24 bit buffer to 15 bit buffer.
BOOL ConvertPal16to15(PTBitmapData bitmap);
  // Convert a 16 bit palette to 15 bit

BOOL TextDraw(PSDrawBlock db, PSDrawParam dp);
  // Write text in the given font, transparent and aliased, with wordwrapping if desired.
  // Returns number of lines of text generated in 'length' data parameter.  See SLineParam
  // structure for params to set in the data portion of 'dp'

BOOL LineDraw(PSDrawBlock db, PSDrawParam dp);
  // Draws line in either 15/16 or 24 bit color.
  // db->dstbitmapflags determine which to draw in.  See SLineParam stucture for
  // parameter data to use for dp->data.

WORD ZFindChunk(PSDrawBlock db, PSDrawParam dp);
  // Return the first uncliped zbuffer point in a chunked bitmap

inline void SetupZDraw(int *bmwidth, int *bmheight, int *srcoff, int *srcadd, int *srczoff, int *srczadd,
    int *dstoff, int *dstadd, int *dstzoff, int *dstzadd, PSDrawBlock db, PSDrawParam dp)
{
    int srcbytes = 2;
    if (db->srcbitmapflags & BM_8BIT) srcbytes = 1;
    else if (db->srcbitmapflags & (BM_15BIT | BM_16BIT)) srcbytes = 2;
    else if (db->srcbitmapflags & BM_24BIT) srcbytes = 3;
    else if (db->srcbitmapflags & BM_32BIT) srcbytes = 4;

    int dstbytes = 2;
    if (db->dstbitmapflags & BM_8BIT) dstbytes = 1;
    else if (db->dstbitmapflags & (BM_15BIT | BM_16BIT)) dstbytes = 2;
    else if (db->dstbitmapflags & BM_24BIT) dstbytes = 3;
    else if (db->dstbitmapflags & BM_32BIT) dstbytes = 4;

    // Get offsets/add stuff
    *dstoff  = (dp->dy + dp->originy) * db->dstride + dp->dx + dp->originx;
    *dstadd  = db->dstride - dp->dwidth;
        
    *dstzoff  = (dp->dy + dp->originy) * db->dzstride + dp->dx + dp->originx;
    *dstzadd  = db->dzstride - dp->dwidth;

    if (dp->drawmode & DM_REVERSEVERT)
    {
        *srcoff = (dp->sy + dp->sheight - 1) * db->sstride + dp->sx;
        *srcadd = -db->sstride + dp->swidth;
        *srczoff = (dp->sy + dp->sheight - 1) * db->szstride + dp->sx;
        *srczadd = -db->szstride + dp->swidth;
    }
    else
    {
        *srcoff = dp->sy * db->sstride + dp->sx;
        *srcadd = db->sstride - dp->swidth;
        *srczoff = dp->sy * db->szstride + dp->sx;
        *srczadd = db->szstride - dp->swidth;
    }
    
    *bmwidth  = dp->swidth * srcbytes;
    *bmheight = dp->sheight;

    *dstoff = *dstoff * dstbytes;
    *dstadd = *dstadd * dstbytes;
    *dstzoff = *dstzoff * 2;
    *dstzadd = *dstzadd * 2;

    *srcoff = *srcoff * srcbytes;
    *srcadd = *srcadd * srcbytes;
    *srczoff = *srczoff * 2;
    *srczadd = *srczadd * 2;
}

inline void SetupDraw(int *bmwidth, int *bmheight, int *srcoff, int *srcadd,
    int *dstoff, int *dstadd, PSDrawBlock db, PSDrawParam dp)
{
    int srcbytes = 2;
    if (db->srcbitmapflags & BM_8BIT) srcbytes = 1;
    else if (db->srcbitmapflags & (BM_15BIT | BM_16BIT)) srcbytes = 2;
    else if (db->srcbitmapflags & BM_24BIT) srcbytes = 3;
    else if (db->srcbitmapflags & BM_32BIT) srcbytes = 4;

    int dstbytes = 2;
    if (db->dstbitmapflags & BM_8BIT) dstbytes = 1;
    else if (db->dstbitmapflags & (BM_15BIT | BM_16BIT)) dstbytes = 2;
    else if (db->dstbitmapflags & BM_24BIT) dstbytes = 3;
    else if (db->dstbitmapflags & BM_32BIT) dstbytes = 4;

    // Get offsets/add stuff
    *dstoff  = (dp->dy + dp->originy) * db->dstride + dp->dx + dp->originx;
    *dstadd  = db->dstride - dp->dwidth;
        
    if (dp->drawmode & DM_REVERSEVERT)
    {
        *srcoff = (dp->sy + dp->sheight - 1) * db->sstride + dp->sx;
        *srcadd = -db->sstride + dp->swidth;
    }
    else
    {
        *srcoff = dp->sy * db->sstride + dp->sx;
        *srcadd = db->sstride - dp->swidth;
    }
    
    *bmwidth  = dp->swidth * srcbytes;
    *bmheight = dp->sheight;

    *dstoff = *dstoff * dstbytes;
    *dstadd = *dstadd * dstbytes;

    *srcoff = *srcoff * srcbytes;
    *srcadd = *srcadd * srcbytes;
}

#define SETUP_DRAW\
    int bmheight, bmwidth, srcoff, srcadd, dstoff, dstadd;\
    SetupDraw(&bmwidth, &bmheight, &srcoff, &srcadd, &dstoff, &dstadd, db, dp);

#define SETUP_Z_DRAW\
    int bmheight, bmwidth, srcoff, srcadd, srczoff, srczadd, dstoff, dstadd, dstzoff, dstzadd;\
    SetupZDraw(&bmwidth, &bmheight, &srcoff, &srcadd, &srczoff, &srczadd, &dstoff, &dstadd, &dstzoff, &dstzadd, db, dp);

// Fills out default draw param struct  
inline void MakeDP(SDrawParam &dp, 
    int x, int y, int sx, int sy, int swidth, int sheight, DWORD drawmode)
{
    dp.drawmode  = drawmode;
    dp.func      = NULL;
    dp.callback  = NULL;
    dp.data      = NULL;
    dp.dx        = x;
    dp.dy        = y;
    dp.dwidth    = swidth;
    dp.dheight   = sheight;
    dp.sx        = sx;
    dp.sy        = sy;
    dp.swidth    = swidth;
    dp.sheight   = sheight;
    dp.zpos      = dp.normal = 0;
    dp.clipx     = dp.clipy = dp.clipwidth = dp.clipheight = dp.originx = dp.originy = 0;
    dp.intensity = 31; dp.color = 0;
}

inline void MakeDPNoSrc(SDrawParam &dp, int x, int y, int width, int height, DWORD drawmode)
{
    MakeDP(dp, x, y, 0, 0, 0, 0, drawmode);
    dp.dwidth = width;
    dp.dheight = height;
}

#endif
