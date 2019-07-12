// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     area.h - Game Area Manager                        *
// ************************************************************************* 

#include "revenant.h" 
#include "parse.h"
#include "sound.h"
#include "mappane.h"
#include "textbar.h"
#include "script.h"
#include "player.h"
#include "playscreen.h"
#include "textbar.h"
#include "area.h"

// *****************************
// * TArea - Basic area object *
// *****************************

static S3DPoint lastpos;
static int lastlevel;
static int lastdaylight;
static int lastdayflag;

// Initializes and loads an area
TArea::TArea()
{
    flags = 0;
    name[0] = NULL;
    rects.Clear();
    scriptfile[0] = NULL;
    amblight = 30;
    nightamblight = 10;
    ambcolor.red = ambcolor.green = ambcolor.blue = 255;
    nightambcolor.red = nightambcolor.green = nightambcolor.blue = 255;
    level = -1; // All levels
    rects.Clear();

  // CD audio music playing
    cdplaynum = 0;
    cdplaylistsize = 0;
    cdplayisrandom = FALSE;
    cdplaypause = 0;
}

// Gets rid of area
TArea::~TArea()
{
    Exit();
}

// Loads the area from the "AREA.DEF" file
BOOL TArea::Load(char *aname, TToken &t)
{
    strncpyz(name, aname, MAXNAMELEN);

    t.SkipBlanks();
    if (!t.Is("BEGIN"))
        t.Error("Area block BEGIN expected");
    
  // Now get first trigger token
    t.LineGet();
    
  // Iterate through the triggers and setup trigger list
    while (t.Type() != TKN_EOF && !t.Is("END"))
    {
      // Parse trigger tags now
        if (t.Type() != TKN_IDENT)
            t.Error("Area keyword expected");

      // Tags...
        if (t.Is("LEVEL"))
        {
            t.WhiteGet();

            if (t.Type() == TKN_NUMBER)
            {
                level = t.Index();
                t.WhiteGet();
            }
            else if (t.Is("ALL"))
            {
                level = -1;
                t.WhiteGet();
            }
            
            if (t.Type() != TKN_RETURN)
                t.Error("'LEVEL <ALL|level>' expected");
            
            t.LineGet();
        }
        else if (t.Is("RECT"))
        {
            SRect r;
            if (!Parse(t, "RECT %i, %i, %i, %i\n", &r.left, &r.top, &r.right, &r.bottom))
                t.Error("'RECT left top right bottom' expected");
            int temp;
            if (r.right < r.left)
                { temp = r.left; r.left = r.right; r.right = temp; }
            if (r.bottom < r.top)
                { temp = r.top; r.top = r.bottom; r.bottom = temp; }
            rects.Add(r);
        }
        else if (t.Is("SCRIPT"))
        {
            if (!Parse(t, "SCRIPT %s\n", scriptfile))
                t.Error("'SCRIPT \"filename\"' expected");
            flags |= AREA_LOADSCRIPTS;
        }
        else if (t.Is("AMBLIGHT"))
        {
            if (!Parse(t, "AMBLIGHT %i\n", &amblight))
                t.Error("'AMBLIGHT light' expected");

            flags |= AREA_SETAMBIENT;
        }
        else if (t.Is("AMBCOLOR"))
        {
            if (!Parse(t, "AMBCOLOR %b, %b, %b\n", &ambcolor.red, &ambcolor.green, &ambcolor.blue))
                t.Error("'AMBCOLOR red green blue' expected");

            flags |= AREA_SETAMBIENT;
        }
        else if (t.Is("NIGHTAMBLIGHT"))
        {
            if (!Parse(t, "NIGHTAMBLIGHT %i\n", &nightamblight))
                t.Error("'NIGHTAMBLIGHT light' expected");

            flags |= AREA_DONIGHT;
        }
        else if (t.Is("NIGHTAMBCOLOR"))
        {
            if (!Parse(t, "NIGHTAMBCOLOR %b, %b, %b\n", &nightambcolor.red, &nightambcolor.green, &nightambcolor.blue))
                t.Error("'NIGHTAMBCOLOR red green blue' expected");

            flags |= AREA_DONIGHT;
        }
        else if (t.Is("CDPLAYLIST"))
        {
            char *cdplaylisterr = "'CDPLAYLIST <RANDOM> <PAUSE pause> TRACKS track1 track2 track3 ...' expected";

            t.WhiteGet();
            if (t.Is("RANDOM"))
            {
                cdplayisrandom = TRUE;
                t.WhiteGet();
            }
            else 
                cdplayisrandom = FALSE;

            if (t.Is("PAUSE"))
            {
                if (!Parse(t, "PAUSE %d", &cdplaypause))
                    t.Error(cdplaylisterr);
            }

            if (!t.Is("TRACKS"))
                    t.Error(cdplaylisterr);

            t.WhiteGet();

            cdplaylistsize = 0;
            while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
            {
                if (cdplaylistsize >= MAXPLAYLISTSIZE)
                    t.Error("CDPLAYLIST too many tracks");

                if (t.Type() != TKN_NUMBER)
                    t.Error(cdplaylisterr);

                cdplaylist[cdplaylistsize] = t.Index();
                cdplaylistsize++;

                t.WhiteGet();
            }
            t.LineGet();

            flags |= AREA_PLAYCDMUSIC;
        }
        else
        {
            char buf[80];
            sprintf(buf, "Invalid area tag %s", t.Text());
            t.Error(buf);
        }
    }

    if (!t.Is("END"))
        t.Error("Area block END expected");

  // If no rects for area, add a default super huge rect
    if (rects.NumItems() <= 0)
    {
        SRect r;
        r.left = -1000000000;
        r.top = -1000000000;
        r.right = 1000000000;
        r.bottom = 1000000000;
        rects.Add(r);
    }

    return TRUE;
}

