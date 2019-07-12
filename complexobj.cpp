// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                complexobj.cpp - TComplexObject module                 *
// *************************************************************************

#include <stdlib.h>

#include "revenant.h"
#include "textbar.h"
#include "complexobj.h"

//#define SHOWSTATES

#ifdef SHOWSTATES
char desname1[RESNAMELEN], desname2[RESNAMELEN], desname3[RESNAMELEN], desname4[RESNAMELEN];
#endif

// ******************** Action Block ************************

TActionBlock::TActionBlock(char *n, ACTION a)
{
    ClearBlock();
    strcpy(name, n);
    action = a;
}

TActionBlock::TActionBlock(char *n, char *str, ACTION a)
{
    ClearBlock();
    sprintf(name, "%s%s", n, str);
    action = a;
}

TActionBlock::TActionBlock(TActionBlock &ab, char *str, ACTION a)
{
    memcpy(this, &ab, sizeof(TActionBlock));
    if (str)
        strcpy(name, str);
    if (a != ACTION_NONE)
        action = a;
}

void TActionBlock::ClearBlock()
{
    name[0] = NULL;
    action = ACTION_ANIMATE;
    frame = -1; // Means set frame to default
    wait = 0;
    angle = moveangle = 0;
    turnrate = 16; // Default turn rate
    target.x = target.y = target.z = 0;
    obj = NULL;
    data = NULL;
    flags = 0;
    damage = 0;
    firsttime = TRUE;
    attack = NULL;
    impact = NULL;
}

BOOL TActionBlock::Is(char *s)
{
    char *p1 = name;
    char *p2 = s;

    while (*p1 && *p2 && 
             (
                *p2 == '?' ||                     // 1 char
                *p2 == '*' ||                     // 0 or more chars
                *p2 == '[' ||                     // Beginning of char list
                *p2 == ']' ||                     // Ending of char list
                (*p2 == '#' && isdigit(*p1)) ||   // Any digit
                tolower(*p2) == tolower(*p1)      // Regular match
             )
          )
    {
        if (*p2 == '[') // Multichar match
        {
            p2++;
            while (*p2 && *p2 != ']')
            {
                if (*p1 == *p2)
                {
                    p1++;
                    break;
                }
            }
            if (*p2)
                p2++;
        }
        else            // Ordinary match
        {
            p1++;
            if (*p2 != '*')
                p2++;
        }
    }
    
    if (*p2 == '*')
        p2++;

    if (*p1 || *p2)
        return FALSE;

    return TRUE;
}

BOOL TActionBlock::IsRight(char *state)
{
    int len = strlen(state);
    if (!strnicmp(name, state, len) && name[len] == 'r' && name[len + 1] == NULL)
        return TRUE;

    return FALSE;
}

BOOL TActionBlock::IsLeft(char *state)
{
    int len = strlen(state);
    if (!strnicmp(name, state, len) && name[len] == 'l' && name[len + 1] == NULL)
        return TRUE;

    return FALSE;
}

BOOL TActionBlock::IsStep(char *state)
{
    int len = strlen(state);
    if (!strnicmp(name, state, len) && 
      (name[len] == 'r' || name[len] == 'l') && name[len + 1] == NULL)
        return TRUE;

    return FALSE;
}

// TRUE if state is one of the master state (i.e. "attack1" is one of "attack")
BOOL TActionBlock::IsOneOf(char *state)
{
    int len = strlen(state);
    if (!strnicmp(name, state, len) && 
      (name[len] >= '0' && name[len] <= '9') && name[len + 1] == NULL)
        return TRUE;

    return FALSE;
}

// Returns the number at the end of a state
int TActionBlock::StateNum()
{
    int len = strlen(name);
    if (len <= 1)
        return 0;
    else
        return atol(name + len - 1);
}

static char stnamebuf[RESNAMELEN];

// Helper function to make it easy to make state names
char *StName(char *name, int num)
{
    sprintf(stnamebuf, "%s%d", name, num);
    return stnamebuf;
}

// Helper function to make it easy to make state names
char *StName(char *name, char *str)
{
    sprintf(stnamebuf, "%s%s", name, str);
    return stnamebuf;
}

BOOL TActionBlock::IsPartOf(char *prefix, char *state)
{
    if (state)
        sprintf(stnamebuf, "%s%s*", prefix, state);
    else
        sprintf(stnamebuf, "%s*", prefix, state);
    return Is(stnamebuf);
}

