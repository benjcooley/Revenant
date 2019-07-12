// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 character.cpp - TCharacter module                     *
// *************************************************************************

#include <windows.h>
#include <math.h>
#include <string.h>

#include "revenant.h"
#include "character.h"
#include "rules.h"
#include "mappane.h"
#include "display.h"
#include "spell.h"
#include "sound.h"
#include "playscreen.h"
#include "multi.h"
#include "animation.h"
#include "statusbar.h"
#include "dialog.h"
#include "effect.h"
#include "textbar.h"
#include "3dimage.h"
#include "inventory.h"
#include "exit.h"
#include "script.h"
#include "effect.h"
#include "charanimator.h"
#include "weapon.h"
#include "player.h"

#define GRIDSNAPSIZE    48

REGISTER_BUILDER(TCharacter)
TObjectClass CharacterClass("CHARACTER", OBJCLASS_CHARACTER, 0);

extern TObjectClass EffectClass;

// Hard coded character class stats
DEFSTAT(Character, Radius,          RAD,  0, 16, 16, 256)

// Hard coded character object stats
DEFOBJSTAT(Character, Aggressive,   AGGR, CHRFLAG_FIRST + CHRFLAG_AGGRESSIVE, 1, 0, 1)
DEFOBJSTAT(Character, Poisoned,     PSND, CHRFLAG_FIRST + CHRFLAG_POISONED, 0, 0, 1)
DEFOBJSTAT(Character, Sleeping,     SLP,  CHRFLAG_FIRST + CHRFLAG_SLEEPING, 0, 0, 1)

DEFOBJSTAT(Character, Health,       HLT,  CHRSTAT_FIRST + CHRSTAT_HEALTH, 25, 0, 10000)
DEFOBJSTAT(Character, Fatigue,      FAT,  CHRSTAT_FIRST + CHRSTAT_FATIGUE, 25, 0, 10000)
DEFOBJSTAT(Character, Mana,         MAN,  CHRSTAT_FIRST + CHRSTAT_MANA, 25, 0, 10000)

extern TDialogPane DialogPane;

// Some character defines
#define IMPACT_THRESHOLD    4   // how much damage has to be done before they enter impact state
#define MAXENEMYRANGE       512 // Maximum range of enemy

// *************************************************************
// *                          Character                        *
// *************************************************************

void TCharacter::ClearChar()
{
    flags |= OF_MOVING | OF_PULSE | OF_ANIMATE | OF_AI; // Is moving object, pulse, animate, and has AI

    waittype = WAIT_NOTHING;
    waitticks = 0;
    forcecommanddone = FALSE;
    forcenomove = FALSE;
    is_invisible = FALSE;

    // When character loaded or created, make exit timestamp current..
    // Prevents ONEXIT flag from expiring when character is saved on top of a
    // destination exit.
    exittimestamp = CurrentScreen->FrameCount();

  // Set root state
  // NONE: DefaultRootState() will NOT be virtual when ClearChar()
  // is called in the constructor!
    if (!root)
        root = doing = desired = new TActionBlock(DefaultRootState() /* SEE ABOVE NOTE */);
    else
        strcpy(root->name, DefaultRootState());

    int newstate = FindState(root->name);
    if (newstate < 0)
        newstate = FindState("walk");
    if (newstate < 0)
        newstate = FindState("combat");
    if (newstate < 0)
        newstate = 0;
    state = newstate;

  // Set character data pointer
    chardata = Rules.GetCharData(objtype, objclass);

  // Set initial health/fatigue/mana values
    SetHealth(MaxHealth());
    SetFatigue(MaxFatigue());
    SetMana(MaxMana());

  // Reset AI data
    nextattack = -1;
    shovedir = -1;                // Block go around direction choice
    glimpse = noise = -1;         // Reset glimpse and noise values

  // Clear lastattack stuff
    lastattacknum = -1;
    lastattack = NULL;
    lastattackticks = 0;
}

void TCharacter::Pulse()
{
//  if (!animator)  // Quick hack to fix super slodown... BEN  (if not on screen, ignore me)
//      return; 

    if (forcecommanddone)
    {
        if (doing)
            doing->wait = 0;

        commanddone = TRUE;
        forcecommanddone = FALSE;
    }

    // Is the character doing an autocombo?
    if (lastattack && (lastattack->flags & CA_AUTOCOMBO) && 
        PlayScreen.GameFrame() == lastattackticks + lastattack->nextwait)
    {
        SpecificAttack(lastattacknum + 1); // Do the next attack boyz
    }

    TComplexObject::Pulse();

    // Signal processing - let other characters know what's going on
    PTCharacter target = Fighting();
    if (target)
    {
        target->SignalHostility(this, target);
        if ((PTPlayer)this == Player)
            TextBar.SetHealthDisplay(target->GetName(), target->Health());
    }

    // Check to see if exit flag has expired (exit flags are set by exit objects)
    // Exits search through character list to tag character that it is on an exit.  Characters
    // then check the time stamp and clear themselves after two frames.  This system prevents
    // the old reflective exit bug, where an exit takes a character to another exit, which
    // then takes him back to the first... etc. etc. as the character can only activate an
    // exit when he was previously (in the past two frames) not already on one.
    if (exittimestamp < CurrentScreen->FrameCount() - 2)
        SetFlag(OF_ONEXIT, FALSE);

    if (IsDead())
    {
        Halt(); // Don't go anywhere!
        
        if (!IsDoing(ACTION_DEAD))
        {
            if (FindState("dead") >= 0)
                ForceCommand(new TActionBlock("dead", ACTION_DEAD));
        }

        if (script) // Hey Mr. Script.. , I'm dead now...
            script->Trigger(TRIGGER_DEAD);
        return;         // all processing below is for alive characters only
    }

    // Do the character's AI now
    AI();

    // Recover health, fatigue, and mana
    // This code checks last recovery time stamps (in game 100ths of a second) to see
    // if the health,fatigue, or mana for a character need to be updated.  The time stamps
    // are actually saved with the character data, so even if a character has been unloaded
    // for a while, when he is reloaded, he will have the proper health,mana,and fatigue based
    // on the elapsed game time.
    int gametime = PlayScreen.GameTime();
    int pcnt;
    if (Health() < MaxHealth() && gametime - lasthealthrecov >= Rules.healthrecovrate)
    {
        pcnt = ((gametime - lasthealthrecov) / Rules.healthrecovrate) * Rules.healthrecovval;
        SetHealth(max(MaxHealth(), Health() + (MaxHealth() * pcnt / 100)));
        lasthealthrecov = gametime;
    }
    if (Fatigue() < MaxFatigue() && gametime - lastfatiguerecov >= Rules.fatiguerecovrate)
    {
        pcnt = ((gametime - lastfatiguerecov) / Rules.fatiguerecovrate) * Rules.fatiguerecovval;
        SetFatigue(max(MaxFatigue(), Fatigue() + (MaxFatigue() * pcnt / 100)));
        lastfatiguerecov = gametime;
    }
    if (Mana() < MaxMana() && gametime - lastmanarecov >= Rules.manarecovrate)
    {
        pcnt = ((gametime - lastmanarecov) / Rules.manarecovrate) * Rules.manarecovval;
        SetMana(max(MaxMana(), Mana() + (MaxMana() * pcnt / 100)));
        lastmanarecov = gametime;
    }

    // Handle affects
/*  if (IsPoisoined()poison > 0)
    {
        poisonaccum += poison;
        int dam = poisonaccum / POISON_SPEED;
        if (dam)
        {
            Damage(dam, DAMAGE_POISON);
            poisonaccum -= dam * POISON_SPEED;

            // give a chance for it to wear off
//          if (GetResistance(DAMAGE_POISON) >= random(0, 100) || random(0, 20) == 7)
//              poison--;
        }
    }
*/
}

void TCharacter::UpdateAction(int bits)
{
  // Reset stealth values for every move (when not in root state)
    if ((commanddone && root != doing) || glimpse < 0 || noise < 0)
        ResetStealthValues();

  // Set sleep to awake if doing non sleeping root states
    if (root->Is("walk") || root->Is("combat"))
        SetSleeping(0);

  // ********************************************************************
  // Animation 'commanddone' state is set, now process this state through
  // the ACTION handlers.  If nothing sets the command state, we set it by
  // seeing if the character is doing anything.  This is defined as either
  // being in the root state, having the current animation completed.
    int comstate = ResolveAction(bits);
    if (comstate == 0)
        comstate = (commanddone || !animator || (flags & OF_INVISIBLE)) ? // If done, or not animating..                                        // Done if finished animating
            COM_COMPLETED : COM_EXECUTING;

  // Reset priority when command complete   
    if (comstate == COM_COMPLETED && doing->priority)
        doing->priority = FALSE;

  // Set firstime flag to FALSE
    if (doing->firsttime)
        doing->firsttime = FALSE;

  // ********************************************************************
  // Now continue the script (before the code below sets the Action Block for the
  // next frame.  The script is currently paused if this code is running.  Ordinary
  // script CMD_WAITS always wait on the next command being finished (WAIT_NOTHING).
  // Special waits can set the WAIT_ flags to wait on other things.
    if (GetScript())
    {
        BOOL continuescript = FALSE;

        if (waittype == WAIT_NOTHING && comstate == COM_COMPLETED) // Ordinary wait action done
            continuescript = TRUE;
        else if (waittype == WAIT_TICKS)                           // Wait for ticks done
        {
            waitticks--;
            if (waitticks <= 0)
                continuescript = TRUE;  
        }
        else if (waittype == WAIT_RESPONSE && DialogPane.HasResponded())  // Wait for a response    
        {
            ScriptJump(DialogPane.GetResponseLabel());
            waittype = WAIT_NOTHING;
            continuescript = TRUE;
            DialogPane.Hide(); // Done with dialog pane, now hide it
        }
        else if (waittype == WAIT_CHAR_DONE && comstate == COM_COMPLETED)
        {
            if (!doing->obj || ((PTCharacter)doing->obj)->IsInRoot())
                continuescript = TRUE;
        }

      // NEXT LINE: Next line in script
        if (continuescript)
        {
            commanddone = TRUE;             // Force animation state to done
            comstate = COM_COMPLETED;       // Force command state to COMPLETED
            if (desired)                    // Break whatever is animating!
                desired->priority = FALSE;
            if (doing)                      // Break whatever is animating!
                doing->priority = FALSE;
            waittype = WAIT_NOTHING;        // Don't wait anymore
            ContinueScript();
        }
    }


  // ********************************************************************
  // Now set the action blocks for the next frame.  The rules are:
  //
  // If an action block is compleded, and it's not a ROOT2ROOT block, it
  // just sits there with the desired and doing doing the same thing.
  // If the animation is a looping animation, it resets the frame and does
  // it again.  If the animation is ROOT2ROOT, it just transitions back to
  // the root state.

  // Cause character to fall if move bits flag has fall
    if (bits & MOVE_FALLING)
        comstate = ForceCommand(new TActionBlock("fall"), bits);
    else
        comstate = TryCommand(desired, bits);

  // ********************************************************************
  // Decrement the wait value (if any)

    if (doing && doing->wait > 0)
        doing->wait--;
}

