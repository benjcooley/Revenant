// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 automap.cpp  - EXILE Automapper File                  *
// *************************************************************************

#include "revenant.h"
#include "bitmap.h"
#include "mappane.h"
#include "automap.h"
#include "display.h"
#include "playscreen.h"
#include "multi.h"
#include "cursor.h"
#include "player.h"

#define FILTER_SIZE 5
#define FILTER_PERCENT 90
#define FILTER_CUTOFF FILTER_SIZE * FILTER_SIZE * FILTER_PERCENT / 100


extern PTBitmap PointerCursor;
extern PTBitmap HandCursor;
extern BYTE  ColorTableUpperHi[32 * 256];
extern BYTE  ColorTableUpperLo[32 * 256];
extern BYTE  ColorTableLower[32 * 256];


//============================================================================
// Function : Initialize.
//----------------------------------------------------------------------------
//     Desc : This will load all the maps needed and create a bitmap to
//            draw the map to be shown.
//
//  Returns : Returns TRUE if successful, FALSE if not.
//
//============================================================================

BOOL TAutoMap::Initialize()
{
    TPane::Initialize();

    BOOL Success = FALSE;

    // Flag that there is no current map
    CurrentMap = 0xFFFF;

    // Set the position variables
    ScrollX = ScrollY = 0;
    OldPlayerX = OldPlayerY = 0xFFFF;
    OldScrollX = OldScrollY = 0xFFFF;
    DraggingMap = FALSE;
    ScrollToTarget = FALSE;

    // Initialize all the pointers for the graphics
    MapGrfx = NULL;
    DisplayMap = NULL;
    ActiveBuf = NULL;

    // Load the map list
    MapList = (PTAutoMapList)LoadResource("AutoMap.DAT", -1);

    // Make sure the map list loaded correctly
    if (MapList != NULL)
    {
        // Create the arrays to hold which colors are Active in the MaskMaps
        ActiveBuf = (BYTE *)calloc(MapList->NumMaps, 256);
        if (ActiveBuf != NULL)
        {
            // Read the Active information from the saved game

            Success = TRUE;
        }
    }

    SetDirty(TRUE);

    return Success;
}



//============================================================================
// Function : Close.
//----------------------------------------------------------------------------
//     Desc : This will perform any cleanup needed.
//
//============================================================================

void TAutoMap::Close()
{
    free(MapList);
    MapList = NULL;

    free(ActiveBuf);
    ActiveBuf = NULL;

    FreeMapGraphics();
}



//============================================================================
// Function : DrawBackground.
//----------------------------------------------------------------------------
//     Desc : This will draw the current set of maps merged appropriately
//            showning the discovered areas pulled from the aftermap on the
//            beforemap background.
//
//============================================================================

void TAutoMap::DrawBackground()
{
    int d;
    int PaneWidth = GetWidth();
    int PaneHeight = GetHeight();
    int PlayerX;
    int PlayerY;
    BOOL Update = FALSE;

    if (!DisplayMap)
        return;

    // Make sure that the maps are loaded
    if (MapGrfx == NULL)
        CheckMaps();

    GetPlayerOnAutoMap(PlayerX, PlayerY);

    // See if the map is automatically scrolling to show the player
    if (ScrollToTarget)
    {
        d = abs(TargetScrollX - ScrollX);
        if (d < abs(MapScrollDx))
            ScrollX = TargetScrollX;
        else
            ScrollX += MapScrollDx;

        d = abs(TargetScrollY - ScrollY);
        if (d < abs(MapScrollDy))
            ScrollY = TargetScrollY;
        else
            ScrollY += MapScrollDy;

        // See if we've reached the target scroll position
        if ((ScrollX == TargetScrollX) && (ScrollY == TargetScrollY))
            ScrollToTarget = FALSE;
    }

    // Make sure we don't scroll negative
    if (ScrollX < 0)
        ScrollX = 0;
    if (ScrollY < 0)
        ScrollY = 0;

    // Make sure we don't scroll past the right / bottom of the map
    if (ScrollX + PaneWidth > DisplayMap->width)
        ScrollX = DisplayMap->width - PaneWidth;
    if (ScrollY + PaneHeight > DisplayMap->height)
        ScrollY = DisplayMap->height - PaneHeight;

    // Check if the entire pane is dirty or if the player has moved
    if ((IsDirty()) || (PlayerX != OldPlayerX) || (PlayerY != OldPlayerY))
    {
        // Restore the pixel where he was
        RestorePlayerMarker();

        // Draw the Marker where the player is
        DrawPlayerMarker(PlayerX, PlayerY);
        Update = TRUE;
    }
    else
    // Check if the pane has scrolled
    if ((ScrollX != OldScrollX) || (ScrollY != OldScrollY))
        Update = TRUE;

    // See if we need to update the pane to the screen
    if (Update)
    {
        Display->Put(0, 0, DisplayMap, ScrollX, ScrollY, DisplayMap->width, DisplayMap->height, DM_BACKGROUND);
        OldScrollX = ScrollX;
        OldScrollY = ScrollY;
        OldPlayerX = PlayerX;
        OldPlayerY = PlayerY;
        SetDirty(FALSE);
        PlayScreen.MultiUpdate();
    }
}



//============================================================================
// Function : DrawPlayerMarker.
//----------------------------------------------------------------------------
//     Desc : This will draw the marker on the DisplayMap signifying where
//            the player is on the map. This will also save the area under
//            the players marker so it can be restored by RestorePlayerMarker.
//
//    Entry : PlayerX = x coordinate of the player relative to DisplayMap
//            PlayerY = y coordinate of the player relative to DisplayMap
//
//============================================================================

