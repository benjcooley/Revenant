// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     Player.cpp - TPlayer module                       *
// *************************************************************************

#include "revenant.h"
#include "player.h"
#include "textbar.h"
#include "sound.h"
#include "weapon.h"
#include "armor.h"
#include "statusbar.h"
#include "equip.h"
#include "death.h"
#include "playscreen.h"
#include "inventory.h"
#include "mappane.h"

extern TDeathPane DeathPane;

extern TObjectClass  CharacterClass; // Used to indicate player is derived from character

REGISTER_BUILDER(TPlayer)
TObjectClass PlayerClass("PLAYER", OBJCLASS_PLAYER, 0, &CharacterClass);

// Miscellaneous character values
DEFOBJSTAT(Player, Level,           LEV,  PLRVAL_FIRST + PLRVAL_LEVEL, 1, 1, 100)
DEFOBJSTAT(Player, Exp,             EXP,  PLRVAL_FIRST + PLRVAL_EXP, 0, 0, 1000000)

// Character stats
DEFOBJSTAT(Player, Strn,            STRN, PLRSTAT_FIRST + PLRSTAT_STRN, 14, 0, 100)
DEFOBJSTAT(Player, Cons,            CONS, PLRSTAT_FIRST + PLRSTAT_CONS, 14, 0, 100)
DEFOBJSTAT(Player, Agil,            AGIL, PLRSTAT_FIRST + PLRSTAT_AGIL, 14, 0, 100)
DEFOBJSTAT(Player, Rflx,            RFLX, PLRSTAT_FIRST + PLRSTAT_RFLX, 14, 0, 100)
DEFOBJSTAT(Player, Mind,            MIND, PLRSTAT_FIRST + PLRSTAT_MIND, 14, 0, 100)
DEFOBJSTAT(Player, Luck,            LUCK, PLRSTAT_FIRST + PLRSTAT_LUCK, 14, 0, 100)

// Character skill values
DEFOBJSTAT(Player, Attack,          ATK,  SK_FIRST + SK_ATTACK, 0, 0, 100)
DEFOBJSTAT(Player, Defense,         DEF,  SK_FIRST + SK_DEFENSE, 0, 0, 100)
DEFOBJSTAT(Player, Invoke,          INV,  SK_FIRST + SK_INVOKE, 0, 0, 100)
DEFOBJSTAT(Player, Hands,           HAN,  SK_FIRST + SK_HANDS, 0, 0, 100)
DEFOBJSTAT(Player, ShortBlades,     SBL,  SK_FIRST + SK_SHORTBLADES, 0, 0, 100)
DEFOBJSTAT(Player, LongBlades,      LBL,  SK_FIRST + SK_LONGBLADES, 0, 0, 100)
DEFOBJSTAT(Player, Bludgeons,       BLD,  SK_FIRST + SK_BLUDGEONS, 0, 0, 100)
DEFOBJSTAT(Player, Axes,            AXE,  SK_FIRST + SK_AXES, 0, 0, 100)
DEFOBJSTAT(Player, Bows,            BOW,  SK_FIRST + SK_BOWS, 0, 0, 100)
DEFOBJSTAT(Player, Stealth,         SLT,  SK_FIRST + SK_STEALTH, 0, 0, 100)
DEFOBJSTAT(Player, LockPick,        LPK,  SK_FIRST + SK_LOCKPICK, 0, 0, 100)

// Character skill experience
DEFOBJSTAT(Player, AttackExp,       ATKE, SKE_FIRST + SK_ATTACK,0, 0, 1000000)
DEFOBJSTAT(Player, DefenseExp,      DEFE, SKE_FIRST + SK_DEFENSE,0, 0, 1000000)
DEFOBJSTAT(Player, InvokeExp,       INVE, SKE_FIRST + SK_INVOKE, 0, 0, 1000000)
DEFOBJSTAT(Player, HandsExp,        HANE, SKE_FIRST + SK_HANDS, 0, 0, 1000000)
DEFOBJSTAT(Player, ShortBladesExp,  SBLE, SKE_FIRST + SK_SHORTBLADES, 0, 0, 1000000)
DEFOBJSTAT(Player, LongBladesExp,   LBLE, SKE_FIRST + SK_LONGBLADES, 0, 0, 1000000)
DEFOBJSTAT(Player, BludgeonsExp,    BLDE, SKE_FIRST + SK_BLUDGEONS, 0, 0, 1000000)
DEFOBJSTAT(Player, AxesExp,         AXEE, SKE_FIRST + SK_AXES, 0, 0, 1000000)
DEFOBJSTAT(Player, BowsExp,         BOWE, SKE_FIRST + SK_BOWS, 0, 0, 1000000)
DEFOBJSTAT(Player, StealthExp,      SLTE, SKE_FIRST + SK_STEALTH, 0, 0, 1000000)
DEFOBJSTAT(Player, LockPickExp,     LPKE, SKE_FIRST + SK_LOCKPICK, 0, 0, 1000000)

// **************
// * Skill Tree *
// **************