int TCharacter::ResolveAction(int bits)
{
    int comstate = TComplexObject::ResolveAction(bits);
    if (comstate != 0)
        return comstate;

  // Check for movement 
    if (doing->action == ACTION_MOVE)
        comstate = ResolveMove(doing, bits);
    else if (doing->action == ACTION_ATTACK) 
        comstate = ResolveAttack(doing, bits);
    else if (doing->action == ACTION_INVOKE)
        comstate = ResolveInvoke(doing, bits);
    else if (doing->action == ACTION_IMPACT || doing->action == ACTION_KNOCKDOWN || doing->action == ACTION_STUN)
        comstate = ResolveImpact(doing, bits);
    else if (doing->action == ACTION_BLOCK)
        comstate = ResolveBlock(doing, bits);
    else if (doing->action == ACTION_COMBATMOVE)
        comstate = ResolveCombatMove(doing, bits);
    else if (doing->action == ACTION_COMBAT)
        comstate = ResolveCombat(doing, bits);
    else if (doing->action == ACTION_COMBATLEAP)
        comstate = ResolveLeap(doing, bits);
    else if (doing->action == ACTION_SAY)
        comstate = ResolveSay(doing, bits);
    else if (doing->action == ACTION_PULL)
        comstate = ResolvePull(doing, bits);
    else if (doing->action == ACTION_PIVOT)
        comstate = ResolvePivot(doing, bits);
    else if (doing->action == ACTION_DEAD)
        comstate = ResolveDead(doing, bits);
    
    return comstate;
}

void TCharacter::Animate(BOOL draw)
{
    TObjectInstance::Animate(draw);

    if (animator && draw && doing && 
        doing->action == ACTION_SAY && 
        doing->data &&
        doing->wait > 12) // Leave a half a second between sentences
    {
        SRect r;
        Display->GetClipRect(r);

        SColor color = { 0, 150, 255 };

        if (this == (PTCharacter)Player)
        {
            color.red = 200;
            color.green = 0;
            color.blue = 0;
        }

        int x, y;
        WorldToScreen(pos, x, y);

        PlayScreen.AddPostCharText((char *)doing->data, x, y - 120, &color, r.right - r.left + 1 - 32);
    }
}

void TCharacter::Notify(int notify, void *ptr)
{
    // **** WARNING!!! MAKE SURE YOU CHECK FOR BROKEN LINKS AND DELETED OBJECTS HERE!!! ****
    // If you want to be notified, you must call SetNotify() in your contsructor

    TComplexObject::Notify(notify, ptr);
}

#define MAXZMOVE 32

BOOL TCharacter::Blocked(S3DPoint &pos, S3DPoint &newpos, DWORD bits, int *height, PTCharacter *bychar)
{
    int h;
    if (!height)
        height = &h;

  // Get walk map values in current area
    int mindelta, maxdelta;
    if (bits & MOVE_NOTMOVING)
    {
        *height = MapPane.GetWalkHeight(newpos);  // If not moving, just get what's under us
        maxdelta = mindelta = 0;
    }
    else
    {
      // If moving, check Radius() around us to see if changes (deltas) between walk
      // grids go to far up (maxdelta - walls, barriers), or too far down (mindelta -
      // holes, etc.)  Also returns height underneath us.
        MapPane.GetWalkHeightRadius(newpos, Radius(), mindelta, maxdelta, *height);
    }

    int aniflags = GetAniFlags();

    PTCharacter dummybychar;
    if (!bychar)
        bychar = &dummybychar;
    *bychar = NULL;

  // Are we blocked? 
    BOOL blocked = !(bits & MOVE_FALLING) && 
        (abs(pos.z - *height) > MAXZMOVE || 
        abs(maxdelta) > MAXZMOVE || abs(mindelta) > MAXZMOVE ||
        *height == 0 || (!(aniflags & AF_FLY) && (*bychar = CharBlocking(this, newpos))));
         
    return blocked;
}

DWORD TCharacter::Move()
{
    if (flags & OF_IMMOBILE || inventnum >= 0 || IsDead())
        return MOVE_NOTHING;

    DWORD retval = 0;

    int h = MapPane.GetWalkHeight(pos);
    int d = pos.z - h;

    if (d < -16)
        pos.z = h;
    else if (d > 16)
    {
        vel.z = max(vel.z - GRAVITY, -TERMINAL_VELOCITY);
        retval |= MOVE_FALLING;
    }

    if (d < 1)
        vel.z = 0;

    S3DPoint tmpaccum = accum;

    if (forcenomove)
        forcenomove = FALSE;
    else
        tmpaccum += nextmove;

    tmpaccum += vel;

    S3DPoint newpos = pos;
    rollover(tmpaccum.x, newpos.x);
    rollover(tmpaccum.y, newpos.y);
    rollover(tmpaccum.z, newpos.z);

    if (newpos == pos &&
        nextmove.x == 0 && nextmove.y == 0 && nextmove.z == 0 &&
        vel.x == 0 && vel.y == 0 && vel.z == 0)
            retval |= MOVE_NOTMOVING;

    int nh;

  // Check to see if character is blocked
    if (Blocked(pos, newpos, retval, &nh)) // New position is blocked
    {
      // Check to see if character is totally stuck...
        if (!Blocked(pos, pos, retval, &nh))
            retval |= MOVE_BLOCKED; // If he's not totally stuck, report him as blocked
    }
    else
    {
        shovedir = -1;      // Not blocked anymore!  Reset block direction choice

        vel.x = vel.y = vel.z = 0;

        if (newpos.z != nh)
            if (vel.z < 0)
                newpos.z = nh;
            else
                newpos.z += (nh - newpos.z) / 2;

        if (retval & MOVE_NOTMOVING)
        {
            accum = tmpaccum;
            return MOVE_NOTHING;        // can only check this _after_ gravity
        }
    }

    // If blocked, check in front of char at walk granularity intervals for an open
    // path, if we find one, shove the character in that direction, and save the dir
    // in 'shovedir' so we keep shoving in the same direction until the character is 
    // stuck, or he's around the obstacle.
    if (retval & MOVE_BLOCKED) // Try to nudge!
    {
        // check nearby squares in front of char to try to find open path
        if (shovedir < 0)
        {
            for (int i = 0; i < 4; i++)
            {
                int d;
                if (i & 1)
                    d = WALKMAPGRANULARITY << (i >> 1);
                else
                    d = -(WALKMAPGRANULARITY << (i >> 1));

                S3DPoint v, p;
//              p = pos;
//              ConvertToVector(moveangle, 6, v);
//              p += v;
                p = newpos;
                ConvertToVector(moveangle + 64, d, v);
                p += v;

              // Check to see if there's open space to right/left
                if (!Blocked(pos, p, retval)) // This way's ok
                {
                    if (d < 0)
                        shovedir = (moveangle - 64) & 255; // Go left around obstacle
                    else
                        shovedir = (moveangle + 64) & 255; // Go right around obstacle
                    break;
                }
            }
        }

        if (shovedir >= 0)
        {
            S3DPoint v;
            ConvertToVector(shovedir, 4, v);
            newpos = pos;
            newpos += v;
            tmpaccum = accum;
            if (!Blocked(pos, newpos, retval)) // This way's ok
                retval &= ~MOVE_BLOCKED; // Go ahead and move there
        }
    }

    if (!(retval & MOVE_BLOCKED))
    {
        retval |= MOVE_MOVED;

        accum = tmpaccum;
        if (newpos != pos)
            SetPos(newpos);
    }

    return retval;
}

int TCharacter::GetDamageType(int weapontype, int attackflags)
{
    if (weapontype == WT_HAND)
    {
        return DT_HAND;
    }
    else if (weapontype == WT_SHORTBLADE || weapontype == WT_LONGBLADE)
    {
        if (attackflags & CA_THRUST)
            return DT_PUNCTURE;
        else if (attackflags & CA_SLASH)
            return DT_CUT;
        else if (attackflags & CA_CHOP)
            return DT_CHOP;
    }
    else if (weapontype == WT_BLUDGEON)
    {
        if (attackflags & CA_THRUST)
            return DT_NONE;
        else if (attackflags & CA_SLASH)
            return DT_BLUDGEON;
        else if (attackflags & CA_CHOP)
            return DT_BLUDGEON;
    }
    else if (weapontype == WT_AXE)
    {
        if (attackflags & CA_THRUST)
            return DT_NONE;
        else if (attackflags & CA_SLASH)
            return DT_CHOP;
        else if (attackflags & CA_CHOP)
            return DT_CHOP;
    }
    else if (weapontype == WT_BOW)
    {
        return DT_NONE; // Bow can't hurt anybody, only arrows
    }

    return DT_NONE;
}

// Get the total damage amount (based on this function)
int TCharacter::CalculateDamage(int damage, int damagetype, int modifier)
{
    int attackdamage, totaldamage;
    if (damagetype == DT_NONE)
        return 0;
    else
    {
        attackdamage = damage * (100 + modifier) / 100;
        totaldamage = attackdamage * (100 + DamageModifier(damagetype)) / 100;
    }

    return totaldamage;
}

void TCharacter::Damage(int damage, int damagetype, int modifier,
    PTActionBlock impact, PTActionBlock death)
{
  // Calculate total damage
    if (damagetype >= 0)
        damage = CalculateDamage(damage, damagetype, modifier);

  // Apply damage to low level object
    TObjectInstance::Damage(damage);

  // Do death...
    if (Health() < 1)
    {
        if(!random(0, 2))
        {
            S3DPoint vel;
            vel.x = random(-8, 8);
            vel.y = random(-8, 8);
            vel.z = random(7, 12); 
            int count = random(6, 16);
            Pulp(vel, count, count * 30);
        }

        if (impact)
            delete impact;
        if (!death)
        {
            if (FindState("dead") < 0) // Doesn't have death, so remove character
            {
                SetFlag(OF_KILL); 
                return;
            }
            death = new TActionBlock("dead", ACTION_DEAD);
        }
        death->obj = doing->obj;
        death->interrupt = TRUE;
        ForceCommand(root);     // Make sure we play "combat to" transitions
        ForceCommand(death);
    }

  // Or do impact...
    else
    {
        if (death)
            delete death;
        if (!impact)
        {
            if (!HasActionAni("impact"))  // Do we have state, or transition state?
                return;
            impact = new TActionBlock("impact", ACTION_IMPACT);
        }
        impact->obj = doing->obj;
        impact->interrupt = TRUE;
        ForceCommand(root);     // Make sure we play "combat to" transitions
        ForceCommand(impact);
    }
}

void TCharacter::RestoreHealth()
{
    SetPoisoned(FALSE);
    SetHealth(MaxHealth());
}

// DLS brightness routine (gives brightness given distance)
extern double GetLightBrightness(int dist, int intensity, int multiplier);

// Returns the total visibility 1-100 for character (based on lights, ambient, and fog, etc.)
int TCharacter::Visibility()
{
    int brightness = 0;

    for (TMapIterator i(*this, CHECK_MAPRECT | CHECK_NOINVENT, OBJSET_LIGHTS); i; i++)
    {
        brightness += i->GetIllumination(this);
    }
    
    if (brightness <= 0)
        brightness = (int)(min(GetLightBrightness(512, 256, 40), 1.0) * 255.0); 
                    // Use out of range light value (distance=512) to
                    // get ambient light intensity (256 and 40 don't matter)

    if (brightness > 255)
        brightness = 255;

    return brightness * 100 / 255;
}

void TCharacter::AdvanceAngles(int faceang, int moveang, int maxturn)
{
    int dif;

  // Do face angle
    faceang &= 255;

    dif = faceang - GetFace();
    if (dif > 128)
        dif = dif - 256;
    if (dif < -128)
        dif = dif + 256;

    int newfaceang;
    if (dif < 0)
        newfaceang = (GetFace() + max(dif, -maxturn)) & 255;
    else
        newfaceang = (GetFace() + min(dif, maxturn)) & 255;

  // Do move angle
    moveang &= 255;

    dif = moveang - GetMoveAngle();
    if (dif > 128)
        dif = dif - 256;
    if (dif < -128)
        dif = dif + 256;

    int newmoveang;
    if (dif < 0)
        newmoveang = (GetMoveAngle() + max(dif, -maxturn)) & 255;
    else
        newmoveang = (GetMoveAngle() + min(dif, maxturn)) & 255;

    if (newfaceang != GetFace())
        FaceOnly(newfaceang);

    if (newmoveang != GetMoveAngle())
        SetMoveAngle(newmoveang);
}