void TAutoMap::DrawPlayerMarker(int PlayerX, int PlayerY)
{
    int Offset = PlayerX + PlayerY * DisplayMap->width;

    MarkerX = PlayerX;
    MarkerY = PlayerY;

    // Set the pixel where he is
    SavePlayerPixel = DisplayMap->data16[Offset];
    DisplayMap->data16[Offset] = 0xFFFF;
}



//============================================================================
// Function : RestorePlayerMarker.
//----------------------------------------------------------------------------
//     Desc : This will restore the graphics underneath the players marker.
//
//============================================================================

void TAutoMap::RestorePlayerMarker()
{
    // Restore the pixel where he was
    if (MarkerX != 0xFFFF)
        DisplayMap->data16[MarkerX + MarkerY * DisplayMap->width] = SavePlayerPixel;
}



//============================================================================
// Function : DrawAutoMap.
//----------------------------------------------------------------------------
//     Desc : This will draw the DisplayMap by laying down the beforemap
//            first and then pulling pixels from the aftermap depending on
//            if the area has been discovered or not.
//
//============================================================================

void TAutoMap::DrawAutoMap()
{
    int Filter[FILTER_SIZE];
    int *EndFilter = Filter + FILTER_SIZE;
    int *c;
    int FilterTotal;
    int x, y;
    int Width = MapGrfx->Background->width;
    int Height = MapGrfx->Background->height;
    int RightClip = Width - FILTER_SIZE / 2 - 1;
    WORD *b = MapGrfx->Background->data16;
    WORD *o = MapGrfx->OverlayMap->data16;
    BYTE *m = MapGrfx->MaskMap->data8;
    WORD *d = DisplayMap->data16;

    // See if we're can display the background or not depending on if the
    // player has the map in his inventory or not
    if (HasMap)
    {
        //--------------------------------------------------------------------
        //                    Top & Right Clipping Version
        //--------------------------------------------------------------------

        for (y = 0; y < FILTER_SIZE / 2; y++)
        {
            // Keep track of the total number of pixels set in FilterTotal
            FilterTotal = 0;

            // Set up the initial state of the Filter
            memset(Filter, 0, FILTER_SIZE);

            for (c = Filter + FILTER_SIZE / 2; c < EndFilter; c++, m++)
            {
                if (y > 1)
                    *c += Active[*(m - (Width << 1))];
                if (y > 0)
                    *c += Active[*(m - Width)];
                *c += Active[*m];
                *c += Active[*(m + Width)];
                *c += Active[*(m + (Width << 1))];

                // Add the column total to the FilterTotal
                FilterTotal += *c;
            }

            // Reset the column total pointer to the beginning of the filter
            c = Filter;

            // Do all the pixels until we would need to right clip
            for (x = 0; x < RightClip; x++, b++, o++, d++, m++)
            {
                // See if any of the pixels are set
                if (FilterTotal)
                {
                    // See if the total number of pixels set is above the
                    // visible cutoff
                    if (FilterTotal < FILTER_CUTOFF)
                    {
                        // The total number of pixels set was below the
                        // cutoff for it to be completely visible, so set
                        // the pixel as a percentage based on how many
                        // pixels were set
                        *d = MergePixel(b, o, FilterTotal, FILTER_CUTOFF);
                    }
                    else
                        *d = *o;
                }


                // Subtract the previous contents of the column total from
                // the FilterTotal
                FilterTotal -= *c;

                // Reset the column total
                *c = 0;

                // Calculate the new column total
                if (y > 1)
                    *c += Active[*(m - (Width << 1))];
                if (y > 0)
                    *c += Active[*(m - Width)];
                *c += Active[*m];
                *c += Active[*(m + Width)];
                *c += Active[*(m + (Width << 1))];

                // Add the column total to the FilterTotal
                FilterTotal += *c;

                // Get the next column to work on
                c++;
                if (c == EndFilter)
                    c = Filter;
            }

            // Finish off the line, but just clear the column totals since
            // they are off the right side of the image
            for (; x < Width; x++, b++, o++, d++)
            {
                // See if any of the pixels are set
                if (FilterTotal)
                {
                    // See if the total number of pixels set is above the
                    // visible cutoff
                    if (FilterTotal < FILTER_CUTOFF)
                    {
                        // The total number of pixels set was below the
                        // cutoff for it to be completely visible, so set
                        // the pixel as a percentage based on how many
                        // pixels were set
                        *d = MergePixel(b, o, FilterTotal, FILTER_CUTOFF);
                    }
                    else
                        *d = *o;
                }

                // Subtract the previous contents of the column total from
                // the FilterTotal
                FilterTotal -= *c;

                // Get the next column to work on
                c++;
                if (c == EndFilter)
                    c = Filter;
            }
        }


        //--------------------------------------------------------------------
        //                     Only Right Clipping Version
        //--------------------------------------------------------------------

        for (; y < Height - FILTER_SIZE / 2; y++)
        {
            // Keep track of the total number of pixels set in FilterTotal
            FilterTotal = 0;

            // Set up the initial state of the Filter
            memset(Filter, 0, FILTER_SIZE);

            for (c = Filter + FILTER_SIZE / 2; c < EndFilter; c++, m++)
            {
                *c += Active[*(m - (Width << 1))];
                *c += Active[*(m - Width)];
                *c += Active[*m];
                *c += Active[*(m + Width)];
                *c += Active[*(m + (Width << 1))];

                // Add the column total to the FilterTotal
                FilterTotal += *c;
            }

            // Reset the column total pointer to the beginning of the filter
            c = Filter;

            // Do all the pixels until we would need to right clip
            for (x = 0; x < RightClip; x++, b++, o++, d++, m++)
            {
                // See if any of the pixels are set
                if (FilterTotal)
                {
                    // See if the total number of pixels set is above the
                    // visible cutoff
                    if (FilterTotal < FILTER_CUTOFF)
                    {
                        // The total number of pixels set was below the
                        // cutoff for it to be completely visible, so set
                        // the pixel as a percentage based on how many
                        // pixels were set
                        *d = MergePixel(b, o, FilterTotal, FILTER_CUTOFF);
                    }
                    else
                        *d = *o;
                }


                // Subtract the previous contents of the column total from
                // the FilterTotal
                FilterTotal -= *c;

                // Reset the column total
                *c = 0;

                // Calculate the new column total
                *c += Active[*(m - (Width << 1))];
                *c += Active[*(m - Width)];
                *c += Active[*m];
                *c += Active[*(m + Width)];
                *c += Active[*(m + (Width << 1))];

                // Add the column total to the FilterTotal
                FilterTotal += *c;

                // Get the next column to work on
                c++;
                if (c == EndFilter)
                    c = Filter;
            }

            // Finish off the line, but just clear the column totals since
            // they are off the right side of the image
            for (; x < Width; x++, b++, o++, d++)
            {
                // See if any of the pixels are set
                if (FilterTotal)
                {
                    // See if the total number of pixels set is above the
                    // visible cutoff
                    if (FilterTotal < FILTER_CUTOFF)
                    {
                        // The total number of pixels set was below the
                        // cutoff for it to be completely visible, so set
                        // the pixel as a percentage based on how many
                        // pixels were set
                        *d = MergePixel(b, o, FilterTotal, FILTER_CUTOFF);
                    }
                    else
                        *d = *o;
                }

                // Subtract the previous contents of the column total from
                // the FilterTotal
                FilterTotal -= *c;

                // Get the next column to work on
                c++;
                if (c == EndFilter)
                    c = Filter;
            }
        }


        //--------------------------------------------------------------------
        //                  Bottom & Right Clipping Version
        //--------------------------------------------------------------------

        for (; y < Height; y++)
        {

            // Set up the initial state of the Filter
            memset(Filter, 0, FILTER_SIZE);

            for (c = Filter + FILTER_SIZE / 2; c < EndFilter; c++, m++)
            {
                *c += Active[*(m - (Width << 1))];
                *c += Active[*(m - Width)];
                *c += Active[*m];
                if (y < Height - 1)
                    *c += Active[*(m + Width)];
                if (y < Height - 2)
                    *c += Active[*(m + (Width << 1))];

                // Add the column total to the FilterTotal
                FilterTotal += *c;
            }

            // Reset the column total pointer to the beginning of the filter
            c = Filter;

            // Do all the pixels until we would need to right clip
            for (x = 0; x < RightClip; x++, b++, o++, d++, m++)
            {
                // See if any of the pixels are set
                if (FilterTotal)
                {
                    // See if the total number of pixels set is above the
                    // visible cutoff
                    if (FilterTotal < FILTER_CUTOFF)
                    {
                        // The total number of pixels set was below the
                        // cutoff for it to be completely visible, so set
                        // the pixel as a percentage based on how many
                        // pixels were set
                        *d = MergePixel(b, o, FilterTotal, FILTER_CUTOFF);
                    }
                    else
                        *d = *o;
                }

                // Subtract the previous contents of the column total from
                // the FilterTotal
                FilterTotal -= *c;

                // Reset the column total
                *c = 0;

                // Calculate the new column total
                *c += Active[*(m - (Width << 1))];
                *c += Active[*(m - Width)];
                *c += Active[*m];
                if (y < Height - 1)
                    *c += Active[*(m + Width)];
                if (y < Height - 2)
                    *c += Active[*(m + (Width << 1))];

                // Add the column total to the FilterTotal
                FilterTotal += *c;

                // Get the next column to work on
                c++;
                if (c == EndFilter)
                    c = Filter;
            }

            // Finish off the line, but just clear the column totals since
            // they are off the right side of the image
            for (; x < Width; x++, b++, o++, d++)
            {
                // See if any of the pixels are set
                if (FilterTotal)
                {
                    // See if the total number of pixels set is above the
                    // visible cutoff
                    if (FilterTotal < FILTER_CUTOFF)
                    {
                        // The total number of pixels set was below the
                        // cutoff for it to be completely visible, so set
                        // the pixel as a percentage based on how many
                        // pixels were set
                        *d = MergePixel(b, o, FilterTotal, FILTER_CUTOFF);
                    }
                    else
                        *d = *o;
                }


                // Subtract the previous contents of the column total from
                // the FilterTotal
                FilterTotal -= *c;

                // Get the next column to work on
                c++;
                if (c == EndFilter)
                    c = Filter;
            }
        }
    }
    else
    {
        //--------------------------------------------------------------------
        //                  No Map Top & Right Clipping Version
        //--------------------------------------------------------------------

        for (y = 0; y < FILTER_SIZE / 2; y++)
        {
            // Keep track of the total number of pixels set in FilterTotal
            FilterTotal = 0;

            // Set up the initial state of the Filter
            memset(Filter, 0, FILTER_SIZE);

            for (c = Filter + FILTER_SIZE / 2; c < EndFilter; c++, m++)
            {
                if (y > 1)
                    *c += Active[*(m - (Width << 1))];
                if (y > 0)
                    *c += Active[*(m - Width)];
                *c += Active[*m];
                *c += Active[*(m + Width)];
                *c += Active[*(m + (Width << 1))];

                // Add the column total to the FilterTotal
                FilterTotal += *c;
            }

            // Reset the column total pointer to the beginning of the filter
            c = Filter;

            // Do all the pixels until we would need to right clip
            for (x = 0; x < RightClip; x++, b++, o++, d++, m++)
            {
                // See if any of the pixels are set
                if (FilterTotal)
                {
                    // See if the total number of pixels set is above the
                    // visible cutoff
                    if (FilterTotal < FILTER_CUTOFF)
                    {
                        // The total number of pixels set was below the
                        // cutoff for it to be completely visible, so set
                        // the pixel as a percentage based on how many
                        // pixels were set
                        *d = MergePixelToBlack(o, FilterTotal, FILTER_CUTOFF);
                    }
                    else
                        *d = *o;
                }


                // Subtract the previous contents of the column total from
                // the FilterTotal
                FilterTotal -= *c;

                // Reset the column total
                *c = 0;

                // Calculate the new column total
                if (y > 1)
                    *c += Active[*(m - (Width << 1))];
                if (y > 0)
                    *c += Active[*(m - Width)];
                *c += Active[*m];
                *c += Active[*(m + Width)];
                *c += Active[*(m + (Width << 1))];

                // Add the column total to the FilterTotal
                FilterTotal += *c;

                // Get the next column to work on
                c++;
                if (c == EndFilter)
                    c = Filter;
            }

            // Finish off the line, but just clear the column totals since
            // they are off the right side of the image
            for (; x < Width; x++, b++, o++, d++)
            {
                // See if any of the pixels are set
                if (FilterTotal)
                {
                    // See if the total number of pixels set is above the
                    // visible cutoff
                    if (FilterTotal < FILTER_CUTOFF)
                    {
                        // The total number of pixels set was below the
                        // cutoff for it to be completely visible, so set
                        // the pixel as a percentage based on how many
                        // pixels were set
                        *d = MergePixelToBlack(o, FilterTotal, FILTER_CUTOFF);
                    }
                    else
                        *d = *o;
                }

                // Subtract the previous contents of the column total from
                // the FilterTotal
                FilterTotal -= *c;

                // Get the next column to work on
                c++;
                if (c == EndFilter)
                    c = Filter;
            }
        }


        //--------------------------------------------------------------------
        //                  No Map Only Right Clipping Version
        //--------------------------------------------------------------------

        for (; y < Height - FILTER_SIZE / 2; y++)
        {
            // Keep track of the total number of pixels set in FilterTotal
            FilterTotal = 0;

            // Set up the initial state of the Filter
            memset(Filter, 0, FILTER_SIZE);

            for (c = Filter + FILTER_SIZE / 2; c < EndFilter; c++, m++)
            {
                *c += Active[*(m - (Width << 1))];
                *c += Active[*(m - Width)];
                *c += Active[*m];
                *c += Active[*(m + Width)];
                *c += Active[*(m + (Width << 1))];

                // Add the column total to the FilterTotal
                FilterTotal += *c;
            }

            // Reset the column total pointer to the beginning of the filter
            c = Filter;

            // Do all the pixels until we would need to right clip
            for (x = 0; x < RightClip; x++, b++, o++, d++, m++)
            {
                // See if any of the pixels are set
                if (FilterTotal)
                {
                    // See if the total number of pixels set is above the
                    // visible cutoff
                    if (FilterTotal < FILTER_CUTOFF)
                    {
                        // The total number of pixels set was below the
                        // cutoff for it to be completely visible, so set
                        // the pixel as a percentage based on how many
                        // pixels were set
                        *d = MergePixelToBlack(o, FilterTotal, FILTER_CUTOFF);
                    }
                    else
                        *d = *o;
                }


                // Subtract the previous contents of the column total from
                // the FilterTotal
                FilterTotal -= *c;

                // Reset the column total
                *c = 0;

                // Calculate the new column total
                *c += Active[*(m - (Width << 1))];
                *c += Active[*(m - Width)];
                *c += Active[*m];
                *c += Active[*(m + Width)];
                *c += Active[*(m + (Width << 1))];

                // Add the column total to the FilterTotal
                FilterTotal += *c;

                // Get the next column to work on
                c++;
                if (c == EndFilter)
                    c = Filter;
            }

            // Finish off the line, but just clear the column totals since
            // they are off the right side of the image
            for (; x < Width; x++, b++, o++, d++)
            {
                // See if any of the pixels are set
                if (FilterTotal)
                {
                    // See if the total number of pixels set is above the
                    // visible cutoff
                    if (FilterTotal < FILTER_CUTOFF)
                    {
                        // The total number of pixels set was below the
                        // cutoff for it to be completely visible, so set
                        // the pixel as a percentage based on how many
                        // pixels were set
                        *d = MergePixelToBlack(o, FilterTotal, FILTER_CUTOFF);
                    }
                    else
                        *d = *o;
                }

                // Subtract the previous contents of the column total from
                // the FilterTotal
                FilterTotal -= *c;

                // Get the next column to work on
                c++;
                if (c == EndFilter)
                    c = Filter;
            }
        }


        //--------------------------------------------------------------------
        //                No Map Bottom & Right Clipping Version
        //--------------------------------------------------------------------

        for (; y < Height; y++)
        {

            // Set up the initial state of the Filter
            memset(Filter, 0, FILTER_SIZE);

            for (c = Filter + FILTER_SIZE / 2; c < EndFilter; c++, m++)
            {
                *c += Active[*(m - (Width << 1))];
                *c += Active[*(m - Width)];
                *c += Active[*m];
                if (y < Height - 1)
                    *c += Active[*(m + Width)];
                if (y < Height - 2)
                    *c += Active[*(m + (Width << 1))];

                // Add the column total to the FilterTotal
                FilterTotal += *c;
            }

            // Reset the column total pointer to the beginning of the filter
            c = Filter;

            // Do all the pixels until we would need to right clip
            for (x = 0; x < RightClip; x++, b++, o++, d++, m++)
            {
                // See if any of the pixels are set
                if (FilterTotal)
                {
                    // See if the total number of pixels set is above the
                    // visible cutoff
                    if (FilterTotal < FILTER_CUTOFF)
                    {
                        // The total number of pixels set was below the
                        // cutoff for it to be completely visible, so set
                        // the pixel as a percentage based on how many
                        // pixels were set
                        *d = MergePixelToBlack(o, FilterTotal, FILTER_CUTOFF);
                    }
                    else
                        *d = *o;
                }

                // Subtract the previous contents of the column total from
                // the FilterTotal
                FilterTotal -= *c;

                // Reset the column total
                *c = 0;

                // Calculate the new column total
                *c += Active[*(m - (Width << 1))];
                *c += Active[*(m - Width)];
                *c += Active[*m];
                if (y < Height - 1)
                    *c += Active[*(m + Width)];
                if (y < Height - 2)
                    *c += Active[*(m + (Width << 1))];

                // Add the column total to the FilterTotal
                FilterTotal += *c;

                // Get the next column to work on
                c++;
                if (c == EndFilter)
                    c = Filter;
            }

            // Finish off the line, but just clear the column totals since
            // they are off the right side of the image
            for (; x < Width; x++, b++, o++, d++)
            {
                // See if any of the pixels are set
                if (FilterTotal)
                {
                    // See if the total number of pixels set is above the
                    // visible cutoff
                    if (FilterTotal < FILTER_CUTOFF)
                    {
                        // The total number of pixels set was below the
                        // cutoff for it to be completely visible, so set
                        // the pixel as a percentage based on how many
                        // pixels were set
                        *d = MergePixelToBlack(o, FilterTotal, FILTER_CUTOFF);
                    }
                    else
                        *d = *o;
                }


                // Subtract the previous contents of the column total from
                // the FilterTotal
                FilterTotal -= *c;

                // Get the next column to work on
                c++;
                if (c == EndFilter)
                    c = Filter;
            }
        }
    }

    // Draw any graphics that are Overlayed on the map
    for (x = 0; x < MapGrfx->NumOverlays; x++)
    {
        if (Active[MapGrfx->Overlays[x].ActiveColor])
        {
            DisplayMap->Put(MapGrfx->Overlays[x].XOffset, MapGrfx->Overlays[x].YOffset,
                MapGrfx->Overlays[x].Image);
        }
    }
}