SSkill SkillTree[NUM_SKILLS] =
{
    { "Attack",         TRUE,   FALSE,  -1, 3 },
    { "Defence",        TRUE,   FALSE,  -1, 3 },
    { "Short Blades",   TRUE,   FALSE,  -1, 3 },
    { "Long Blades",    TRUE,   FALSE,  -1, 3 },
    { "Bludgeons",      TRUE,   FALSE,  -1, 3 },
    { "Axes",           TRUE,   FALSE,  -1, 3 },
    { "Bows",           TRUE,   FALSE,  -1, 3 },
    { "Stealth",        TRUE,   FALSE,  -1, 3 },
    { "Lockpicking",    TRUE,   FALSE,  -1, 3 },
    { "Invocation",     TRUE,   FALSE,  -1, 3 }
};

// *********************************************
// * TPlayer - Represents a game player object *
// *********************************************

// REMEMBER: !!!!! Please attempt to put MOST of player functionality into character instead.
// Player should only be the place for functionality that CAN'T be placed into character!!

TPlayer::TPlayer(PTObjectImagery newim) : TCharacter(newim)
{
    ClearPlayer();
}

TPlayer::TPlayer(PSObjectDef def, PTObjectImagery newim) : TCharacter(def, newim)
{
    ClearPlayer();
}

TPlayer::~TPlayer()
{
    PlayerManager.RemovePlayer(this);
}

void TPlayer::ClearPlayer()
{
    memset(equipment, 0, NUM_EQ_SLOTS * sizeof(PTObjectInstance));

    flags |= OF_NONMAP; // Allows us to insert object into the game map, and not
                        // worry about it getting deleted, saved, or whatnot

    OnTheHog = FALSE;
}

int deathframe = 0;

void TPlayer::Pulse()
{
    TCharacter::Pulse();

    if (!Editor)
    {
        if (Health() <= 0)
        {
            if (deathframe++ > 100)
                PlayScreen.SetNextPane(&DeathPane);
        }
        else
            deathframe = 0;
    }
}

DWORD TPlayer::Move()
{
    DWORD retval = TCharacter::Move();

    if (!OnTheHog)
        return retval;

    static BOOL riding = FALSE;
    BOOL oldriding = riding;

    if (nextmove.x || nextmove.y || nextmove.z)
        riding = TRUE;
    else
        riding = FALSE;

    if (oldriding && !riding)
    {
        SoundPlayer.Stop("hog drive");
        SoundPlayer.Play("hog idle");
    }
    else if (!oldriding && riding)
    {
        SoundPlayer.Stop("hog idle");
        SoundPlayer.Play("hog drive");
    }

    return retval;
}

void TPlayer::Damage(int damage, int type)
{
    TCharacter::Damage(damage, type);
}

int TPlayer::GetResistance(int type)
{
    return 0; // Huhh?
}

int TPlayer::ResolveCombat(PTActionBlock ab, int bits)
{
    if (ab->firsttime && !ab->transition && ab->Is("combat"))
        PLAY("draw sword");

    return TCharacter::ResolveCombat(ab, bits);
}

void TPlayer::RefreshEquip()
{
    for (TInventoryIterator i(this); i; i++)
    {
        if (i.Item()->InventNum() > 255)
            Equip(i.Item(), i.Item()->InventNum() - 256);

        if (this == Player)
            EquipPane.SetDirty(TRUE);
    }
}

void TPlayer::Equip(PTObjectInstance oi, int slot)
{
    if ((DWORD)slot < NUM_EQ_SLOTS)
    {
        equipment[slot] = oi;
        if (this == Player)
            EquipPane.SetDirty(TRUE);
    }
}

// Returns the modifier value for the given player stat
int TPlayer::PlyrStatMod(int plyrstat, int flags)
{
    int mod;

    switch (PlyrStat(plyrstat))
    {
      case 0: case 1:  case 2:
        mod = -4;
        break;
      case 3: case 4: case 5: case 6:
        mod = -3;
        break;
      case 7: case 8: case 9:
        mod = -2;
        break;
      case 10: case 11:
        mod = -1;
        break;
      case 12: case 13: case 14: case 15:
        mod = 0;
        break;
      case 16: case 17:
        mod = 1;
        break;
      case 18: case 19: case 20:
        mod = 2;
        break;
      case 21: case 22: case 23: case 24:
        mod = 3;
        break;
      case 25: case 26: case 27: case 28: case 29:
        mod = 4;
        break;
      default:
        if (plyrstat < 0)
            mod = -5;
        else
            mod = 5;
        break;
    }

    if (flags == PLYRSTATMOD_HALFROUNDUP)
    {
        if (mod > 0)
            mod = (mod + 1) / 2;
        else
            mod = (mod - 1) / 2;
    }
    else if (flags == PLYRSTATMOD_HALFROUNDDOWN)
    {
        mod = mod / 2;
    }

    return mod;
}

// Returns a modifier value for skill based on a physical attribute
int TPlayer::PlyrStatSkillMod(int skill)
{
    return 0;
}

void TPlayer::SetHealth(int v)
{
    TCharacter::SetHealth(v);

    if (this == Player)
        HealthBar.ChangeLevel(v * 1000 / MaxHealth());
}