#define MAXCHANGE   200
#define STOPHEIGHT  50

int TCharacter::UpdateAngle(int angle)
{
/*
    int ba = angle;
    int bd = 1000000;

    for (int a = angle - 16; a < angle + 16; a += 1)
    {
        S3DPoint vect;
        ConvertToVector(a, 250, vect);

        int lh = MapPane.GetWalkHeight(pos), delta = 0, h;
        S3DPoint targ = pos, i = pos;
        targ += vect;

        if (vect.x == 0)
        {
            for (i.y = pos.y + 1; i.y < targ.y; i.y++)
            {
                h = MapPane.GetWalkHeight(i);
                delta += absval(h - lh);
                lh = h;

                if (h > STOPHEIGHT || delta > MAXCHANGE)
                    break;
            }
        }
        else if (vect.y == 0)
        {
            for (i.x = pos.x + 1; i.x < targ.x; i.x++)
            {
                h = MapPane.GetWalkHeight(i);
                delta += absval(h - lh);
                lh = h;

                if (h > STOPHEIGHT || delta > MAXCHANGE)
                    break;
            }
        }
        else
        {
            int dist = (int)sqrt((double)(sqr(vect.x) + sqr(vect.y)));
            float yr = (float)vect.y / (float)dist;
            float xr = (float)vect.x / (float)dist;
            S3DPoint oldpoint = i;

            for (int j = 1; j < dist; j++)
            {
                i.x = (int)(xr * (float)j);
                i.y = (int)(yr * (float)j);
                i += pos;

                if (oldpoint.x != i.x && oldpoint.y != i.y)
                {
                    h = MapPane.GetWalkHeight(i);
                    delta += absval(h - lh);
                    lh = h;
                }

                oldpoint = i;

                if (h > STOPHEIGHT || delta > MAXCHANGE)
                    break;
            }
        }

        int d = (int)sqrt((double)SQRDIST(i, targ));

        if (d < bd)
        {
            bd = d;
            ba = a;
        }
    }

    return ba;
*/
    int ch = MapPane.GetWalkHeight(pos);
    int nudge = 0;
    S3DPoint c;
    c.z = 0;

    // hum...need to make this keep some sort of static nudge
    // value in order to avoid the quivering affect.
    // possibly need to reset that when the target location
    // is changed, as well.
    // also - in the case of a coridor, need to make sure that
    // no matter what way they are facing it always sends them
    // straight down it.

    for (c.y = pos.y - 32; c.y <= (pos.y + 32); c.y += 16)
        for (c.x = pos.x - 32; c.x <= (pos.x + 32); c.x += 16)
        {
            if (c.y == pos.y && c.x == pos.x)
                continue;

            if (absval(MapPane.GetWalkHeight(c) - ch) < 30)
                continue;

            int dist = (sqr(64) - SQRDIST(c, pos)) / 100;
            dist = min(50, max(1, dist));

            int ang = ConvertToFacing(pos, c) - GetFace();

            int weight = (dist * (64 - absval(ang))) / 100;

            if (weight < 1)
                continue;

            if (ang > 0)
                nudge -= weight;
            else
                nudge += weight;
        }

    return angle;
}

PTObjectInstance TCharacter::FindObjAhead()
{
    S3DPoint pos, v;
    GetPos(pos);
    ConvertToVector(GetFace(), 60, v);
    pos += v;

    int list[MAXFOUNDOBJS];
    int n = MapPane.FindObjectsInRange(pos, list, 60);

    PTObjectInstance best = NULL;
    int bestdist;

    for (int i = 0; i < n; i++)
    {
        PTObjectInstance oi = MapPane.GetInstance(list[i]);
        if (!oi)
            continue;

        if (oi->CursorType() != CURSOR_NONE || oi->IsInventoryItem())
        {
            int dist = Distance(oi);

            if (best == NULL || dist <= bestdist)
            {
                best = oi;
                bestdist = dist;
            }
        }
    }

    return best;
}

// ***********************
// * General AI Routines *
// ***********************

int TCharacter::ResolveMove(PTActionBlock ab, int bits)
{
    if (bits & MOVE_BLOCKED)
    {
        SetDesired(NULL);
        return COM_COMPLETED;
    }

    BOOL advanceang = TRUE;

    if (ab->target.x > 0 || ab->target.y > 0 || ab->target.z > 0)
    {
        BOOL exact = FALSE;
        int endfacing = -1;

#if 0
        if (ab->data)
        {
            exact = ((int *)ab->data)[0];
            endfacing = ((int *)ab->data)[1];
        }
#endif

        if (exact)
        {
            int sqrdist = dist(pos.x, pos.y, ab->target.x, ab->target.y);
            
            if (sqrdist < 16)
            {
                ab->terminating = TRUE;

                // start adjusting the angle
                if (endfacing >= 0)
                {
                    advanceang = FALSE;
                    int savefacing = facing;
                    AdvanceAngles(endfacing, endfacing, 128);
                    facing = savefacing;
                }

                // make sure he hits the point exactly
                S3DPoint newpos = pos;
                newpos += nextmove;
                if (sqrdist < dist(newpos.x, newpos.y, ab->target.x, ab->target.y))
                    forcenomove = TRUE;

                return COM_COMPLETED;
            }
            else
            {
                ab->angle = ConvertToFacing(pos, ab->target);
            }

        }
        else
        {
            if (dist(pos.x, pos.y, ab->target.x, ab->target.y) < 8)
            {
                ab->target.z = pos.z;
                SetPos(ab->target);
                ab->terminating = TRUE;
                return COM_COMPLETED;
            }
            else
            {
                ab->angle = ConvertToFacing(pos, ab->target);
            }
        }
        if (ab->terminating)
        {
            return COM_COMPLETED;
        }
    }

    if (!GridSnap)
    {
        if (advanceang)
            AdvanceAngles(ab->angle, ab->angle, 128);
    }
    else
    {
        if (commanddone)
            Face(ab->angle);
    }
    
    if (doing && desired && !ab->stop) // Take another step unless we were stopped
    {
      // Single forward animation
        if (doing->Is(StName(root->name, "f")) && !desired->Is(StName(root->name, "f")))
        {
            SetDesired(doing); // Just keep looping it!
        }
      // Left step
        else if (doing->IsLeft(root->name) && !desired->IsRight(root->name))
        {
            PTActionBlock newab = new TActionBlock(*doing);
            newab->stop = newab->interrupt = FALSE;
            strcpy(newab->name, StName(root->name, "r"));
            SetDesired(newab);
        }
      // Right step   
        else if (doing->IsRight(root->name) && !desired->IsLeft(root->name))
        {
            PTActionBlock newab = new TActionBlock(*doing);
            newab->stop = newab->interrupt = FALSE;
            strcpy(newab->name, StName(root->name, "l"));
            SetDesired(newab);
        }
    }

    return COM_EXECUTING;
}

int TCharacter::ResolveAttack(PTActionBlock ab, int bits)
{
    PTCharacter targ = (PTCharacter)ab->obj;
    PSCharAttackData attack = ab->attack;

  // Make sure moving angle equals face (it doesn't during a combat move)
    SetMoveAngle(GetFace());

    int dist, angle;
    if (targ)
    {
        dist = Distance(targ);
        angle = FaceAngleTo(targ);
    }
    else
    {
        dist = 0;
        angle = 0;
    }

  // Do this
    if (ab->firsttime)
    {
      // Signal our opponent that he's getting wailed on
        if (targ)
            targ->SignalAttack(this, ab->obj);
    }

  // If we have target, and frame has reached trigger time for impact, then do the impact.
    if (attack && frame == attack->impacttime)
    {
        int damage = 0;
        PSCharAttackImpact impact = NULL;

      // Check to see if impact is even possible?   
        if (targ &&
            dist >= attack->hitminrange &&
            dist <= attack->hitmaxrange && 
            abs(angle) <= attack->hitangle)
        {

            S3DPoint vect;
            ConvertToVector(facing, 4 * ROLLOVER, vect);
            ((PTCharacter)ab->obj)->vel += vect;

          // Chance to hit is concatenation of all the below
          // Note: To hit value is based on the following formula
          // 
          //    Armor + DefenseModifier + FatigueModifier(always negative) - AttackModifier
          // 
          //    This TOHIT value should 'almost' always be less than 25.  If it is greater,
          //    the random value will be 1-(tohit + 1) to always give the char a chance to hit.  
          // 
            int targangle = targ->FaceAngleTo(this);
            int tohit = targ->ArmorValue() + targ->DefenseModifier() + 
                targ->FatigueModifier() - AttackModifier();
            int maxroll = max(25, tohit + 1);           
            if (abs(targangle) < 48  &&                 // HALVE hit chances if blocking/dodging
                (targ->IsDoing(ACTION_BLOCK) || targ->IsDoing(ACTION_DODGE)))
                tohit += (maxroll - tohit + 1) / 2;
            if (abs(targ->FaceAngleTo(this)) >= 48 ||   // If not facing us or...
                targ->IsDoing(ACTION_ATTACK))           // is currently trying to do an attack
                tohit -= (maxroll - tohit + 1) / 2;     // Then DOUBLE hit chances

          // We hit, so set damage value to precalculated attack damage!
            damage = ab->damage; 
            impact = ab->impact;
        }

      // We hit and did damage!
        if (damage > 0)         // ****** CODE FOR HIT *******
        {
          // Do snap if needed
            if (impact && impact->snapdist > 0)
                targ->SnapDist(this, impact->snapdist);

          // Create a special DEATH action to use for Damage() function
            PTActionBlock deathab, impactab;
            deathab = impactab = NULL;
            if (targ->Health() - damage < 1)
            {
                char *deathname = "dead";
                if (impact && targ->HasActionAni(impact->impactname))
                    deathname = impact->impactname;
                deathab = new TActionBlock(deathname, ACTION_DEAD);
                deathab->obj = this;
                deathab->priority = TRUE;   // Don't interrupt period!
                deathab->attack = attack;
                deathab->impact = impact;
                deathab->damage = damage;
                targ->SetFighting(this);
            }

          // Create a special impact command to use for Damage() function
            else
            {
              // If we're not fighting anyone, fight the guy who just hit us
                if (!targ->Fighting())
                {
                    // not fighting - shall we engage this target?
                    if (dist <= targ->chardata->combatrange)
                        targ->BeginCombat(this);
                }

              // Get impact animations
                char *impactname = "impact";
                if (impact && targ->HasActionAni(impact->impactname))
                {
                    impactname = impact->impactname;
                    targ->SetFighting(this); // If special impact, face char to impact
                }
                ACTION a;
                if (!impact)
                    a = ACTION_IMPACT;
                else if (impact->flags & CAI_STUN)
                    a = ACTION_STUN;
                else if (impact->flags & CAI_KNOCKDOWN)
                    a = ACTION_KNOCKDOWN;
                else
                    a = ACTION_IMPACT;
                impactab = new TActionBlock(impactname, a);
                impactab->obj = this;
                impactab->interrupt = TRUE; // Interrupt current char 'doing'
                impactab->priority = TRUE;  // Don't imterrupt until done!
                impactab->attack = attack;
                impactab->impact = impact;
                impactab->damage = damage;
                if (impact)
                    impactab->wait = impact->looptime;
            }

//        Don't use this anymore... see below...
//          targ->Damage(damage, 
//                  GetDamageType(WeaponType(), attack->flags),  
//                  attack->damagemod,
//                  impactab, deathab);

          // Note: DamageType and damagemod's are already reflected in 'damage' total so we can get
          // the right impact/death for this attack.  This means we have to use DT_NONE to avoid
          // recalculating damage with damagetype modifiers, etc.
            targ->Damage(damage, DT_NONE, 0, impactab, deathab);
        }
        else                            // ****** CODE FOR MISS ******
        {
            PTActionBlock missab;

          // Do we play miss animation, or return straightway to combat state? 
            if (!(attack->flags & CA_NOMISS))
            {
                if (HasActionAni(attack->missname))
                {
                    missab = new TActionBlock(attack->missname, ACTION_MISS);
                    missab->attack = attack;
                }
                else
                {
                    missab = new TActionBlock("combat", ACTION_COMBAT);
                    PLAY(listrnd(chardata->misssounds));  // Play default sounds
                }
                missab->interrupt = TRUE;
                missab->obj = ab->obj;
                SetDesired(missab);
            }
            
          // Play block sound do sparks
            if (targ && targ->doing->action == ACTION_BLOCK)
            {
                if (attack->flags && CA_SPARKS)
                    EffectBurst("sparks");
                PLAY(listrnd(chardata->blocksounds));
            }
        }
    }

    return 0;
}