//============================================================================
// Function : MergePixel.
//----------------------------------------------------------------------------
//     Desc : This will return the Overlay pixel faded onto the Background
//            pixel with the translucent percentage defined by Mul / Div.
//
//============================================================================

inline WORD TAutoMap::MergePixel(WORD *Background, WORD *Overlay, int Mul, int Div)
{
    BYTE Alpha = (BYTE)((Mul << 5) / Div);
    WORD Result;

    __asm
    {
        xor  edx, edx

        mov  esi, [Background]
        mov  edi, [Overlay]

        mov  dh, [Alpha]

        mov  dl, BYTE PTR [edi + 1]
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, BYTE PTR [edi]
        add  al, [ColorTableLower + edx]
        adc  ah, 0

        mov  dh, 31
        sub  dh, [Alpha]

        mov  dl, BYTE PTR [esi + 1]
        mov  bh, [ColorTableUpperHi + edx]
        mov  bl, [ColorTableUpperLo + edx]
        mov  dl, BYTE PTR [esi]
        add  bl, [ColorTableLower + edx]
        adc  bh, 0

        add  ax, bx

        mov  [Result], ax
    }

    return Result;
}



//============================================================================
// Function : MergePixelToBlack.
//----------------------------------------------------------------------------
//     Desc : This is the same as MergePixel except that the Overlayed pixel
//            is faded to black depending on the Mul / Div.
//
//============================================================================