// Swaps the root part of the name with a new root
void TActionBlock::SwapRoot(char *root, char *newroot)
{
    int l = strlen(root);
    if (strnicmp(name, root, l) != 0)
        return;

    char buf[RESNAMELEN];
    strncpyz(buf, newroot, RESNAMELEN);
    strncatz(buf, name + l, RESNAMELEN);
    strcpy(name, buf);
}

// ******************** Complex Object ************************

// NOTE: DefaultRootState() will not be virtual (will only call COMPLEXOBJ version)
// when ClearComplexObj() is called from constructor!

void TComplexObject::ClearComplexObj()
{
    flags |= OF_PULSE | OF_COMPLEX; // Causes the PULSE function to be called for this object
    SetNotify(N_DELETING);          // Check for deleted objs in action blocks
    root = doing = desired = new TActionBlock(DefaultRootState() /* SEE ABOVE NOTE */);
    state = FindState(root->name);
}

void TComplexObject::Pulse()
{
    if (UpdatingBoundingRect)
        return;

    UpdateAction(GetMoveBits());        // Update current state given current movement flags
}

void TComplexObject::UpdateAction(int bits)
{
    int comstate = ResolveAction(bits);

    if (comstate == 0)
        comstate = commanddone ? COM_COMPLETED : COM_EXECUTING;

    if (doing)
    {
        // check firsttime (first frame) and priority (on completion)
        if (doing->firsttime)
            doing->firsttime = FALSE;

        if (comstate == COM_COMPLETED && doing->priority)
            doing->priority = FALSE;
    }

    if (desired)
    {
        if (desired == root)
            comstate = COM_COMPLETED;
        else
            comstate = TryCommand(desired, bits);
    }

    if (comstate == COM_COMPLETED || comstate == COM_IMPOSSIBLE)
    {
        SetDesired(NULL);
        TryCommand(root);
    }
}

int TComplexObject::ResolveAction(int bits)
{
    if (!doing)
        return COM_IMPOSSIBLE;

    return 0;
}

void TComplexObject::SetRoot(PTActionBlock ab)
{
    if (ab->noroot)
        return;

    if (ab != root)
    {
        if (root && root != doing && root != desired && root != ab)
            delete root;
        root = ab;
    }

    root->nowaitdone = TRUE;    // We can always interrupt a root state
    root->interrupt = FALSE;    // We don't ever interrupt another state
}

void TComplexObject::SetDoing(PTActionBlock ab)
{
    if (ab != doing)
    {
        if (doing && doing != root && doing != desired && doing != ab)
            delete doing;
        doing = ab;
    }
    root->interrupt = FALSE;    // Now that we're doing, don't interrupt
}

void TComplexObject::SetDesired(PTActionBlock ab)
{
    if (ab == NULL)             // NULL indicates we want to go back to root
        ab = root;

    if (desired && desired != doing &&  // If we have a priority action, and we aren't already
        desired->priority)              // playing it, don't replace it.
            return;

    if (ab != desired)                  // Set the desired command
    {
        if (desired && desired != doing && desired != root)
            delete desired;
        desired = ab;

#ifdef SHOWSTATES
        strcpy(desname4, desname3);
        strcpy(desname3, desname2);
        strcpy(desname2, desname1);
        strcpy(desname1, ab->name);
#endif
    }

    if (ab->interrupt && doing != ab && !doing->priority) // Interrupt flag means DO IT RIGHT NOW
    {
        ForceCommand(ab);
        ab->interrupt = FALSE;
    }
}

// Have object attempt to move into a new state
int TComplexObject::TryCommand(PTActionBlock ab, int bits)
{
  // Use root if desired is NULL
    if (ab == NULL)
        ab = root;

  // Wait till we're done animating (unless we're in root)
    if ((!doing->priority || desired == doing) &&       // Can't do if trying to change and priority isn't cleared
        (commanddone || !doing ||                       // Okay, command done, or not doing anything
        (doing && ab != doing && doing->nowaitdone) ||  // Or doing action is nowait action
        (desired && ab != desired && desired->interrupt))) // Or desired action is interrupt
        return ForceCommand(ab, bits); // Do new state when old one is done (or interrupted)
    else
        return COM_EXECUTING;   // Otherwise tell program we're still waiting
}