int TCharacter::ResolveImpact(PTActionBlock ab, int bits)
{
  // Make sure moving angle equals face (it doesn't during a combat move)
    SetMoveAngle(GetFace());

#if 0
    static int frame;
    if (ab->firsttime)
        frame = 0;

    if (frame < 4)
    {
        int x, y;
        S3DPoint fpos = pos;
        fpos.z += 75;
        S3DPoint vect;
        ConvertToVector(facing, 14, vect);
        fpos += vect;
        WorldToScreen(fpos, x, y);
        PlayScreen.AddPostCharAnim(x, y, 0, GameData->Animation("flashred")->GetFrame(frame++), DM_ALPHA);
    }
#endif

  // Do blood for impale    
    if ((ab->Is("impale") && random(0, 5) == 1))
        EffectBurst("blood", ab->Is("impale") ? 40 : 50);
    
  // Do blood for attack
    if (ab->firsttime && (!ab->attack || (ab->attack->flags & CA_BLOOD)))
        EffectBurst("blood", ab->Is("impale") ? 40 : 50);

  // Return from looping or root impact states.
  // This allows some impacts to be used as death states.  For example, the impact state is
  // a looping root state of the guy on the ground, and the impact has a "impact to combat"
  // state where the guy gets up again and goes back to fight pose.  The impact state will
  // use the get up transition, where the death state will just loop forever in the 'on the ground'
  // animation.
  // It also allows some impacts to have looping "stun" states where the attack sets the
  // wait value of the action block to 'stunwait', and the impact loop plays until 'wait'
  // is exauhsted.
    if (commanddone)
    {
      // If we're done waiting, go back to combat mode
        if (ab->wait <= 0)
        {
            doing->priority = FALSE;
            root->interrupt = TRUE;
            SetDesired(root);
        }
      // If we're done with impact animation, see if we should do the loop animation
      // NOTE: impacts can have a "combat to impact" transition state and "impact_l" looping state without
      // specifying anything for 'loopname'.  'loopname' is provided so that different impacts can end in
      // the same looping stun, knockdown, or death state.
        else if (ab->impact && 
            ab->Is(ab->impact->impactname) && 
            ab->impact->loopname[0] != NULL &&
            FindState(ab->impact->loopname) >= 0)
        {
            PTActionBlock newab = new TActionBlock(*ab, ab->impact->loopname, ab->action);
            newab->priority = TRUE;
            newab->interrupt = TRUE;
            desired->priority = FALSE;
            doing->priority = FALSE;
            SetDesired(newab);
        }
    }

    return 0;
}

int TCharacter::ResolveBlock(PTActionBlock ab, int bits)
{
    if (ab->wait <= 0 || (doing && doing->stop))
    {
        ab->wait = 0;
        SetDesired(NULL);
        return COM_COMPLETED;
    }

    return COM_EXECUTING;
}

int TCharacter::ResolveDead(PTActionBlock ab, int bits)
{
  // Make sure moving angle equals face (it doesn't during a combat move)
    SetMoveAngle(GetFace());

    return COM_EXECUTING;
}


void TCharacter::EffectBurst(char *name, int height)
{
    extern TObjectClass EffectClass;

    SObjectDef def;
    memset(&def, 0, sizeof(SObjectDef));

    def.objclass = OBJCLASS_EFFECT;
    def.objtype = EffectClass.FindObjType(name);

    def.pos = pos;
    def.level = MapPane.GetMapLevel();

    int index = MapPane.NewObject(&def);
    PTObjectInstance inst = MapPane.GetInstance(index);
    if (!inst)
        return;

    inst->CreateAnimator();
    PTParticle3DAnimator anim = (PTParticle3DAnimator)inst->GetAnimator();

    int ang = (GetFace() + random(-80, 80)) & 0xff;

    S3DPoint vect;
    ConvertToVector(ang, 100, vect);

    if (anim)
    {
        SParticleParams pr;

        if (stricmp(name, "blood") == 0)
        {
            pr.particles = random(5, 10);
            pr.pos.x = (D3DVALUE)0.0;
            pr.pos.y = (D3DVALUE)0.0;
            pr.pos.z = (D3DVALUE)height;
            pr.pspread.x = (D3DVALUE)3.0;
            pr.pspread.y = (D3DVALUE)3.0;
            pr.pspread.z = (D3DVALUE)3.0;
            pr.dir.x = (D3DVALUE)((float)vect.x / (float)100.0);
            pr.dir.y = (D3DVALUE)((float)vect.y / (float)100.0);
            pr.dir.z = (D3DVALUE)((float)vect.z / (float)100.0);
            pr.spread.x = (D3DVALUE)0.5;
            pr.spread.y = (D3DVALUE)0.5;
            pr.spread.z = (D3DVALUE)0.5;
            pr.gravity = (D3DVALUE)0.3;
            pr.trails = 3;
            pr.minstart = 0;
            pr.maxstart = 8;
            pr.minlife = 10;
            pr.maxlife = 30;
            pr.bounce = FALSE;
            pr.killobj = TRUE; 
            pr.objflags = 0xF;
            pr.seektargets = FALSE;
            pr.numtargets = 0;
        }
        else
        {
            S3DPoint vect0, tpos;
            doing->obj->GetPos(tpos);
            ang = ConvertToFacing(pos, tpos);
            int dist = TObjectInstance::Distance(doing->obj);
            ConvertToVector(ang, dist / 2, vect0);
            vect0.z += 45;

            pr.particles = random(15, 25);
            pr.pos.x = (D3DVALUE)vect0.x;
            pr.pos.y = (D3DVALUE)vect0.y;
            pr.pos.z = (D3DVALUE)vect0.z;
            pr.pspread.x = (D3DVALUE)3.0;
            pr.pspread.y = (D3DVALUE)3.0;
            pr.pspread.z = (D3DVALUE)3.0;
            pr.dir.x = (D3DVALUE)((float)vect.x / (float)100.0);
            pr.dir.y = (D3DVALUE)((float)vect.y / (float)100.0);
            pr.dir.z = (D3DVALUE)((float)vect.z / (float)100.0);
            pr.spread.x = (D3DVALUE)0.5;
            pr.spread.y = (D3DVALUE)0.5;
            pr.spread.z = (D3DVALUE)0.5;
            pr.gravity = (D3DVALUE)0.2;
            pr.trails = 1;
            pr.minstart = 0;
            pr.maxstart = 8;
            pr.minlife = 20;
            pr.maxlife = 40;
            pr.bounce = FALSE;
            pr.killobj = TRUE; 
            pr.objflags = 1 << (ObjId() & 0x3);
            pr.seektargets = FALSE;
            pr.numtargets = 0;
        }

        anim->InitParticles(&pr);
    }
}

int TCharacter::ResolveCombat(PTActionBlock ab, int bits)
{
    // line them up with whoever they are fighting
    if (ab->obj)
    {
      // If target is dead, change to new target, or end combat
        PTCharacter targ = (PTCharacter)ab->obj;
        if (targ->IsDead())
        {
            targ = FindClosestEnemy();
            if (!targ)
            {
                EndCombat();
                return 0;
            }
            else
            {
                SetFighting(targ);
            }
        }

      // If walking, set proper walk animation for current monster direction
        S3DPoint target;
        ab->obj->GetPos(target);
        int angle = ConvertToFacing(pos, target);

        if (ab->action == ACTION_COMBATMOVE && !ab->stop) // If angle to target has changed, get new walk anim
        {
            int roundangle = ((angle + 15) & 0xE0); // round to 8 dirs
            int diff = ab->moveangle - roundangle;
            int anim = (diff & 255) / 32;

            char *animname;
            switch (anim)
            {
                case 0: animname = "combatf"; break;
                case 1: animname = "combatfr"; break;
                case 2: animname = "combatr"; break;
                case 3: animname = "combatbr"; break;
                case 4: animname = "combatb"; break;
                case 5: animname = "combatbl"; break;
                case 6: animname = "combatl"; break;
                case 7: animname = "combatfl"; break;
            }
            
            if (!ab->Is(animname))  // Not already going this way
            {
                PTActionBlock newab = new TActionBlock(animname, ACTION_COMBATMOVE);
                newab->angle = angle;
                newab->moveangle = ab->moveangle;
                newab->obj = ab->obj;
                SetDesired(newab);
            }
        }

      // Change facing angle here, but don't change movement angle
        FaceOnly(angle);
        SetMoveAngle(ab->moveangle);
    }

    return 0;
}

int TCharacter::ResolveCombatMove(PTActionBlock ab, int bits)
{
    if (bits & MOVE_BLOCKED || ab->stop)
    {
        ForceCommand(root);
        return COM_COMPLETED;
    }

    if (!IsDesired(ACTION_COMBATMOVE) && !ab->stop) // Set next step if not stopped
        SetDesired(doing);  

    return ResolveCombat(ab, bits);
}

int TCharacter::ResolveLeap(PTActionBlock ab, int bits)
{
    return ResolveCombat(ab, bits);
}

int TCharacter::ResolvePull(PTActionBlock ab, int bits)
{
    int start;

    if (!ab->data)
        return 0;

    start = ((int *)ab->data)[0];

    if ((start == LEVER_PULLSOUTH) || (start == LEVER_PULLNORTH))
    {
        if (commanddone && ab->obj)
        {
            if (start == LEVER_PULLNORTH)
            {
                if (ab->obj->GetState() == EXIT_OPEN)
                    ab->obj->SetState( EXIT_CLOSING);
            }
            else
            {
                if (ab->obj->GetState() == EXIT_CLOSED)
                    ab->obj->SetState( EXIT_OPENING);
            }
        }
    }

    if ((start == LEVER_PUSHSOUTH) || (start == LEVER_PUSHNORTH))
    {
        if (ab->firsttime && ab->obj)
        {
            if (start == LEVER_PUSHNORTH)
            {
                if (ab->obj->GetState() == EXIT_OPEN)
                    ab->obj->SetState( EXIT_CLOSING);
            }
            else
            {
                if (ab->obj->GetState() == EXIT_CLOSED)
                    ab->obj->SetState( EXIT_OPENING);
            }
        }
    }

    return 0;
}