inline WORD TAutoMap::MergePixelToBlack(WORD *Overlay, int Mul, int Div)
{
    BYTE Alpha = (BYTE)((Mul << 5) / Div);
    WORD Result;

    __asm
    {
        xor  edx, edx

        mov  edi, [Overlay]

        mov  dh, [Alpha]

        mov  dl, BYTE PTR [edi + 1]
        mov  ah, [ColorTableUpperHi + edx]
        mov  al, [ColorTableUpperLo + edx]
        mov  dl, BYTE PTR [edi]
        add  al, [ColorTableLower + edx]
        adc  ah, 0

        mov  [Result], ax
    }

    return Result;
}



//============================================================================
// Function : LoadMapGraphics.
//----------------------------------------------------------------------------
//     Desc : This will load the map graphics needed to display the automap
//            for the map specified.
//
//    Entry : MapNum = number of the automap to load.
//
//  Returns : Returns TRUE if successful, FALSE if not.
//
//============================================================================

BOOL TAutoMap::LoadMapGraphics(int MapNum)
{
    // If they are trying to load the map that's already loaded, return immediately
    if (MapNum == CurrentMap)
        return TRUE;

    // Make sure the MapNum is valid
    if ((MapNum < 0) || (MapNum >= MapList->NumMaps))
       return FALSE;

    // Clear out the old map graphics
    FreeMapGraphics();

    // Load all the graphics for the map from the specified AUTOMAP resource
    int ResID = MapList->MapList[MapNum].ResID;
    MapGrfx = (PTAutoMapGraphics)LoadResource("AUTOMAP", ResID);
    if (MapGrfx == NULL)
        return FALSE;

    // Create another bitmap to draw the map that's going to be displayed
    DisplayMap = TBitmap::NewBitmap(MapGrfx->Background->width,
        MapGrfx->Background->height, MapGrfx->Background->flags);
    if (DisplayMap == NULL)
        return FALSE;

    // Point the Active pointer to the correct section of the ActiveBuf to
    // correspond to the map we've just loaded
    Active = ActiveBuf + MapNum * 256;

    // Reset the position variables
    ScrollX = ScrollY = 0;
    OldPlayerX = OldPlayerY = 0xFFFF;
    OldScrollX = OldScrollY = 0xFFFF;

    // Keep track of the map we just loaded
    CurrentMap = MapNum;

    // Draw the initial state of the map

    // Clear the DisplayMap to the Background or black if the player doesn't
    // have the appropriate map in his inventory
    if (HasMap)
        DisplayMap->Put(0, 0, MapGrfx->Background);
    else
    {
        SColor black = { 0, 0, 0 };
        DisplayMap->Clear(black);
    }

    DrawAutoMap();

    SetDirty(TRUE);

    return TRUE;
}