void TPlayer::SetFatigue(int v)
{
    TCharacter::SetFatigue(v);

//  if (this == Player)
//      StaminaBar.ChangeLevel(v * 1000 / MaxFatigue());
}

void TPlayer::SetMana(int v)
{
    TCharacter::SetMana(v);

    if (this == Player)
        StaminaBar.ChangeLevel(v * 1000 / MaxMana());
}

// Special cheat function
void TPlayer::GetOnYerHog()
{
    int newtype = PlayerClass.FindObjType(OnTheHog ? "Locke" : "Hog");
    if (newtype < 0)
        return;

    if (animator)
    {
        delete animator;
        animator = NULL;
    }
    
    TObjectImagery::FreeImagery(imagery);
    imagery = TObjectImagery::LoadImagery(PlayerClass.GetObjType(newtype)->imageryid);
    if (!imagery)
        return;

    objtype = newtype;

    if (!OnTheHog)
    {
        SoundPlayer.Mount("hog idle");
        SoundPlayer.Mount("hog drive");

        SoundPlayer.Play("hog idle");

        OnTheHog = TRUE;
    }
    else
    {
        SoundPlayer.Unmount("hog idle");
        SoundPlayer.Mount("hog drive");

        OnTheHog = FALSE;
    }
}

// *******************************************************************
// * TPlayerManager - Object to manager the player list for the game *
// *******************************************************************

// Initialize the player manager
BOOL TPlayerManager::Initialize()
{
    players.Clear();
    mainplayernum = -1;
    Player = NULL;

    return TRUE;
}

// Closes the player manager
void TPlayerManager::Close()
{
    players.DeleteAll();
    Player = NULL;
    mainplayernum = -1;
}

// Clears the player manager (deletes all players for the current game)
void TPlayerManager::Clear()
{
    SetMainPlayer(-1);
    players.DeleteAll();
}

// Sets the main player for the game
void TPlayerManager::SetMainPlayer(int newplayernum)
{
    if (mainplayernum == newplayernum)
        return;

    mainplayernum = newplayernum;

    if ((DWORD)mainplayernum >= (DWORD)players.NumItems())
        mainplayernum = players.NumItems() - 1;  // The last player added

    if (mainplayernum < 0)
    {
        Player = NULL;

        if (CurrentScreen == &PlayScreen)
        {
          // Set map position
            S3DPoint pos;
            MapPane.GetMapPos(pos);
            MapPane.CenterOnPos(pos, MapPane.GetMapLevel(), FALSE);

          // Set inventory container
            Inventory.SetContainer(NULL);

          // Setup health bars
            HealthBar.SetLevel(0);
            StaminaBar.SetLevel(0);
        }
    }
    else
    {
        Player = GetPlayer(mainplayernum);

        if (CurrentScreen == &PlayScreen)
        {
          // Center on this player in map
            MapPane.CenterOnObj(Player, FALSE);

          // Setup equipment pane
            Player->RefreshEquip();

          // Setup inventory pane
            Inventory.SetContainer(Player);

          // Set status bars
            HealthBar.SetLevel(Player->Health() * 1000 / Player->MaxHealth());
            StaminaBar.SetLevel(Player->Mana() * 1000 / Player->MaxMana());
        }
    }
}

// Sets the main player for the game
void TPlayerManager::SetMainPlayer(PTPlayer player)
{
    int newplayernum = -1;

    for (int c = 0; c < players.NumItems(); c++)
    {
        if (players[c] == player)
        {
            newplayernum = c;
            break;
        }
    }

    SetMainPlayer(newplayernum);
}

// Add player
int TPlayerManager::AddPlayer(PTPlayer newplayer)
{
    int c;
    int empty = -1;
    for (c = 0; c < players.NumItems(); c++)
    {
        if (players[c] == NULL)
            empty = c;
        else if (players[c] == newplayer)
            return c;
    }

    if (empty >= 0)
        c = players.Set(newplayer, empty);
    else
        c = players.Add(newplayer);

    if (!Player)
        SetMainPlayer(newplayer);

    return c;
}

// Remove a player
// The collapse flag allows you to choose whether you want to leave an empty space
// in the player list or not.  You need to leave empty slots when in a net game so that
// player id numbers don't change.  When setting up a game, however, you need to collapse
// the list.
void TPlayerManager::RemovePlayer(int removenum, BOOL collapse)
{
    if ((DWORD)removenum >= (DWORD)players.NumItems())
        return;

    if (collapse)
        players.Collapse(removenum, FALSE);
    else
        players.Remove(removenum);

    if (removenum == mainplayernum)
        SetMainPlayer(-1); // Set to last player in list
}

// Remove a player (given player pointer)
void TPlayerManager::RemovePlayer(PTPlayer removeplayer, BOOL collapse)
{
    int removenum;
    for (removenum = 0; removenum < players.NumItems(); removenum++)
    {
        if (players[removenum] == removeplayer)
        {
            RemovePlayer(removenum, collapse);
            return;
        }
    }
}       