int TCharacter::ResolveSay(PTActionBlock ab, int bits)
{
    if (ab->wait <= 0 || (doing && doing->stop))
    {
        ab->wait = 0;
        SetDesired(NULL);
        return COM_COMPLETED;
    }

    return COM_EXECUTING;
}

int TCharacter::ResolvePivot(PTActionBlock ab, int bits)
{
    if (commanddone)
    {
        int pos = strlen(root->name) + 5;  // i.e. "WALKPIVOTxx"
        // Checks last characters of "xxxxPIVOTxx" to get turn values
        if (!stricmp(ab->name + pos, "fl"))
            Face((facing + 32) & 255);
        else if (!stricmp(ab->name + pos, "fr"))
            Face((facing - 32) & 255);
        else if (!stricmp(ab->name + pos, "l"))
            Face((facing + 64) & 255);
        else if (!stricmp(ab->name + pos, "r"))
            Face((facing - 64) & 255);
        else if (!stricmp(ab->name + pos, "bl"))
            Face((facing + 96) & 255);
        else if (!stricmp(ab->name + pos, "br"))
            Face((facing - 96) & 255);
        else if (!stricmp(ab->name + pos, "al"))
            Face((facing + 128) & 255);
        else if (!stricmp(ab->name + pos, "ar"))
            Face((facing - 128) & 255);
             
        if (animator)       // Since hips are rotated, interpolation will cause it to rotate even more
            animator->ClearPrevState(); // So clear prev ani so it won't interpolate

        SetDesired(NULL); // Return to root state
    }

    return COM_EXECUTING;
}

int TalismanStat(int tal, char *statname)
{
    extern TObjectClass TalismanClass;

    return TalismanClass.GetStat(tal, statname);
}

int TCharacter::ResolveInvoke(PTActionBlock ab, int bits)
{
    if (ab->firsttime)
    {
        /* stuff to handle failed spellcasting should be here - a fizzle
            results when you suck due to mind stat and invocation ability,
            as compared to the power level of the spell. */

        // now calculate the values for the spell and replace the string with the value block
        PTSpellBlock spell = new TSpellBlock(this, Fighting());

        int *ptr = (int *)ab->data;
        spell->AddPower(*ptr);

        delete ab->data;        // don't need the spell values anymore

        ab->data = spell;       // point to the spell block
    }

    else //if (!ab->transition)     // don't begin execution until they have fully entered the state
    {
        PTSpellBlock spell = (PTSpellBlock)ab->data;

        /*
        BOOL kill = FALSE;
        if (ab->wait >= 3)
            kill = TRUE;

        if (commanddone && !kill)
            ab->wait++;
        
        if (((PTSpellBlock)ab->data)->Pulse(kill) && commanddone)
            return COM_COMPLETED;
        */
        BOOL kill = FALSE;
        if(commanddone)
            kill = TRUE;
        int result;
        result = ((PTSpellBlock)ab->data)->Pulse(kill);
        if(kill)
            return COM_COMPLETED;
    }

    return COM_EXECUTING;
}

BOOL TCharacter::IsFighting()
{
    if (IsRoot(ACTION_COMBAT))
        return TRUE;

    return FALSE;
}

BOOL TCharacter::IsFinalState()
{
    if (doing && (doing->Is("collapse") || doing->Is("dead") || doing->Is("impale") || doing->Is("fall")))
        return TRUE;

    return FALSE;
}

BOOL TCharacter::IsEnemy(PTCharacter chr)
{
    if (listin(chardata->enemies, chr->GetName()) ||
        listin(chardata->enemies, chr->GetTypeName()) )
            return chr->Aggressive();

    int numgroups = listnum(chr->chardata->groups);

    for (int c = 0; c < numgroups; c++)
    {
        if (listin(chardata->enemies, listget(chr->chardata->groups, c)))
            return chr->Aggressive() || chr->ObjClass() == OBJCLASS_PLAYER;
    }

    return FALSE;
}

// ***********************
// * General AI Routines *
// ***********************

// Change the two values below to determine when characters start fighting.
// The first number is the range to start a fight.  The second number is the
// buffer after which the fight stops - increase it if there is problems with
// characters bouncing in and out of combat mode when other characters are right
// on the edge of HOSTILITY_RANGE.
#define HOSTILITY_RANGE             (chardata->combatrange)
#define HOSTILITY_EDGE              16
#define HOSTILITY_TOTAL             (chardata->combatrange + HOSTILITY_EDGE)

void TCharacter::AI()
{
    if (flags & OF_DISABLED || Editor)
        return;

    PTCharacter target = Fighting();
    
  // We don't have a target.. try to find one
    if (!target && !Editor && Aggressive())
    {
        target = FindClosestEnemy(); // Finds the closest visible enemy (if it can see it)
        if (target && Distance((PTCharacter)target) < HOSTILITY_RANGE)
            BeginCombat(target);
    }

    if (target && doing)
    {
        if (doing->action == ACTION_COMBAT)
        {
            if (nextattack > 0)
                nextattack--;

            if (nextattack == 0)
            {
                if (random(1,100) <= chardata->blockfreq)
                    Block();
                else
                    RandomAttack(random(1,100));
                nextattack--;
            }
            else

            if (nextattack <= 0)
            {
                nextattack = 
                    random(chardata->minattackfreq * FRAMERATE / 100, 
                           chardata->maxattackfreq * FRAMERATE / 100);
            }
        }
        else if (doing->action == ACTION_COMBATMOVE)
        {
            Stop();
        }
    }
}

BOOL TCharacter::CanHearCharacter(PTCharacter chr)
{
    BOOL hear = TRUE;
    int dist = Distance(chr);
    int noise = chr->LastNoise();
    S3DPoint from, to;
    GetPos(from);
    from.z += LIGHTINGCHARHEIGHT; // Nominal character height
    chr->GetPos(to);
    to.z += LIGHTINGCHARHEIGHT; // Nominal character height
    if (dist > chardata->hearingrange ||    // Within hearing range
         noise < (100 - Hearing(dist)) ||   // Last noise made was too quiet
        !MapPane.LineOfSight(from, to))     // Has line of sight
        hear = FALSE;

    return hear;
}

BOOL TCharacter::CanSeeCharacter(PTCharacter chr, int angle)
{
    BOOL see = TRUE;

    if (angle < 1)
        angle = GetFace();
    int dist = Distance(chr);
    int angleto = AngleTo(chr);
    int anglediff = abs(AngleDiff(angle, angleto));

    int glimpse = chr->LastGlimpse();
    if (chardata->flags & CF_INFRAVISION)
        glimpse = 10000; // Always sees
    else if (chardata->flags & CF_LIGHTBLIND)
        glimpse = 100 - glimpse; // Reverse glimpse value so more light is less visible!

    S3DPoint from, to;
    GetPos(from);
    from.z += LIGHTINGCHARHEIGHT; // Nominal character height
    chr->GetPos(to);
    to.z += LIGHTINGCHARHEIGHT; // Nominal character height

    if (dist > chardata->sightrange ||
        anglediff > chardata->sightangle ||
        !MapPane.LineOfSight(from, to) ||
        glimpse < (100 - Sight(dist)))
        see = FALSE;

    return see;
}

PTCharacter TCharacter::FindCharacter(int range, int angle, int anglerange, int flags)
{
    PTCharacter best = NULL;
    int bestdist;
    
    if (range < 0 || (flags & FINDCHAR_HEAR))
        range = max(range, chardata->hearingrange);
    if (range < 0 || (flags & FINDCHAR_SEE))
        range = max(range, chardata->sightrange);

    for (TMapIterator i(Pos(), range, CHECK_NOINVENT | CHECK_MAPRECT, OBJSET_CHARACTER); i; i++)
    {
        PTCharacter chr = (PTCharacter)i.Item();

        if (chr == this)
            continue;

        if ((flags & FINDCHAR_ENEMY) && !IsEnemy(chr))
            continue;

        int dist = Distance(chr);
        int angleto = AngleTo(chr);

     // Do we hear this guy?
        BOOL hear = TRUE;
        if (flags & FINDCHAR_HEAR)
            hear = CanHearCharacter(chr);

      // Do we see this guy
        BOOL see = TRUE;
        if (flags & FINDCHAR_SEE)
            see = CanSeeCharacter(chr, angle); // Uses 'angle' if >= 0, otherwise uses facing
    
      // If we can't hear him or see him, he doesn't exist  
        if (!hear && !see)
            continue;

      // Is this guy in the direction we're checking?   
        if (angle >= 0)
        {
            int diff = abs(AngleDiff(angle, angleto));
            if (diff > anglerange)
                continue;
            dist = dist * ((anglerange + 1) - diff); // Dist gets bigger when diff between angles small
        }
    
      // Is this the closest guy        
        if (best == NULL || dist <= bestdist)
        {
            best = chr;
            bestdist = dist;
        }
    }

    return best;
}

PTCharacter TCharacter::FindCharacterAhead(int angle, int anglerange)
{
    return FindCharacter(512, angle, anglerange, 0);
}

PTCharacter TCharacter::FindClosestEnemy(int angle, int anglerange)
{
    return FindCharacter(-1, angle, anglerange, FINDCHAR_ENEMY | FINDCHAR_SEE | FINDCHAR_HEAR);
}

// Returns a 1-100 hearing value which indicates how the average noise will be heard
// by a monster.  If the monster is sleeping, the listening value is 20% of normal. 
// The hearing value is based on the minhearing/maxhearing values in the chardata structure,
// where minhearing is the hearing value at the characters maximum hearing range, and 
// maxhearing is the hearing value right in front of the character.
int TCharacter::Hearing(int dist)
{
    dist = max(0, dist - (Radius() + 32));

    if (dist > chardata->hearingrange)
        return 0;

    int hearing = chardata->hearingmin +
        (chardata->hearingrange - dist) *
        (chardata->hearingmax - chardata->hearingmin) / 
        chardata->hearingrange;

    if (Sleeping())
        hearing = hearing * 20 / 100;

    return hearing;
}

// Returns a 1-100 sight value which indicates how the average char will be seen
// by a monster in the darkness.  The sight value is based on the minsight/maxsight
// values in the chardata structure, where minsight is the sight value at the characters
// maximum sight range, and maxsight is the sight value right in front of the character.
// If the monster is sleeping, the sight value is always 0.
int TCharacter::Sight(int dist)
{
    dist = max(0, dist - (Radius() + 32));

    if (Sleeping() || dist > chardata->sightrange)
        return 0;
    
  // Basically return min + (max - min) * dist/range
    return chardata->sightmin +
        (chardata->sightrange - dist) *
        (chardata->sightmax - chardata->sightmin) / 
        chardata->sightrange;
}

// Resets the noise and glimpse values to control whether monsters see you or not
void TCharacter::ResetStealthValues()
{
  // Figure out stealth!
    int stealthmod;
    if (IsSneakMode())
        stealthmod = Rules.sneakstealth;
    else
        stealthmod = Rules.maxstealth;
    stealthmod = stealthmod * (100 - StealthMod()) / 100;
    if (stealthmod < Rules.minstealth)
        stealthmod = Rules.minstealth;  // Always aleast ten percent

    int r = random(1, 100);

  // Get noise character made!
    noise = r * stealthmod / 100;

  // Get glimpse character made! 
    int visibility = Visibility();
    glimpse = (visibility + (r * visibility / 100)) * stealthmod / 100;
}

void TCharacter::SignalMovement(PTObjectInstance actor)
{
}

void TCharacter::SignalHostility(PTObjectInstance actor, PTObjectInstance target)
{
    if (target != this)
        return;
}