//============================================================================
// Function : FreeMapGraphics.
//----------------------------------------------------------------------------
//     Desc : This will free up the memory allocated for graphics used by
//            the current automap.
//
//============================================================================

void TAutoMap::FreeMapGraphics()
{
    free(MapGrfx);
    MapGrfx = NULL;

    delete DisplayMap;
    DisplayMap = NULL;
}



//============================================================================
// Function : RecordTravels.
//----------------------------------------------------------------------------
//     Desc : This checks the players position against the MaskMap and
//            sets the area underneath him as active.
//
//============================================================================

void TAutoMap::RecordTravels()
{
    int PlayerX;
    int PlayerY;
    int Width;
    int Height;
    int Offset;
    int Color;

    // Make sure that the maps are loaded
    if (MapGrfx == NULL)
        CheckMaps();

    if (MapGrfx == NULL)
        return;

    GetPlayerOnAutoMap(PlayerX, PlayerY);

    if ((PlayerX != OldPlayerX) || (PlayerY != OldPlayerY))
    {
        int Left   = PlayerX - AUTOMAP_VIEW_WIDTH / 2;
        int Top    = PlayerY - AUTOMAP_VIEW_HEIGHT / 2;
        int Right  = Left + AUTOMAP_VIEW_WIDTH;
        int Bottom = Top + AUTOMAP_VIEW_HEIGHT;

        CheckMaps();

        if (MapGrfx == NULL)
            return;

        Width = MapGrfx->Background->width;
        Height = MapGrfx->Background->height;

        // Make sure that the players coordinates are in the map
        if (Left < 0)
            Left = 0;
        if (Top < 0)
            Top = 0;
        if (Right >= Width)
            Right = Width - 1;
        if (Bottom >= Height)
            Bottom = Height - 1;

        for (int y = Top; y < Bottom; y++)
        {
            for (int x = Left; x < Right; x++)
            {
                // Check if this area of the map is active or not
                Offset = x + y * Width;
                Color = MapGrfx->MaskMap->data8[Offset];
                if (!Active[Color])
                {
                    // The player has just revealed this area.
                    Active[Color] = 1;

                    DrawAutoMap();

                    // Draw the Marker where the player is
                    DrawPlayerMarker(PlayerX, PlayerY);

                    SetDirty(TRUE);

                    // If there are any scripting things to be done, do them here
                }
            }
        }

        // If the player has started moving and the map is not centered
        // on the player and the player is not dragging the map around
        // then we can start moving the map to show the player
        if ((!DraggingMap) && (!ScrollToTarget))
        {
            int PaneWidth = GetWidth();
            int PaneHeight = GetHeight();

            TargetScrollX = PlayerX - PaneWidth / 2;
            TargetScrollY = PlayerY - PaneHeight / 2;

            // Make sure we don't scroll negative
            if (TargetScrollX < 0)
                TargetScrollX = 0;
            if (TargetScrollY < 0)
                TargetScrollY = 0;

            // Make sure we don't scroll past the right / bottom of the map
            if (TargetScrollX + PaneWidth > DisplayMap->width)
                TargetScrollX = DisplayMap->width - PaneWidth;
            if (TargetScrollY + PaneHeight > DisplayMap->height)
                TargetScrollY = DisplayMap->height - PaneHeight;

            if ((TargetScrollX != ScrollX) || (TargetScrollY != ScrollY))
            {
                // Calculate the distance each axis must move
                MapScrollDx = TargetScrollX - ScrollX;
                MapScrollDy = TargetScrollY - ScrollY;

                // Determine which is larger and set the Deltas accordingly
                if (abs(MapScrollDx) > abs(MapScrollDy))
                {
                    if (abs(MapScrollDx) >= MAX_AUTOMAP_SCROLL_SPEED)
                    {
                        MapScrollDy /= abs(MapScrollDx / MAX_AUTOMAP_SCROLL_SPEED);
                        MapScrollDx = MAX_AUTOMAP_SCROLL_SPEED * MapScrollDx / abs(MapScrollDx);
                    }
                }
                else
                {
                    if (abs(MapScrollDy) >= MAX_AUTOMAP_SCROLL_SPEED)
                    {
                        MapScrollDx /= abs(MapScrollDy / MAX_AUTOMAP_SCROLL_SPEED);
                        MapScrollDy = MAX_AUTOMAP_SCROLL_SPEED * MapScrollDy / abs(MapScrollDy);
                    }
                }

                ScrollToTarget = TRUE;
            }
        }
    }
}



