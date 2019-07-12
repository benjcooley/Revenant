// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   SaveGame.cpp - savegame objects                     *
// *************************************************************************

#include "revenant.h"
#include "playscreen.h"
#include "mappane.h"
#include "sector.h"
#include "player.h"
#include "inventory.h"
#include "automap.h"
#include "io.h"
#include "multictrl.h"
#include "statusbar.h"
#include "script.h"
#include "dls.h"
#include "savegame.h"

#define NOTHING     0
#define CONTENTS    1
#define NEXT        2

#define DATA_SLOTS      32      // number of extra data slots in the savefile

BOOL TSaveGame::WriteGame(char *name)
{
    if (!name)
        name = "game01.sav";

    if (!Player)
        return FALSE;

    if (saved != Player)
        saved = Player;

    pane = MultiCtrl.GetActivePane();
    gametime = PlayScreen.GameTime();
    version = MAP_VERSION;

    TOutputStream os(32768, 16384);

    FILE *fp;
    fp = popen(name, "wb");
    if (!fp)
        return FALSE;

    BOOL retval = TRUE;

    fseek(fp, 0, 0);

    // Write out the Auto Map data first
    if (AutoMap.WriteAutoMapData(fp) == -1)
        retval = FALSE;
    else
    {
        // Then some other random info...
        // Note there are DATA_SLOTS integers here for adding more extra info
        // to the save file without ruining any existing ones
        int data[DATA_SLOTS];
        memset(data, 0, sizeof(int) * DATA_SLOTS);
        data[0] = gametime;
        data[1] = pane;
        data[7] = version;

        if (fwrite(data, sizeof(int), DATA_SLOTS, fp) < DATA_SLOTS)
            retval = FALSE;
        else
        {
            // Finally Player himself (including all inventory and equipment)
            TObjectInstance::SaveObject(saved, os);
            if (fwrite(os.Buffer(), os.DataSize(), 1, fp) < 1)
                retval = FALSE;
        }
    }

    fclose(fp);

    return retval;
}

BOOL TSaveGame::WriteGame(int gamenum)
{
    char name[12];

    if (gamenum < 0)
        gamenum = 0;

    sprintf(name, "game%02d.sav", gamenum);

    return WriteGame(name);
}

BOOL TSaveGame::ReadGame(char *name)
{
    loading = TRUE;

    if (!name)
        name = "game01.sav";

  // Kill the currently loaded map (return to original map)
    MapPane.ClearCurMap();

  // Reload initial game states
    ScriptManager.ReloadStates();

  // Clear the current player list
    PlayerManager.Clear();
    
  // Now attempt to load the game
    BOOL retval = TRUE;

    FILE *fp = popen(name, "rb");
    if (fp == NULL)
    {
        loading = FALSE;
        return FALSE;
    }
    size_t bufsize = _filelength(fileno(fp));
    BYTE *buf = NULL;

    if (bufsize > 0)
    {
        // Keep track of how big the Auto Map data is
        size_t size;

        size = AutoMap.ReadAutoMapData(fp);
        if (size == -1)
            retval = FALSE;
        else
        {
            // Read in the spare data
            bufsize -= DATA_SLOTS * sizeof(int);
            int data[DATA_SLOTS];

            if (fread(data, sizeof(int), DATA_SLOTS, fp) < DATA_SLOTS)
                retval = FALSE;
            else
            {
                // Transfer the data to the appropriate values
                gametime = data[0];
                pane = data[1];
                version = data[7];

                // Now adjust bufsize so we can read in the streamed object list
                bufsize -= size;

                buf = (BYTE *)malloc(bufsize);

                if (fread(buf, bufsize, 1, fp) < 1)
                    retval = FALSE;
            }
        }
    }
    else
    {
        buf = (BYTE *)malloc(sizeof(int));
        bufsize = 4;
        *((int *)buf) = 0;
    }

    fclose(fp);

    if (version < 3)        // first version supporting recursive object writes
        FatalError("game01.sav is an old version, get rid of it please");

    if (retval)
    {
        TInputStream is(buf, bufsize);
        saved = TObjectInstance::LoadObject(is, version);
    }

    if (buf)
        free(buf);

    if (!saved)
    {
        loading = FALSE;
        return FALSE;
    }

  // Set visible pane
    MultiCtrl.ActivatePane(pane);

  // Adds the player into the player manager
    PlayerManager.AddPlayer((PTPlayer)saved);

  // Tell system this is our main player
    PlayerManager.SetMainPlayer((PTPlayer)saved);

  // Restore game time
    PlayScreen.SetGameTime(gametime);

  // Redraw whole window
    PlayScreen.Redraw();

    loading = FALSE;
    return retval;
}

BOOL TSaveGame::ReadGame(int gamenum)
{
    char name[12];
    
    if (gamenum < 0)
        gamenum = 0;

    sprintf(name, "game%02d.sav", gamenum);

    return ReadGame(name);
}