void TCharacter::SignalAttack(PTObjectInstance actor, PTObjectInstance target)
{
    if (target != this)
        return;

    if (actor == doing->obj && 
        random(1, 100) <= BlockPcnt())
          Block();
}

void TCharacter::SetOnExit()
{
    SetFlag(OF_ONEXIT, TRUE);
    exittimestamp = CurrentScreen->FrameCount();
}

// ***********************************************************************************
// * Access functions - called by script or player to make the character do whatever *
// ***********************************************************************************

BOOL TCharacter::Go(int angle)
{
    PTActionBlock ab = NULL;

    if (!IsFighting())
    {
        if (doing == root)          // If root (standing)..
        {
            Face(angle);            // Face new direction immediately instead of turning
            root->angle = angle;
        }

        if (IsDesired(ACTION_MOVE))
        {
            ab = desired;
            ab->stop = FALSE;       // Make sure desired stop flag is false too
        }
        else
        {
            char *name = "walk";
            if (root)
                name = root->name;

            if (HasActionAni(StName(name, "f")))
                ab = new TActionBlock(StName(name, "f"), ACTION_MOVE);
            else if (FindTransitionState(name, StName(name, "l")) >= 0)
                ab = new TActionBlock(StName(name, "l"), ACTION_MOVE);
            else
                ab = new TActionBlock(StName(name, "r"), ACTION_MOVE);
        }
        
        if (IsDoing(ACTION_MOVE))
        {
            doing->stop = FALSE;    // Keep going if we we're recently stopped
            if (doing->angle != angle)
                doing->angle = angle;
        }

        ab->angle = angle;
    }
    else    // Do fighting walking
    {
      // Do we start fighting a new character?
        PTCharacter newtarg = FindClosestEnemy(angle, 32);
        if (newtarg && newtarg != (PTCharacter)doing->obj)
            SetFighting(newtarg);

        if (!doing || !doing->obj) // If we're not facing an enemy, set facing to move angle
            Face(angle);

        if (doing->action != ACTION_COMBAT && doing->action != ACTION_COMBATMOVE)
            return FALSE;

        int roundangle = ((GetFace() + 15) & 0xE0); // round to 8 dirs
        int diff = (angle - roundangle) & 255;
        int anim = diff / 32;

        switch (anim)
        {
            case 0: ab = new TActionBlock("combatf", ACTION_COMBATMOVE); break;
            case 1: ab = new TActionBlock("combatfr", ACTION_COMBATMOVE); break;
            case 2: ab = new TActionBlock("combatr", ACTION_COMBATMOVE); break;
            case 3: ab = new TActionBlock("combatbr", ACTION_COMBATMOVE); break;
            case 4: ab = new TActionBlock("combatb", ACTION_COMBATMOVE); break;
            case 5: ab = new TActionBlock("combatbl", ACTION_COMBATMOVE); break;
            case 6: ab = new TActionBlock("combatl", ACTION_COMBATMOVE); break;
            case 7: ab = new TActionBlock("combatfl", ACTION_COMBATMOVE); break;
        }

        if (ab && doing) // Copy combat target
            ab->obj = doing->obj;

        if (doing->obj)
            ab->angle = AngleTo(doing->obj);
        else
            ab->angle = angle;
        ab->moveangle = angle;
        SetMoveAngle(angle);
    
        if (doing->action == ACTION_COMBATMOVE) // Interrupt current step if dir changes
            ab->interrupt = TRUE;
    }

    if (!HasActionAni(ab->name)) // Can't do this
    {
        if (ab != doing && ab != desired && ab != root)
            delete ab;
        return FALSE;
    }

    SetDesired(ab);
    return TRUE;
}

BOOL TCharacter::Go(S3DPoint vect)
{
    S3DPoint zero;
    memset(&zero, 0, sizeof(S3DPoint));
    int angle = ConvertToFacing(zero, vect);
    return Go(angle);
}

BOOL TCharacter::Goto(int x, int y, BOOL exact, int endfacing)
{
    if (IsFighting())
        return FALSE;
    
    PTActionBlock ab;
    char *name = root->name;

    if (FindTransitionState(name, StName(name, "f")) >= 0)
        ab = new TActionBlock(StName(name, "f"), ACTION_MOVE);
    else if (FindTransitionState(name, StName(name, "l")) >= 0)
        ab = new TActionBlock(StName(name, "l"), ACTION_MOVE);
    else
        ab = new TActionBlock(StName(name, "r"), ACTION_MOVE);

    ab->target.x = x;
    ab->target.y = y;
    ab->target.z = pos.z;
    ab->angle = ConvertToFacing(pos, ab->target);
    ab->terminating = FALSE;
#if 0
    ab->data = malloc(sizeof(int) * 2);
    ((int *)ab->data)[0] = exact;
    ((int *)ab->data)[1] = endfacing;
#endif

    if (!HasActionAni(ab->name)) // Can't do this
    {
        if (ab != doing && ab != desired && ab != root)
            delete ab;
        return FALSE;
    }

    SetDesired(ab);

    return TRUE;
}

BOOL TCharacter::Stop(char *name)
{
    if (!IsMoving() &&
        (!name || !doing->Is(name)))                                   // Is a use specified command
        return FALSE;

//  if (doing)
//      doing->stop = TRUE;

    root->interrupt = TRUE;
    SetDesired(root);

    return TRUE;
}

BOOL TCharacter::Disable()
{
    SetFlags(OF_DISABLED);
    if (doing != root)
        ForceCommand(root);
    return TRUE;
}

// Causes character to jump (in normal mode, use Leap in Combat mode)
BOOL TCharacter::Jump()
{
    return FALSE;
}

// Sets walk mode
BOOL TCharacter::SetWalkMode()
{
    if (IsWalkMode())
        return TRUE;

    PTActionBlock ab = new TActionBlock("walk");

    if (doing && doing->action == ACTION_MOVE)      // If moving, change next step to new root
        desired->SwapRoot(root->name, ab->name);
    else if (doing && doing->action == ACTION_ANIMATE)  // If standing, set desired to new root
        SetDesired(ab);

  // Whatever we're doing now, we'll go back to this state when we're done
    SetRoot(ab);

    return TRUE;
}

// Sets sneak mode
BOOL TCharacter::SetSneakMode()
{
    if (IsSneakMode())
        return TRUE;

    PTActionBlock ab = new TActionBlock("sneak");

    if (doing && doing->action == ACTION_MOVE)      // If moving, change next step to new root
        desired->SwapRoot(root->name, ab->name);
    else if (doing && doing->action == ACTION_ANIMATE)  // If standing, set desired to new root
        SetDesired(ab);

  // Whatever we're doing now, we'll go back to this state when we're done
    SetRoot(ab);

    return TRUE;
}

// Sets run mode
BOOL TCharacter::SetRunMode()
{
    if (IsRunMode())
        return TRUE;

    PTActionBlock ab = new TActionBlock("run");

    if (doing && doing->action == ACTION_MOVE)      // If moving, change next step to new root
        desired->SwapRoot(root->name, ab->name);
    else if (doing && doing->action == ACTION_ANIMATE)  // If standing, set desired to new root
        SetDesired(ab);

  // Whatever we're doing now, we'll go back to this state when we're done
    SetRoot(ab);

    return TRUE;
}

// Attempt to play a pivot animation towards the given delta angle
BOOL TCharacter::Pivot(int angle)
{
    if (angle > 0)
        angle = (angle + 15) & 0xE0;
    else
        angle = -((-angle + 15) & 0xE0);

    if (angle == 0)
        return TRUE;

    char buf[30];

    strcpy(buf, root->name);
    strcat(buf, "pivot");
    if (angle == 32)
        strcat(buf, "fl");
    else if (angle == -32)
        strcat(buf, "fr");
    else if (angle == 64)
        strcat(buf, "l");
    else if (angle == -64)
        strcat(buf, "r");
    else if (angle == 96)
        strcat(buf, "bl");
    else if (angle == -96)
        strcat(buf, "br");
    else if (angle == 128)
        strcat(buf, "al");
    else if (angle == -128)
        strcat(buf, "ar");

    if (FindTransitionState(root->name, buf) < 0)
    {
        Face(facing + angle);
        return TRUE;
    }

    PTActionBlock ab = new TActionBlock(buf, ACTION_PIVOT);
    ab->angle = (facing + angle) & 0xFF;
    SetDesired(ab);

    return TRUE;
}

// This obviously does nothing right now
BOOL TCharacter::FollowChar(PTObjectInstance inst)
{
    return FALSE;
}

BOOL TCharacter::Pickup(PTObjectInstance inst)
{
    if (!inst || !inst->IsInventoryItem())
        return FALSE;

    inst->RemoveFromMap();
    if (!Inventory.GetContainer()->AddToInventory(inst))
        TextBar.Print("Can't carry any more.");

/*  
    if (!inst || !inst->IsInventoryItem())
        return;

    desired = new TActionBlock("pickup");
    desired->obj = inst;*/

    return TRUE;
}

BOOL TCharacter::Pull(PTObjectInstance inst)
{
    PTActionBlock ab = new TActionBlock("Pull Front");
    int state = inst->GetState();
    int direction = (((PTLever)inst)->targetpos.z / 64);

    ab->obj = inst;
    ab->priority = TRUE;    // Won't do it otherwise

    ab->data = malloc(sizeof(int));

    if ((direction == 3) && (state == EXIT_OPEN))
        state = LEVER_PUSHNORTH;
    else if ((direction == 3) && (state == EXIT_CLOSED))
        state = LEVER_PULLSOUTH;
    else if ((direction == 1) && (state == EXIT_OPEN))
        state = LEVER_PULLNORTH;
    else if ((direction == 1) && (state == EXIT_CLOSED))
        state = LEVER_PUSHSOUTH;

    ((int *)ab->data)[0] = state;

    SetDesired(ab);

    return TRUE;
}

// Attempts to use something in the direction character is facing
BOOL TCharacter::TryUse()
{
    S3DPoint pos, v;
    GetPos(pos);
    ConvertToVector(GetFace(), 60, v);
    pos += v;

    int list[MAXFOUNDOBJS];
    int n = MapPane.FindObjectsInRange(pos, list, 60);

    PTObjectInstance best = NULL;
    int bestdist;

    for (int i = 0; i < n; i++)
    {
        PTObjectInstance oi = MapPane.GetInstance(list[i]);
        if (!oi)
            continue;

        if (oi->ObjClass() == OBJCLASS_CONTAINER ||
            oi->ObjClass() == OBJCLASS_EXIT)
        {
            int dist = Distance(oi);

            if (best == NULL || dist <= bestdist)
            {
                best = oi;
                bestdist = dist;
            }
        }
    }

    if (!best)
    {
        TextBar.Print("Nothing to use");
        return FALSE;
    }
    
    TextBar.Print("Use %s", best->GetName());
    best->Use(this);

    return TRUE;
}

// Attempts to get something in the direction character is facing
BOOL TCharacter::TryGet()
{
    S3DPoint pos, v;
    GetPos(pos);
    ConvertToVector(GetFace(), 60, v);
    pos += v;

    int list[MAXFOUNDOBJS];
    int n = MapPane.FindObjectsInRange(pos, list, 60);

    PTObjectInstance best = NULL;
    int bestdist;

    for (int i = 0; i < n; i++)
    {
        PTObjectInstance oi = MapPane.GetInstance(list[i]);
        if (!oi)
            continue;

        if (oi->CursorType() != CURSOR_NONE || oi->IsInventoryItem())
        {
            int dist = Distance(oi);

            if (best == NULL || dist <= bestdist)
            {
                best = oi;
                bestdist = dist;
            }
        }
    }

    if (!best)
    {
        TextBar.Print("Nothing to get");
        return FALSE;
    }

    TextBar.Print("Get %s", best->GetName());
    Pickup(best);

    return TRUE;
}