//============================================================================
// Function : CheckMaps.
//----------------------------------------------------------------------------
//     Desc : This sees which set of before and after maps are needed for
//            the map section that we're currently tracking and loads them
//            if necessary.
//
//============================================================================

void TAutoMap::CheckMaps()
{
    S3DPoint Pos;
    MapPane.GetMapPos(Pos);
    int PlayerX;
    int PlayerY;
    int Level = MapPane.GetMapLevel();
    int MapNum = 0;
    int n;
    BOOL Found = FALSE;
    SAutoMapData *ml = MapList->MapList;

    // Look for which auto map to use
    for (n = 0; n < MapList->NumMaps; n++, ml++)
    {
        // Check the map level
        if (ml->Level == Level)
        {
            // Check if the Right & Bottom are both 0 (this signifies the
            // entire level is used for the map)
            if ((ml->Right == 0) && (ml->Bottom == 0))
            {
                MapArea.left   = 0;
                MapArea.top    = 0;
                MapArea.right  = MAXMAPWIDTH - 1;
                MapArea.bottom = MAXMAPHEIGHT - 1;

                Found = TRUE;
            }
            else
            // See if the player is in the map area (in World coords.)
            if ((Pos.x >= ml->Left) && (Pos.x <= ml->Right) &&
                (Pos.y >= ml->Top) && (Pos.y <= ml->Bottom))
            {
                MapArea.left   = ml->Left;
                MapArea.top    = ml->Top;
                MapArea.right  = ml->Right;
                MapArea.bottom = ml->Bottom;

                Found = TRUE;
            }

            if (Found)
            {
                MapNum = n;

                // See if the MapNum has changed
                if (MapNum != CurrentMap)
                {
                    // Check to see if the player has the appropriate map in his
                    // inventory or not
                    if (Player && Player->FindObjInventory(ml->MapName))
                        HasMap = TRUE;
                    else
                        HasMap = FALSE;
                    HasMap = TRUE;

                    // Load the graphics for the map we're going to display
                    LoadMapGraphics(MapNum);

                    GetPlayerOnAutoMap(PlayerX, PlayerY);

                    // Draw the Marker where the player is
                    DrawPlayerMarker(PlayerX, PlayerY);
                }

                return;
            }
        }
    }

}