BOOL TArea::In(S3DPoint &pos, int lev)
{
    if (level != -1 && lev != level)
        return FALSE;

    SPoint p;
    p.x = pos.x;
    p.y = pos.y;

    for (int c = 0; c < rects.NumItems(); c++)
    {
        if (rects[c].In(p))
            return TRUE;
    }

    return FALSE;
}

// Initializes ambient sounds for area
void TArea::InitAmbientSounds()
{
}

// Deinitializes ambient sounds for area
void TArea::CloseAmbientSounds()
{
}

void TArea::PlayAmbientSounds()
{
    // Play ambient sound effects here
}

// Initializes data needed to play CD tracks
void TArea::InitCDMusic()
{
  // Initialize CD play params
    if (CDPlaying())
        CDStop();   

  // Initialize play params
    cdplaynum = -1;
    cdplaystart = tickcount();
    cdplaywait = 0;
    cdplaylength = 0;
}

// Deinitializes CD play system
void TArea::CloseCDMusic()
{
    // Do nothing (allow music to continue playing)
}

// Plays CD music tracks
void TArea::PlayCDMusic()
{
    if ((int)tickcount() - cdplaystart >= cdplaywait)
    {
        if (cdplayisrandom)
        {
            int newplaynum;
            do {
                newplaynum = random(0, cdplaylistsize - 1);
            } while (newplaynum == cdplaynum && cdplaylistsize > 1);
            cdplaynum = newplaynum;
        }
        else
        {
            cdplaynum++;
            if (cdplaynum >= cdplaylistsize)
                cdplaynum = 0;
        }

        cdplaylength = CDTrackLength(cdplaylist[cdplaynum]);
        cdplaywait = cdplaylength + cdplaypause * 1000;
        if (cdplaywait <= 1000)
            cdplaywait = 3 * 60 * 1000; // Wait 3 minutes if no track found
        CDPlayTrack(cdplaylist[cdplaynum]);
        cdplaystart = tickcount();
    }
    else if ((int)tickcount() - cdplaystart >= cdplaylength)
        PlayAmbientSounds();
}

// Gets the current ambient colors based on the time of day
void TArea::GetCurrentAmbient(int &ambient, SColor &color)
{
    if (flags & AREA_DONIGHT)
    {
        int daylight = PlayScreen.Daylight();

        if (daylight == 0)
        {
            ambient = nightamblight;
            color = nightambcolor;
        }
        else if (daylight == 255)
        {
            ambient = amblight;
            color = ambcolor;
        }
        else
        {

            ambient = amblight * daylight / 255 + nightamblight * (255 - daylight) / 255;
            color.red = (BYTE)((int)((int)ambcolor.red * daylight / 255 + 
                (int)nightambcolor.red * (255 - daylight) / 255));
            color.green = (BYTE)((int)((int)ambcolor.green * daylight / 255 + 
                (int)nightambcolor.green * (255 - daylight) / 255));
            color.blue = (BYTE)((int)((int)ambcolor.blue * daylight / 255 + 
                (int)nightambcolor.blue * (255 - daylight) / 255));
        }
    }
    else
    {
        ambient = amblight;
        color = ambcolor;
    }
}

// Called by the game screen Pulse() function to update area stuff
void TArea::Pulse()
{
    if ((flags & AREA_PLAYCDMUSIC) && cdplaylistsize > 0)
        PlayCDMusic();

    if (flags & AREA_PLAYAMBIENT)
        PlayAmbientSounds();

    int dayflag = PlayScreen.DayTimeFlag();
    if (flags & AREA_DONIGHT)
    {
        if ((dayflag != lastdayflag) && !(dayflag == DAY_DAYTIME) && !(dayflag == DAY_NIGHT)) 
        {
            char *daynames[6] = {"midnight", "morning", "daytime", "noon", "evening", "night"};
            TextBar.Print("It was %s", daynames[dayflag]);
        }

        int daylight = PlayScreen.Daylight();
        if (daylight != lastdaylight)
        {
            int ambient;
            SColor color;

            GetCurrentAmbient(ambient, color);
            MapPane.SetAmbient(ambient, color, FALSE); // FALSE=Allow fading to continue

            lastdaylight = daylight;
        }
    }
    lastdayflag = dayflag;
}