BOOL TCharacter::Say(char *string, int wait, char *anim)
{
    char buf[128];
    DialogLine(string, buf, 128);  // Translate dialog line (convert [tags])

    PTActionBlock ab;
    if (anim)
        ab = new TActionBlock(anim, ACTION_SAY);
    else
        ab = new TActionBlock("say", ACTION_SAY);

    ab->data = (void *)strdup(buf);
    if (wait < 0)
        ab->wait = 15 + max(15, strlen(buf) * 5 / 4);  // Ratio of ticks to chars
    else
        ab->wait = wait;
    SetDesired(ab);

    return TRUE;
}

// Checks attack to see if it is valid or not
BOOL TCharacter::IsValidAttack(int attacknum, int &impactnum, int &damage,
    int tdist, int id, int pcnt, int dmgpcnt, int flagmask, int flags)
{
    impactnum = -1;
    damage = 0;

    if ((DWORD)attacknum >= (DWORD)chardata->attacks.NumItems())
        return FALSE;

    PSCharAttackData ad = &(chardata->attacks[attacknum]);
    PTCharacter targ = Fighting();

  // Matches flags
    if ((ad->flags & flagmask) != flags)
        return FALSE;

  // Button id matches attack button id (for controller/keyboard buttons)
    if (id >= 0 && ad->button != id)
        return FALSE;

  // Percentage value is less than percent parameter (for random attack finding)
    if (ad->attackpcnt < pcnt)
        return FALSE;
        
  // In range
    if (ad->maxdist > 0 && targ && (tdist < ad->mindist || tdist > ad->maxdist))
        return FALSE;;

   // Not still doing another attack
    if (doing->action == ACTION_ATTACK && frame < doing->attack->nextwait)
        return FALSE;

  // Has animation
    if (!HasActionAni(ad->attackname))
        return FALSE;

  // Not too tired
    if (Fatigue() < ad->fatigue)
        return FALSE;

  // Check if is a moving attack
    if ((ad->flags & CA_MOVING) && 
        (!(IsDoing(ACTION_COMBATMOVE) || IsDoing(ACTION_MOVE)) || 
         abs(AngleDiff(GetFace(), GetMoveAngle())) > 32) )
        return FALSE;

  // Check if chain attack is valid...
    if ((ad->flags & CA_CHAIN) && 
      (!lastattack || 
       stricmp(lastattack->attackname, ad->chainname) != 0 ||
       PlayScreen.GameFrame() - lastattackticks > ad->chainexptime))
        return FALSE;

  // Check for attack when opponent stunned or down
    if (targ && (ad->flags & CA_ATTACKSTUN) && !targ->IsDoing(ACTION_STUN))
        return FALSE;

  // Check for attack when opponent down
    if (targ && (ad->flags & CA_ATTACKDOWN) && !targ->IsDoing(ACTION_KNOCKDOWN))
        return FALSE;

  // If a response, make sure target character is in correct state to respond to
    if (targ && ad->responsename[0] != 0 && !targ->doing->Is(ad->responsename))
        return FALSE;

  // Calculate damage attack will do to target now
    if (targ)
    {
        damage = targ->CalculateDamage(WeaponDamage(), 
            GetDamageType(WeaponType(), ad->flags), ad->damagemod) * dmgpcnt / 100;
    }

  // If a death attack, make sure the guy will actually die...
    if (ad->flags & CA_DEATH)
    {
        if (!targ)
            return FALSE;
        if (damage < targ->Health())
            return FALSE;
    }
    
  // If impact, make sure character has appropriate impact/death/stun animation
    if (ad->numimpacts > 0 && targ)
    {
        int targdmgpcnt = damage * 100 / targ->MaxHealth();
        if (targdmgpcnt > 100)
            targdmgpcnt = 100;

        PSCharAttackImpact ai = ad->impacts;
        for (int i = 0; i < ad->numimpacts; i++, ai++)
        {
            if (((damage >= targ->Health()) && (ai->flags & CAI_DEATH)) ||      // Is death impact (overrides any damage impacts)
                (impactnum < 0 &&                                               // or death impact not already set and...
                 targdmgpcnt >= ai->damagemin && targdmgpcnt <= ai->damagemax)) // is normal/stun/knockdown impact
            {
                if (!targ->HasActionAni(ai->impactname)) // Don't have this impact ani, don't do this attack!
                    return FALSE;
                if (ai->loopname[0] != NULL && !targ->FindState(ai->loopname)) // Needs the loop too!
                    return FALSE;
                impactnum = i;  // Remember what impact we're using
            }
        }
    }

  // Check player skills, etc.  
    if (objclass == OBJCLASS_PLAYER)
    {
        PTPlayer player = (PTPlayer)this;
        
//    // Is using correct weapon for this attack
//      if (!(ad->weaponmask & (1 << WeaponType())))
//          return FALSE;

      // Player must have high enough attack skill
        if (player->Skill(SK_ATTACK) < ad->attackskill)
            return FALSE;

      // Player must have high enough weapon skill for the given weapon being used
        if (player->WeaponSkill(WeaponType()) < ad->weaponskill)
            return FALSE;
    }

    return TRUE;
}

// Finds a valid attack given an attack id (i.e. controller button)
BOOL TCharacter::FindButtonAttack(int id, int dmgpcnt, int &attacknum, int &impactnum, int &damage)
{
    if (!IsFighting())
        return FALSE;

    int flags;

    int tdist;
    if (Fighting())
        tdist = Distance(Fighting());
    else
        tdist = 10000;

  // Loop through array three times (1-combo attack, 2-special attack, 3-normal attack)
  // This allows combos, specials, and normals to use the same attack buttons on the joypad/
  // keyboard.  Specials and combo's will only happen when they are available and all
  // parameters qualify, otherwise this routine will cascade down to the normal attacks.
    for (int mode = 0; mode < 3; mode++)
    {

    // Look for combos first, then specials, then just normals
      if (mode == 0)
        flags = CA_RESPONSE;
      else if (mode == 1)
        flags = CA_SPECIAL;
      else
        flags = 0;

    // Loop through attack list to find a valid attack
      for (int attack = 0; attack < chardata->attacks.NumItems(); attack++)
      {
        if (IsValidAttack(attack, impactnum, damage, tdist, id, 0, dmgpcnt, CA_RESPONSE | CA_SPECIAL, flags))
        {
            attacknum = attack;
            return TRUE;
        }
      }
    }

    return FALSE;
}

// Finds a valid attack given a randomly generated percentage (0-100) number
BOOL TCharacter::FindPcntAttack(int pcnt, int dmgpcnt, int &attacknum, int &impactnum, int &damage)
{
    if (!IsFighting() || !Fighting())
        return FALSE;

    int tdist = Distance(Fighting());

  // Loop through attack list to find a valid attack
  // Loops through twice.. should be able to find one that works by then...
    for (int attack = 0; attack < chardata->attacks.NumItems() * 2; attack++)
    {
        int randattack = random(0, chardata->attacks.NumItems() - 1);
        if (IsValidAttack(randattack, impactnum, damage, tdist, -1, pcnt, dmgpcnt, 0, 0))
        {
            attacknum = randattack;
            return TRUE;
        }
    }

    return FALSE;
}

// Executes a particular attack (using index into SCharData's attack array)
BOOL TCharacter::DoAttack(int attacknum, int impactnum, int damage)
{
    if (!IsFighting() || (DWORD)attacknum >= (DWORD)chardata->attacks.NumItems())
        return FALSE;

  // Wait for previous attack?
    if (doing->action == ACTION_ATTACK && 
        frame < doing->attack->nextwait)
        return FALSE;
    
  // Get attack info from char data
    PSCharAttackData ad = &(chardata->attacks[attacknum]);

  // Get target
    PTCharacter targ = (PTCharacter)doing->obj;

  // Setup action block
    PTActionBlock ab = new TActionBlock(ad->attackname, ACTION_ATTACK);
    ab->obj = targ;
    ab->attack = ad;
    if (impactnum >= 0)
        ab->impact = &(ad->impacts[impactnum]);
    else
        ab->impact = NULL; // Use default impact
    ab->damage = damage;
    ab->interrupt = TRUE;

    SetDesired(ab);

  // Make sure moving angle equals face (it doesn't during a combat move)
    SetMoveAngle(GetFace());

  // Set last attack stuff
    lastattacknum = attacknum;
    lastattack = ad;
    lastattackticks = PlayScreen.GameFrame();

    return TRUE;
}

// Find attack based on the button the player pressed
BOOL TCharacter::ButtonAttack(int buttonid)
{
    if (!(IsDoing(ACTION_COMBAT) || IsDoing(ACTION_COMBATMOVE) || IsDoing(ACTION_ATTACK)))
        return FALSE;

    int dmgpcnt = random(1, 50) + random(1, 50);
    int attacknum, impactnum, damage;
    BOOL found = FindButtonAttack(buttonid, dmgpcnt, attacknum, impactnum, damage);
    if (found)
        return DoAttack(attacknum, impactnum, damage);
    else
        return FALSE;
}

// Find a random attack (for a monster)
BOOL TCharacter::RandomAttack(int pcnt)
{
    if (!(IsDoing(ACTION_COMBAT) || IsDoing(ACTION_COMBATMOVE) || IsDoing(ACTION_ATTACK)))
        return FALSE;

    int dmgpcnt = random(1, 50) + random(1, 50);
    int attacknum, impactnum, damage;
    BOOL found = FindPcntAttack(pcnt, dmgpcnt, attacknum, impactnum, damage);
    if (found)
        return DoAttack(attacknum, impactnum, damage);
    else 
        return FALSE;
}

// Do a specific attack
BOOL TCharacter::SpecificAttack(int attacknum)
{
    if (!(IsDoing(ACTION_COMBAT) || IsDoing(ACTION_COMBATMOVE) || IsDoing(ACTION_ATTACK)))
        return FALSE;

    PTCharacter targ = Fighting();
    int tdist = 0;
    if (targ)
        tdist = Distance(targ);

    int dmgpcnt = random(1, 50) + random(1, 50);
    int impactnum, damage;
    BOOL valid = IsValidAttack(attacknum, impactnum, damage, tdist, -1, -1, dmgpcnt, 0, 0);

    if (valid)
        return DoAttack(attacknum, impactnum, damage);
    else
        return FALSE;
}

BOOL TCharacter::Leap(int angle)
{
    if (!IsFighting())
        return FALSE;

    int roundangle = ((GetFace() + 15) & 0xE0); // round to 8 dirs
    int diff = (angle - roundangle) & 255;
    int anim = diff / 32;

    PTActionBlock ab = NULL;

    switch (anim)
    {
        case 0: ab = new TActionBlock("combatleapf", ACTION_COMBATLEAP); break;
        case 1: ab = new TActionBlock("combatleapfr", ACTION_COMBATLEAP); break;
        case 2: ab = new TActionBlock("combatleapr", ACTION_COMBATLEAP); break;
        case 3: ab = new TActionBlock("combatleapbr", ACTION_COMBATLEAP); break;
        case 4: ab = new TActionBlock("combatleapb", ACTION_COMBATLEAP); break;
        case 5: ab = new TActionBlock("combatleapbl", ACTION_COMBATLEAP); break;
        case 6: ab = new TActionBlock("combatleapl", ACTION_COMBATLEAP); break;
        case 7: ab = new TActionBlock("combatleapfl", ACTION_COMBATLEAP); break;
    }

    if (ab && doing)    // Copy current target
        ab->obj = doing->obj;

    SetDesired(ab);

    return TRUE;
}