//============================================================================
// Function : GetMapNumber.
//----------------------------------------------------------------------------
//     Desc : This will return which MapNumber in the MapList matches the
//            map specified by MapName.
//
//    Entry : MapName = Name of the map whose mask info we're getting.
//
//  Returns : Returns which MapNumber in the MapList matches MapName
//            or -1 if the MapName was not found in the list.
//
//============================================================================

int TAutoMap::GetMapNumber(char *MapName)
{
    // Find the MapNum by checking the MapName against the MapList
    for (int n = 0; n < MapList->NumMaps; n++)
        if (!stricmp(MapList->MapList[n].MapName, MapName))
            return n;

    return -1;
}



//============================================================================
// Function : GetMaskColorActive.
//----------------------------------------------------------------------------
//     Desc : This will return whether a specific color in a specific map
//            has been activated or not.
//
//    Entry : MapNum = Number of the map whose mask info we're getting.
//            Color  = Index of the color we wish to check.
//
//  Returns : Returns TRUE if the color is active, FALSE if not.
//
//============================================================================

BOOL TAutoMap::GetMaskColorActive(int MapNum, int Color)
{
    // Make sure the MapNum & Color are valid
    if ((MapNum < 0) || (MapNum >= MapList->NumMaps) ||
        (Color < 0) || (Color > 255))
        return FALSE;

    return (ActiveBuf[MapNum * 256 + Color]);
}



//============================================================================
// Function : GetMaskColorActive.
//----------------------------------------------------------------------------
//     Desc : This will return whether a specific color in a specific map
//            has been activated or not.
//
//    Entry : MapName = Name of the map whose mask info we're getting.
//            Color   = Index of the color we wish to check.
//
//  Returns : Returns TRUE if the color is active, FALSE if not.
//
//============================================================================

BOOL TAutoMap::GetMaskColorActive(char *MapName, int Color)
{
    return GetMaskColorActive(GetMapNumber(MapName), Color);
}



//============================================================================
// Function : SetMaskColorActive.
//----------------------------------------------------------------------------
//     Desc : This will set a specific color in a specific map to be active
//            or not.
//
//    Entry : MapNum = Number of the map whose mask info we're setting.
//            Color  = Index of the color we wish to set.
//            Active = State to set the color to.
//
//============================================================================

void TAutoMap::SetMaskColorActive(int MapNum, int Color, BYTE Active)
{
    // Make sure the MapNum & Color are valid
    if ((MapNum < 0) || (MapNum >= MapList->NumMaps) ||
        (Color < 0) || (Color > 255))
        return;

    int Index = MapNum * 256 + Color;

    // See if the new active state is different than the current state
    if (ActiveBuf[Index] != Active)
    {
        ActiveBuf[Index] = Active;

        // If the map that's being affected is the current map, redraw the
        // automap pane
        if (MapNum == CurrentMap)
            SetDirty(TRUE);

        // If there are any scripting things to be done, do them here
    }
}



//============================================================================
// Function : SetMaskColorActive.
//----------------------------------------------------------------------------
//     Desc : This will set a specific color in a specific map to be active
//            or not.
//
//    Entry : MapName = Name of the map whose mask info we're setting.
//            Color   = Index of the color we wish to set.
//            Active  = State to set the color to.
//
//============================================================================

void TAutoMap::SetMaskColorActive(char *MapName, int Color, BYTE Active)
{
    SetMaskColorActive(GetMapNumber(MapName), Color, Active);
}



//============================================================================
// Function : MouseClick.
//----------------------------------------------------------------------------
//     Desc : This handles any mouse clicks over the automap pane.
//
//    Entry : button = state of the mouse buttons
//            x      = mouse x coordinate
//            y      = mouse y coordinate
//
//============================================================================