// Called by the game screen Animate() function to update area stuff
void TArea::Animate(BOOL draw)
{

}

// Called when the player enters the area
void TArea::Enter()
{
    S3DPoint pos;
    int level;

    MapPane.GetMapPos(pos);
    level = MapPane.GetMapLevel();

    if (flags & AREA_PLAYERIN)
        return;

    flags |= AREA_PLAYERIN;

  // Initialize cd play music
    if (flags & AREA_PLAYCDMUSIC)
        InitCDMusic();

  // Initialize ambient sounds
    if (flags & AREA_PLAYAMBIENT)
        InitAmbientSounds();

  // Set ambient values
    if (flags & AREA_SETAMBIENT)
    {
        int ambient;
        SColor color;
        GetCurrentAmbient(ambient, color);

        if (level != lastlevel || dist(pos.x, pos.y, lastpos.x, lastpos.y) > 1024)
            MapPane.SetAmbient(ambient, color);
        else
            MapPane.FadeAmbient(ambient, color, FRAMERATE * 3, 10);
    }

  // Load local scripts
    if (flags & AREA_LOADSCRIPTS)
        ScriptManager.Load(scriptfile, this);

  // Put "Entered" line in the status line
    if (Player)
        TextBar.Print("%s entered %s", Player->GetName(), name);
}

// Called when the player exits the area
void TArea::Exit()
{
    if (!(flags & AREA_PLAYERIN))
        return;

    flags &= ~AREA_PLAYERIN;

  // Close cd play music
    if (flags & AREA_PLAYCDMUSIC)
        CloseCDMusic();

  // Close ambient sounds
    if (flags & AREA_PLAYAMBIENT)
        CloseAmbientSounds();

  // Eliminate local scripts
    if (flags & AREA_LOADSCRIPTS)
        ScriptManager.Clear(this);
}

// **************************************
// * TAreaManager - Area manager object *
// **************************************

// Initialize the area manager
BOOL TAreaManager::Initialize()
{
    if (initialized)
        return TRUE;

    areas.DeleteAll();

  // Clear global last values
    lastpos.x = -100000; lastpos.y = -100000; lastpos.z = -100000;
    lastlevel = 255;
    lastdaylight = -1;
    lastdayflag = -1;

    if (!Load())
        return FALSE;

    initialized = TRUE;

    return TRUE;
}

// Closes the area manager
void TAreaManager::Close()
{
    areas.DeleteAll();
}

// Loads all areas from the "AREA.DEF" file
BOOL TAreaManager::Load()
{
    char fname[MAX_PATH];
    sprintf(fname, "%s%s", ClassDefPath, "area.def");

    FILE *fp = popen(fname, "rb");
    if (!fp)
        FatalError("Unable to find game area file AREA.DEF");

    TFileParseStream s(fp, fname);
    TToken t(s);

    if (!t.DefineGet())
        t.Error("Syntax error in header");

    while (t.Type() != TKN_EOF)
    {

        char areaname[MAXNAMELEN];
        if (!Parse(t, "AREA %s\n", areaname))
            t.Error("AREA \"name\" expected");

        PTArea area = new TArea;

        if (!area->Load(areaname, t))
            t.Error("Error loading area");
        
        areas.Add(area);

        if (!t.DefineGet())
            t.Error("Syntax error between area blocks");
    }
    
    fclose(fp);

    return TRUE;
}

// Called by the game screen Pulse() function to update area stuff
void TAreaManager::Pulse()
{
    S3DPoint pos;
    int level;
    int c;

    MapPane.GetMapPos(pos);
    level = MapPane.GetMapLevel();

    for (c = 0; c < areas.NumItems(); c++)
    {
        PTArea area = areas[c];

        BOOL in = area->In(pos, level);
        
        if (in && !(area->GetFlags() & AREA_PLAYERIN))
            area->Enter(); // In with the new
        else if (!in && (area->GetFlags() & AREA_PLAYERIN))
            area->Exit();  // This should never happen!  But just in case..
    }
    
    for (c = 0; c < areas.NumItems(); c++)
    {
        PTArea area = areas[c];

        if (area->GetFlags() & AREA_PLAYERIN)
            area->Pulse();
    }

  // Save last pos/level
    lastpos = pos;
    lastlevel = level;
}

// Called by the game screen Animate() function to update area stuff
void TAreaManager::Animate(BOOL draw)
{
    for (int c = 0; c < areas.NumItems(); c++)
    {
        PTArea area = areas[c];

        if (area->GetFlags() & AREA_PLAYERIN)
            area->Animate(draw);
    }
}