BOOL TCharacter::Block()
{
    if (!IsFighting() || !IsDoing(ACTION_COMBAT))
        return FALSE;

    PTCharacter targ = (PTCharacter)doing->obj;

    char *blockanim = "block";
    BOOL synchronize = FALSE;

    if (targ &&                                                 // Char is attacking
        targ->IsDoing(ACTION_ATTACK) &&                          
        targ->GetFrame() < targ->GetDoing()->attack->blocktime) // And we're in time to block
    {
        blockanim = targ->GetDoing()->attack->blockname;        // Get desired block name
        if (!HasActionAni(blockanim))
            blockanim = "block";                                // Use default block
        else
        {
//          if (targ->GetDoing()->attack->flags & CA_SNAPBLOCK) // If special block, and needs snap, do snap
//              SnapDist(targ, targ->doing->impact->snapdist);
        }
    }

    if (!HasActionAni(blockanim)) // Do we have this particular block?
        return FALSE;

  // Ok, now start the block (note that AF_SYNCHRONIZE will cause frames to sync with attack
    PTActionBlock ab = new TActionBlock(blockanim, ACTION_BLOCK);
    ab->obj = doing->obj;
    ab->wait = random(chardata->blockmin, chardata->blockmax);
    SetDesired(ab);

  // Make sure moving angle equals face (it doesn't during a combat move)
    SetMoveAngle(GetFace());

    return TRUE;
}

BOOL TCharacter::Dodge()
{
    if (!IsFighting() || !IsDoing(ACTION_COMBAT))
        return FALSE;

    PTActionBlock ab = new TActionBlock("dodge", ACTION_DODGE);
    ab->obj = doing->obj;
    SetDesired(ab);

    return TRUE;
}

BOOL TCharacter::Invoke(int *spell, int len)
{
    char *n = "invoke";

    if (IsFighting())
        n = "invokec";

    if (!HasActionAni(n))
        return FALSE;

    PTActionBlock ab = new TActionBlock(n, ACTION_INVOKE);
    ab->obj = Fighting();
    int *tmp = new int[len+1];
    memcpy(tmp, spell, len);
    tmp[len] = 0xff;            // terminator
    ab->data = tmp;
    ab->priority = TRUE;        // don't interrupt spellcasting
    SetDesired(ab);

  // Make sure moving angle equals face (it doesn't during a combat move)
    SetMoveAngle(GetFace());

    return TRUE;
}

BOOL TCharacter::Pulp(S3DPoint vel, int piece_count, int blood_count)
{
    // create the action block
    PTActionBlock ab = new TActionBlock("pulped");
    ab->priority = TRUE;
    ab->action = ACTION_PULP;
    SetDesired(ab);

    SObjectDef def;
    memset(&def, 0, sizeof(SObjectDef));
    def.objclass = OBJCLASS_EFFECT;
    def.level = MapPane.GetMapLevel();
    def.pos = Pos();
    def.facing = GetFace();
    def.objtype = EffectClass.FindObjType("PULP");

    PTPulpEffect pulpstuff = (PTPulpEffect)MapPane.GetInstance(MapPane.NewObject(&def));
    if (!pulpstuff)
        return FALSE;

    // init all the params
    pulpstuff->Set(vel, this, piece_count, blood_count);

    return TRUE;
}

BOOL TCharacter::Burn()
{
    SObjectDef def;
    memset(&def, 0, sizeof(SObjectDef));
    def.objclass = OBJCLASS_EFFECT;
    def.level = MapPane.GetMapLevel();
    def.pos = Pos();
    def.facing = GetFace();
    def.objtype = EffectClass.FindObjType("BURNBABYBURN");

    PTBurnBabyBurnEffect burnbabyburnstuff = (PTBurnBabyBurnEffect)MapPane.GetInstance(MapPane.NewObject(&def));
    if (!burnbabyburnstuff)
        return FALSE;

    // init all the params
    burnbabyburnstuff->Set(this);

    return TRUE;
}

BOOL TCharacter::BeginCombat(PTCharacter target)
{
    if (target && target->IsDead())
        target = NULL;

    if (!target)
        target = FindClosestEnemy(GetFace(), 32);

    if (root->action == ACTION_COMBAT)
        return SetFighting(target);

    if (FindState("combat") < 0)
        return FALSE;

    PTActionBlock ab = new TActionBlock("combat", ACTION_COMBAT);
    ab->obj = target;
    ab->priority = TRUE;      // Don't interrupt me
    if (doing->Is("walk"))
        doing->priority = FALSE; // Allow him to override an EndCombat() action
    SetDesired(ab);

    if (GetScript())
        GetScript()->Trigger(TRIGGER_COMBAT); // Trigger the combat script

    nextattack = -1;

    return TRUE;
}

BOOL TCharacter::EndCombat()
{
    if (root->action != ACTION_COMBAT)
        return TRUE;

    if (FindState("walk") < 0)      // Avoid trying to set walk state in the future
        return FALSE;

    PTActionBlock ab = new TActionBlock("walk");
    ab->priority = TRUE;         // Don't interrupt me
    if (doing->Is("combat"))
        doing->priority = FALSE; // Allow him to override a BeginCombat() action
    SetDesired(ab);

  // Make sure moving angle equals face (it doesn't during a combat move)
    SetMoveAngle(GetFace());

    if ((PTPlayer)this == Player)
        TextBar.ClearHealthDisplay();

    return TRUE;
}

BOOL TCharacter::SetFighting(PTCharacter newtarget)
{
    if (root->action != ACTION_COMBAT)
        return BeginCombat(newtarget);
    if (doing->obj == newtarget)
        return TRUE;
    doing->obj = newtarget;
    desired->obj = newtarget;
    root->obj = newtarget;
    if (newtarget)
    {
        int newangle = AngleTo(newtarget);
        doing->angle = newangle;
        desired->angle = newangle;
        root->angle = newangle;

        if ((PTPlayer)this == Player)
            TextBar.SetHealthDisplay(newtarget->GetName(), newtarget->Health());
    }

    return TRUE;
}

void TCharacter::Wait(int waitlen)
{
    waittype = WAIT_TICKS;
    waitticks = waitlen;
}

BOOL TCharacter::PlayAnim(char *string)
{
    if (!HasActionAni(string))
        return FALSE;

    PTActionBlock ab = new TActionBlock(string, ACTION_ANIMATE);
    SetDesired(ab);

    return TRUE;
}

BOOL TCharacter::ExecutingQueued()
{
    if (desired && (desired->Is("walkl") || desired->Is("walkr")) &&
        (desired->target.x || desired->target.y))
        return TRUE;

    return FALSE;
}

int TCharacter::CursorType(PTObjectInstance inst)
{
    if (inst)
        return CURSOR_NONE;

    if (IsDead())
    {
        if (RealNumInventoryItems() > 0)
            return CURSOR_HAND;         // loot the corpse
        else
            return CURSOR_NONE;         // He's dead, Jim
    }

    if (Aggressive())
        return CURSOR_NONE;
//      return CURSOR_SWORDS;           // kick its ass

    return CURSOR_MOUTH;                // chat for a bit
}

BOOL TCharacter::Use(PTObjectInstance user, int with)
{
    if (with >= 0)
    {
        PTObjectInstance inst = MapPane.GetInstance(with);
        if (!inst)
            return FALSE;
        if (GetScript())
            GetScript()->Trigger(TRIGGER_GET, inst->GetName());
        if (user && user->GetScript())
            GetScript()->Trigger(TRIGGER_GIVE, inst->GetName());
        return TRUE;
    }

    if (IsDead())
    {
        // loot the corpse
        TInventoryIterator i(this);
        PTObjectInstance oi = i.Item();

        if (oi)
        {
            if ((DWORD)FindFreeInventorySlot() >= MAXINVITEMS)
                TextBar.Print("Can't carry any more.");
            else
            {
                oi->RemoveFromInventory();
                AddToInventory(oi);

                char buf[80];
                sprintf(buf, "%s taken from corpse of %s.", oi->GetName(), GetName());
                TextBar.Print(buf);
            }

            return TRUE;
        }

        return FALSE;
    }

    if (!Aggressive() && user == (PTObjectInstance)Player)
    {
      // Face eachother
        S3DPoint upos;
        user->GetPos(upos);
        int angle = ConvertToFacing(pos, upos);
        Face(angle);
        angle = ConvertToFacing(upos, pos);
        user->Face(angle);

      // Start DIALOG section
        if (GetScript())
            GetScript()->Trigger(TRIGGER_DIALOG);

        return TRUE;
    }

    return FALSE;
}

PTCharacter TCharacter::CharBlocking(PTObjectInstance inst, RS3DPoint pos, int radius)
{
    int list[MAXFOUNDOBJS];
    int n = MapPane.FindObjectsInRange(pos, list, 100, 0, 
        OBJCLASS_CHARACTER, MAXFOUNDOBJS, OBJSET_CHARACTER);

    for (int i = 0; i <= n; i++)
    {
        PTObjectInstance oi;

        if (i >= n)
            oi = (PTObjectInstance)Player;
        else
            oi = MapPane.GetInstance(list[i]);

        if (oi && oi != inst)
        {
            S3DPoint opos;
            oi->GetPos(opos);
            int dist = ::Distance(opos, pos);
            dist -= radius;
            dist -= ((PTCharacter)oi)->Radius();

            if (dist < 1)
                return (PTCharacter)oi;
        }
    }

    return NULL;
}

// ------------- Streaming functions ------------------

// Loads object data from the sector
void TCharacter::Load(RTInputStream is, int version, int objversion)
{
    TComplexObject::Load(is, version, objversion);

  // Get saved last pulse values (so we can figure what has happened to char)
    if (objversion < 1)
        return;

  // Last time any health/fatigue/mana was recovered
    is >> lasthealthrecov;
    is >> lastfatiguerecov;
    is >> lastmanarecov;
}

// Saves object data to the sector
void TCharacter::Save(RTOutputStream os)
{
    if(!strcmp(GetState(), "pulped") || !strcmp(GetState(), "to pulped"))
        return;
    TComplexObject::Save(os);

  // Last time any health/fatigue/mana was recovered
    os << lasthealthrecov;
    os << lastfatiguerecov;
    os << lastmanarecov;
}

// make a character visible
void TCharacter::MakeVisible()
{
    is_invisible = FALSE;

    PTCharAnimator animator = (PTCharAnimator)GetAnimator();

    for(int i = 0; i < animator->Get3DImagery()->NumMaterials(); ++i)
    {
        S3DMat mat;
        animator->Get3DImagery()->GetMaterial(i, &mat);

        D3DMATERIAL &m = mat.matdesc;

        m.ambient.a = 1.0f;
        m.diffuse.a = 1.0f;
        m.specular.a = 1.0f;

        animator->Get3DImagery()->SetMaterial(i, &mat);
    }
}

// make a character invisible
void TCharacter::MakeInvisible()
{
    is_invisible = TRUE;

    PTCharAnimator animator = (PTCharAnimator)GetAnimator();

    for(int i = 0; i < animator->Get3DImagery()->NumMaterials(); ++i)
    {
        S3DMat mat;
        animator->Get3DImagery()->GetMaterial(i, &mat);
        D3DMATERIAL &m = mat.matdesc;

        m.ambient.a = .5f;
        m.diffuse.a = .5f;
        m.specular.a = .5f;

        animator->Get3DImagery()->SetMaterial(i, &mat);
    }
}