void TAutoMap::MouseClick(int button, int x, int y)
{
    // See if we need tp turn of the dragging of the mouse
    if ((button == MB_RIGHTUP) && (DraggingMap))
    {
        DraggingMap = FALSE;
        SetMouseBitmap(PointerCursor);
    }

    // See if they are beginning to drag the map
    if ((button == MB_RIGHTDOWN) && (!DraggingMap) && (InPane(x, y)))
    {
        // The right mouse button is down and over the automap pane, so we
        // can start dragging the map around
        DragMouseX = x;
        DragMouseY = y;
        DragScrollX = ScrollX;
        DragScrollY = ScrollY;
        DraggingMap = TRUE;
        SetMouseBitmap(HandCursor);
        ScrollToTarget = FALSE;
    }

    // See if they are trying to bring up a menu
    if (button == MB_LEFTUP)
    {

    }
}



//============================================================================
// Function : MouseMove.
//----------------------------------------------------------------------------
//     Desc : This handles any mouse movement over the automap pane.
//
//    Entry : button = state of the mouse buttons
//            x      = mouse x coordinate
//            y      = mouse y coordinate
//
//============================================================================

void TAutoMap::MouseMove(int button, int x, int y)
{
    // See if the right mouse button is down
    if (button == MB_RIGHTDOWN)
    {
        // Check if we have already started dragging it
        if (DraggingMap)
        {
            // Get the offset of the mouse from where it began
            int dx = x - DragMouseX;
            int dy = y - DragMouseY;

            // Update the scroll position and redraw the pane
            ScrollX = DragScrollX - dx;
            ScrollY = DragScrollY - dy;
        }
        else
        // Check if the mouse is over the automap pane
        if (InPane(x, y))
        {
            // The right mouse button is down and over the automap pane, so we
            // can start dragging the map around
            DragMouseX = x;
            DragMouseY = y;
            DragScrollX = ScrollX;
            DragScrollY = ScrollY;
            DraggingMap = TRUE;
            SetMouseBitmap(HandCursor);
            ScrollToTarget = FALSE;
        }
    }
    else
    {
        // If they were dragging the map around and just let go, reset our variables
        if (DraggingMap)
        {
            DraggingMap = FALSE;
            SetMouseBitmap(PointerCursor);
        }
    }
}



//============================================================================
// Function : WriteAutoMapData.
//----------------------------------------------------------------------------
//     Desc : This will compact the ActiveBuf to a bit array and write it
//            out to the file passed in.
//
//    Entry : File = File to write the data out to
//
//  Returns : Returns how many bytes were written to the file
//            or -1 if it fails.
//
//============================================================================

size_t TAutoMap::WriteAutoMapData(FILE *fp)
{
    BYTE *Buf;
    BYTE *s = ActiveBuf;
    BYTE *d;
    size_t Size = MapList->NumMaps * 32;

    // Allocate the bit array
    if ((Buf = (BYTE *)malloc(Size)) == NULL)
        return -1;

    // Pack the ActiveBuf into a bit array for storage
    d = Buf;
    for (size_t n = 0; n < Size; n++, s += 8, d++)
    {
        *d  = *s;
        *d |= *(s + 1) << 1;
        *d |= *(s + 2) << 2;
        *d |= *(s + 3) << 3;
        *d |= *(s + 4) << 4;
        *d |= *(s + 5) << 5;
        *d |= *(s + 6) << 6;
        *d |= *(s + 7) << 7;
    }

    // Write it out
    if (fwrite(Buf, 1, Size, fp) != Size)
        Size = -1;

    free(Buf);

    return Size;
}



//============================================================================
// Function : ReadAutoMapData.
//----------------------------------------------------------------------------
//     Desc : This will load the compacted ActiveBuf from the file passed in
//            and then uncompress the bit array back to ActiveBuf.
//
//    Entry : File = File to read the data from.
//
//  Returns : Returns how many bytes were read from the file
//            or -1 if it fails.
//
//============================================================================

size_t TAutoMap::ReadAutoMapData(FILE *fp)
{
    BYTE *Buf;
    BYTE *d = ActiveBuf;
    BYTE *s;
    size_t Size = MapList->NumMaps * 32;

    // Allocate the bit array
    if ((Buf = (BYTE *)malloc(Size)) == NULL)
        return -1;

    // Try to read the packed ActiveBuf into the temp buffer
    if (fread(Buf, 1, Size, fp) != Size)
        Size = -1;
    else
    {
        // Unpack the bit array back into the ActiveBuf
        s = Buf;
        for (size_t n = 0; n < Size; n++, s++)
        {
            *d = (*s) & 1;
            d++;
            *d = (*s >> 1) & 1;
            d++;
            *d = (*s >> 2) & 1;
            d++;
            *d = (*s >> 3) & 1;
            d++;
            *d = (*s >> 4) & 1;
            d++;
            *d = (*s >> 5) & 1;
            d++;
            *d = (*s >> 6) & 1;
            d++;
            *d = *s >> 7;
            d++;
        }
    }

    free(Buf);

    return Size;
}



//============================================================================
// Function : GetPlayerOnAutoMap.
//----------------------------------------------------------------------------
//     Desc : This will convert from the players current position in World
//            coordinates to the (x, y) position on the AutoMap graphic.
//
//    Entry : PlayerX = Ref. to the variable to recieve the players x coord.
//            PlayerY = Ref. to the variable to recieve the players y coord.
//
//============================================================================

void TAutoMap::GetPlayerOnAutoMap(int &PlayerX, int &PlayerY)
{
    S3DPoint Pos;
    MapPane.GetMapPos(Pos);

    PlayerX = (Pos.x - MapArea.left) * DisplayMap->width / (MapArea.right - MapArea.left + 1);
    PlayerX = max(0, min(PlayerX, DisplayMap->width));
    PlayerY = (Pos.y - MapArea.top) * DisplayMap->height / (MapArea.bottom - MapArea.top + 1);
    PlayerY = max(0, min(PlayerY, DisplayMap->height));
}