// Force the object into the given state
int TComplexObject::ForceCommand(PTActionBlock ab, int bits)
{
    if (doing && doing->priority && desired != doing)
        return COM_EXECUTING;

    if (ab == NULL)
        ab = root;

#ifdef SHOWSTATES
    if (this == (PTComplexObject)Player)
        TextBar.Print("[%s,%s,%s] %s %s %s %s", root->name, desired->name, doing->name,
            desname1, desname2, desname3, desname4);
#endif

    ab->interrupt = FALSE;  // Now that we've interrupted... set this to false.

    int newstate = -1;

    if (doing == NULL)
        newstate = FindState(ab->name);
    else
    {
      // ROOT2ROOT means don't do a transition back to the root state because the 
      // transition TO animation (i.e. "walk to say") has the transition BACK animation
      // in it already (has "say to walk" in it).  If current animation is root2root, and user
      // actually wants to go back to the root, don't bother looking for a transition ani,
      // just set directly back to root..
        if ((GetAniFlags() & AF_ROOT2ROOT) && desired && desired == root)
        {
            ab = root; // Return back too root right now
            newstate = FindState(ab->name);
        }
        else
        {
      // Try to find a transition from the current action to the new action.  If we can't
      // find one, we will just force it (see below).
            newstate = FindTransitionState(doing->name, ab->name);
        }

      // Can't find a transition, well then we go ahead and force it... 
        if (newstate < 0 && !ab->dontforce)
        {   
          // One of these things is bound to work!!
          newstate = FindState(ab->name);   // Use end state name
          if (newstate < 0)
            newstate = FindTransitionState(root->name, ab->name);  // Try transition from root
          if (newstate < 0)
            newstate = FindTransitionState(ab->name, root->name);  // Try transition to root
        }

        // flag transition if the two states aren't the same
        ab->transition = (stricmp(doing->name, ab->name) != 0);
    }

    if (newstate < 0)
        return COM_IMPOSSIBLE;

    SetState(newstate);
    SetDoing(ab);

  // Set the first frame (if valid)
    if (ab->frame >= 0)
    {
        SetFrame(min(max(ab->frame, 0), imagery->GetHeader()->states[newstate].frames - 1));
        ab->frame = -1;
    }


  // If the sychronize flag is set, synchronize frames with 'obj'.
  // Use for block/impact animations which must match the frame number of an associated
  // opponents attack animation.  Useful for any other synchronized animations as well.
    if (ab->obj && GetAniFlags() & AF_SYNCHRONIZE)
        SetFrame(ab->obj->GetFrame() + 1);

  // Make this block the root block if current or next animation is a root animation
    DWORD nextaniflags = imagery ? imagery->GetAniFlags(FindState(ab->name)) : 0;
    if ((GetAniFlags() & AF_ROOT || nextaniflags & AF_ROOT) && !ab->noroot)
        SetRoot(ab);

  // If this is a looping animation, set desired to the same action again, otherwise
  // set it to go back to the root state
    if (ab->loop ||
        (GetAniFlags() & AF_LOOPING) || (GetAniFlags() & AF_ROOT) ||
        (nextaniflags & AF_LOOPING) || (nextaniflags & AF_ROOT))
        SetDesired(ab);
    else
        SetDesired(NULL);

    return COM_EXECUTING;
}

// Loads object data from the sector
void TComplexObject::Load(RTInputStream is, int version, int objversion)
{
    if (objversion >= 1)
        LOAD_BASE(TObjectInstance)
    else
        TObjectInstance::Load(is, version, 0);

  // Load root state
    PTActionBlock ab;
    if (version < 7)
    {
        ab = new TActionBlock(DefaultRootState());
    }
    else
    {
        BYTE action;
        char name[RESNAMELEN];
        is >> action;
        is >> name;
        ab = new TActionBlock(name, (ACTION)action);
    }

    SetRoot(ab);
    SetDoing(ab);
    SetDesired(ab);
    state = FindState(ab->name);
}

// Saves object data to the sector
void TComplexObject::Save(RTOutputStream os)
{
    SAVE_BASE(TObjectInstance)

  // Save root state
    os << (BYTE)root->action;
    BYTE len = (BYTE)strlen(root->name);
    os << len;
    char *p = root->name;
    while (len)
    {
        os << (*p);
        p++;
        len--;
    }
}

void TComplexObject::Notify(int notify, void *ptr)
{
    // **** WARNING!!! MAKE SURE YOU CHECK FOR BROKEN LINKS AND DELETED OBJECTS HERE!!! ****
    // If you want to be notified, you must call SetNotify() in your contsructor

    if (sector == (PTSector)ptr)
        return;

    TObjectInstance::Notify(notify, ptr);

    if (root && root->obj && NOTIFY_DELETED(ptr, root->obj))
        root->obj = NULL;
    if (doing && doing->obj && NOTIFY_DELETED(ptr, doing->obj))
        doing->obj = NULL;
    if (desired && desired->obj && NOTIFY_DELETED(ptr, desired->obj))
        desired->obj = NULL;
}
