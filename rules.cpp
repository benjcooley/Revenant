// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  Rules.cpp - TRules object module                     *
// *************************************************************************

#include "revenant.h"
#include "parse.h"
#include "character.h"
#include "player.h"
#include "weapon.h"
#include "playscreen.h"
#include "rules.h"

extern TObjectClass CharacterClass;
extern TObjectClass PlayerClass;

static char errorparsingtag[] = "Error parsing tag %s";

// Tag is tester
#define TAGIS(t) (!stricmp(tag, t))

// *********************
// * SClassData Object *
// *********************

// Loads the character from the "CHAR.DEF" file
BOOL SClassData::Load(char *aname, TToken &t)
{
    strncpyz(name, aname, RESNAMELEN);

    t.SkipBlanks();
    if (!t.Is("BEGIN"))
        t.Error("Char block BEGIN expected");
    
  // Now get first trigger token
    t.LineGet();
    
  // Iterate through the triggers and setup trigger list
    BOOL ok;
    while (t.Type() != TKN_EOF && !t.Is("END"))
    {
      // Parse trigger tags now
        if (t.Type() != TKN_IDENT)
            t.Error("Class data keyword expected");

        char tag[40];
        strncpyz(tag, t.Text(), 40);

        t.WhiteGet();
        ok = TRUE;

      // Tags...
        if (TAGIS("STATREQS"))
        {
            for (int c = 0; c < NUM_PLRSTATS; c++)
            {
                if (c > 0)
                {
                    if (!t.Is(","))
                        t.Error("',' Expected");

                    t.WhiteGet();
                }
            
                if (t.Type() != TKN_NUMBER)
                    t.Error("Stat requirement value expected");

                statreqs[c] = t.Index();

                t.WhiteGet();
            }
        }
        else if (TAGIS("SKILLMODS"))
        {
            for (int c = 0; c < NUM_SKILLS; c++)
            {
                if (c > 0)
                {
                    if (!t.Is(","))
                        t.Error("',' Expected");

                    t.WhiteGet();
                }
            
                if (t.Type() != TKN_NUMBER)
                    t.Error("Skill modifier expected");

                skillmods[c] = t.Index();

                t.WhiteGet();
            }
        }
        else if (TAGIS("HEALTHMOD"))
        {
            ok = Parse(t, "%i", &healthmod);
        }
        else if (TAGIS("FATIGUEMOD"))
        {
            ok = Parse(t, "%i", &fatiguemod);
        }
        else if (TAGIS("MANAMOD"))
        {
            ok = Parse(t, "%i", &manamod);
        }
        else
            t.Error("Invalid class tag %s", t.Text());

        if (!ok)
            t.Error(errorparsingtag, tag);

        if (t.Type() != TKN_RETURN)
            t.Error("Return expected");

        t.LineGet();
    }

    if (!t.Is("END"))
        t.Error("Class block END expected");
    t.Get();

    return TRUE;
}

// ********************
// * SCharData Object *
// ********************

// Construct SCharData
SCharData::SCharData()
{
    objtype = -1;
    objclass = OBJCLASS_CHARACTER;

    flags = 0;
    for (int c = 0; c < NUMDAMAGETYPES; c++)
        damagemods[0] = 0;

    health = 25;
    fatigue = 25;
    mana = 0;

    blockfreq = 10;
    blockmin = 5;
    blockmax = 15;

    combatrangemin = 128;
    combatrangemax = combatrangemin + 64;
    
    playerblockmin = 10;
    playerblockstep = 5;
    playerblockinc = 10;

    strcpy(blocksounds, "clang1,clang2,clang3");
    strcpy(misssounds, "");

    weapontype = WT_HAND;
    weapondamage = 2;
    armorvalue = 1;
    defensemod = 0;
    attackmod = 0;

    sightmin = 30; sightmax = 100; sightrange = 64 * 5; sightangle = 64;
    hearingmin = 10; hearingmax = 50; hearingrange = 64 * 5;

    minattackfreq = 100; maxattackfreq = 250;

    walkspeed = -1; runspeed = -1; sneakspeed = -1; combatwalkspeed = -1; 

    arrowpos.x = 15; arrowpos.y = 15; arrowpos.z = 60;  // In front of by 15, and up at 45
    arrowspeed = 20;                       // 20 units per tick for arrow speed

    bowwait = 12;                          // Half a second
    bowaimspeed = 8;                       // Pivot speed when aiming bow

    maxattackrange = 32;

    strcpy(bodytype, "normal"); // Default male normal size
}

// Destroy SCharData
SCharData::~SCharData()
{
    attacks.Clear();
}

// Loads the character from the "CHAR.DEF" file
BOOL SCharData::Load(char *aname, TToken &t)
{
    strncpyz(name, aname, RESNAMELEN);

    if (!stricmp(name, "Default"))
    {
        objtype = -1; objclass = -1;
    }
    else
    {
        objtype = CharacterClass.FindObjType(name);
        if (objtype < 0)
        {
            objtype = PlayerClass.FindObjType(name);
            if (objtype < 0)
                t.Error("Invalid character type %s", name);
            else
                objclass = OBJCLASS_PLAYER;
        }
        else
            objclass = OBJCLASS_CHARACTER;
    }

    t.SkipBlanks();
    if (!t.Is("BEGIN"))
        t.Error("Char block BEGIN expected");
    
  // Now get first trigger token
    t.LineGet();
    
  // Iterate through the triggers and setup trigger list
    BOOL ok;
    PSCharAttackData lastattack = NULL;
    while (t.Type() != TKN_EOF && !t.Is("END"))
    {
      // Parse trigger tags now
        if (t.Type() != TKN_IDENT)
            t.Error("Char data keyword expected");

        char tag[40];
        strncpyz(tag, t.Text(), 40);

        t.WhiteGet();
        ok = TRUE;

      // Tags...
        if (TAGIS("ATTACK"))
        {
            SCharAttackData ad;

            memset(&ad, 0, sizeof(SCharAttackData));

            ok = Parse(t, "%30s, %i, %i, %30s, %30s, %30s, %30s, "
                "%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i",
                ad.attackname, &ad.flags, &ad.button, 
                ad.responsename, ad.blockname, ad.missname, ad.chainname, 
                &ad.blocktime, &ad.impacttime, &ad.nextwait, &ad.chainexptime, 
                &ad.mindist, &ad.maxdist, &ad.hitminrange, &ad.hitmaxrange,
                &ad.hitangle, &ad.damagemod, &ad.fatigue, &ad.attackskill, 
                &ad.weaponmask, &ad.weaponskill, &ad.attackpcnt);

            if (ok)
            {
                if (ad.responsename[0] != NULL)
                    ad.flags = (ad.flags & ~CA_SPECIAL) | CA_RESPONSE;

                int num = attacks.Add(ad);
                lastattack = &(attacks[num]);
            }
        }
        else if (TAGIS("MAGICATTACK"))
        {
            SCharAttackData ad;

            memset(&ad, 0, sizeof(SCharAttackData));

            ok = Parse(t, "%30s, %i, %i, %i, %i, %i, %31s, %i, %i, %i",
                ad.attackname, &ad.flags, &ad.button, 
                &ad.attackpcnt, &ad.mindist, &ad.maxdist,
                ad.spellname, &ad.spellsource.x, &ad.spellsource.y, &ad.spellsource.z);

            if (ok)
            {
                ad.flags |= CA_MAGICATTACK;

                int num = attacks.Add(ad);
                lastattack = &(attacks[num]);
            }
        }
        else if (TAGIS("PLAYANIM"))
        {
            SCharAttackData ad;

            memset(&ad, 0, sizeof(SCharAttackData));

            ok = Parse(t, "%30s, %i, %i, %i, %i, %i",
                ad.attackname, &ad.flags, &ad.button,        
                &ad.mindist, &ad.maxdist, &ad.attackpcnt);

            if (ok)
            {
                ad.flags |= CA_PLAYANIM;

                int num = attacks.Add(ad);
                lastattack = &(attacks[num]);
            }
        }
        else if (TAGIS("IMPACT") || TAGIS("CHARIMPACT"))
        {
            if (TAGIS("CHARIMPACT"))
            {
                if (numimpacts >= MAXCHARIMPACTS)
                    t.Error("Too many CHARIMPACT tags for this character");
            }
            else
            {
                if (!lastattack)
                    t.Error("ATTACK tag must preceed its IMPACT tags");
                else if (lastattack->flags & CA_MAGICATTACK)
                    t.Error("IMPACT can not follow a MAGICATTACK tag");

                if (lastattack->numimpacts >= MAXATTACKIMPACTS)
                    t.Error("Too many IMPACT tags for this ATTACK");
            }

            SCharAttackImpact ai;

            memset(&ai, 0, sizeof(SCharAttackImpact));

            ok = Parse(t, "%30s, %i, %30s, %i, %i, %i, %i, %i",
                ai.impactname, &ai.flags, ai.loopname, 
                &ai.looptime, &ai.damagemin, &ai.damagemax, &ai.snapdist, &ai.snaptime);

            if (ai.loopname[0] != NULL)
            {
                if (!(ai.flags & (CAI_STUN | CAI_KNOCKDOWN | CAI_DEATH)))
                    t.Error("Used 'loop' animation for impact with no STUN, KNOCKDOWN, or DEATH flag");
            }

            if (ok)
            {
                if (TAGIS("CHARIMPACT"))
                {
                    memcpy(&(impacts[numimpacts]), &ai, 
                        sizeof(SCharAttackImpact));
                    numimpacts++;
                }
                else
                {
                    memcpy(&(lastattack->impacts[lastattack->numimpacts]), &ai, 
                        sizeof(SCharAttackImpact));
                    lastattack->numimpacts++;
                }
            }
        }
        else if (TAGIS("CLASS"))
        {
            if (objclass == OBJCLASS_CHARACTER)
                t.Error("Monsters/NPC's do not have character classes");

            char classname[RESNAMELEN];
            ok = Parse(t, "%30s", classname);
            if (ok)
                classdata = Rules.GetClass(classname);
        }
        else if (TAGIS("DAMAGEMODS"))
        {
            for (int c = 0; c < NUMDAMAGETYPES; c++)
            {
                if (c > 0)
                {
                    if (!t.Is(","))
                        t.Error("',' Expected");

                    t.WhiteGet();
                }
            
                if (t.Type() != TKN_NUMBER)
                    t.Error("Damage type modifier expected");

                damagemods[c] = t.Index();

                t.WhiteGet();
            }
        }
        else if (TAGIS("FLAGS"))
        {
            ok = Parse(t, "%i", flags);
        }
        else if (TAGIS("BLOCK"))
        {
            ok = Parse(t, "%i, %i, %i", &blockfreq, &blockmin, &blockmax);
        }
        else if (TAGIS("PLAYERBLOCK"))
        {
            ok = Parse(t, "%i, %i, %i", &playerblockmin, &playerblockstep, &playerblockinc);
        }
        else if (TAGIS("ARMOR"))
        {
            ok = Parse(t, "%i", &armorvalue);
        }
        else if (TAGIS("DEFENSEMOD"))
        {
            ok = Parse(t, "%i", &defensemod);
        }
        else if (TAGIS("ATTACKMOD"))
        {
            ok = Parse(t, "%i", &attackmod);
        }
        else if (TAGIS("ATTACKFREQ"))
        {
            ok = Parse(t, "%i, %i", &minattackfreq, &maxattackfreq);
        }
        else if (TAGIS("WEAPONTYPE"))
        {
            ok = Parse(t, "%i", &weapontype);
        }
        else if (TAGIS("WEAPONDAMAGE"))
        {
            ok = Parse(t, "%i", &weapondamage);
        }
        else if (TAGIS("BLOCKSOUNDS"))
        {
            ok = Parse(t, "%31s", blocksounds);
        }
        else if (TAGIS("MISSSOUNDS"))
        {
            ok = Parse(t, "%31s", misssounds);
        }
        else if (TAGIS("GROUPS"))
        {
            ok = Parse(t, "%48s", groups);
        }
        else if (TAGIS("ENEMIES"))
        {
            ok = Parse(t, "%48s", enemies);
        }
        else if (TAGIS("SIGHT"))
        {
            ok = Parse(t, "%i, %i, %i, %i", &sightmin, &sightmax, &sightrange, &sightangle);
        }
        else if (TAGIS("HEARING"))
        {
            ok = Parse(t, "%i, %i, %i", &hearingmin, &hearingmax, &hearingrange);
        }
        else if (TAGIS("MANA"))
        {
            ok = Parse(t, "%i", &mana);
        }
        else if (TAGIS("FATIGUE"))
        {
            ok = Parse(t, "%i", &fatigue);
        }
        else if (TAGIS("HEALTH"))
        {
            ok = Parse(t, "%i", &health);
        }
        else if (TAGIS("COMBATRANGE"))
        {
            ok = Parse(t, "%i", &combatrangemin);
            if (ok && t.Is(","))
                ok = Parse(t, ", %i", &combatrangemax);
            else
                combatrangemax = combatrangemin + 64;
        }
        else if (TAGIS("MAXATTACKRANGE"))
        {
            ok = Parse(t, "%i", &maxattackrange);
            if(!ok)
                maxattackrange = 32;
        }
        else if (TAGIS("WALKSPEED"))
        {
            ok = Parse(t, "%i", &walkspeed);
        }
        else if (TAGIS("SNEAKSPEED"))
        {
            ok = Parse(t, "%i", &sneakspeed);
        }
        else if (TAGIS("RUNSPEED"))
        {
            ok = Parse(t, "%i", &runspeed);
        }
        else if (TAGIS("COMBATWALKSPEED"))
        {
            ok = Parse(t, "%i", &combatwalkspeed);
        }
        else if (TAGIS("BODYTYPE"))
        {
            ok = Parse(t, "%30s", bodytype);
        }
        else if (TAGIS("SWIPECOLOR"))
        {
            ok = Parse(t, "%b, %b, %b", &swipecolor.red, &swipecolor.green, &swipecolor.blue);
        }
        else if (TAGIS("ARROWPOS"))
        {
            ok = Parse(t, "%i, %i, %i", &arrowpos.x, &arrowpos.y, &arrowpos.z);
        }
        else if (TAGIS("ARROWSPEED"))
        {
            ok = Parse(t, "%i", &arrowspeed);
        }
        else if (TAGIS("BOWWAIT"))
        {
            ok = Parse(t, "%i", &bowwait);
        }
        else if (TAGIS("BOWAIMSPEED"))
        {
            ok = Parse(t, "%i", &bowaimspeed);
        }
        else
            t.Error("Invalid character tag %s", t.Text());

        if (!ok)
            t.Error(errorparsingtag, tag);

        if (t.Type() != TKN_RETURN)
            t.Error("Return expected");

        t.LineGet();
    }

    if (objclass == OBJCLASS_PLAYER && !classdata)
        t.Error("Class required for player characters");

    if (!t.Is("END"))
        t.Error("Char block END expected");
    t.Get();

    return TRUE;
}

// *****************
// * TRules Object *
// *****************

// Initializes character data stuff
BOOL TRules::Initialize()
{
    if (initialized)
        return TRUE;

    chardata.DeleteAll();
    classdata.DeleteAll();
    def = NULL;

    if (!Load())
        return FALSE;

    initialized = TRUE;

    return TRUE;
}

// Closes the area manager
void TRules::Close()
{
    chardata.DeleteAll();
    classdata.DeleteAll();
}

// Loads all areas from the "RULES.DEF" file
BOOL TRules::Load()
{
    char fname[MAX_PATH];
    sprintf(fname, "%s%s", ClassDefPath, "rules.def");

    FILE *fp = popen(fname, "rb");
    if (!fp)
        FatalError("Unable to find character info file RULES.DEF");

    TFileParseStream s(fp, fname);
    TToken t(s);

    if (!t.DefineGet())
        t.Error("Syntax error in header");

  // Do block loop
    BOOL ok;
    while (t.Type() != TKN_EOF)
    {
      // Parse trigger tags now
        if (t.Type() != TKN_IDENT)
            t.Error("Rules block name or tag expected");

        char tag[40];
        strncpyz(tag, t.Text(), 40);

        t.WhiteGet();
        ok = TRUE;

      // Tags...
        if (TAGIS("DAYLENGTH"))
        {
            ok = Parse(t, "%i", &daylength);
        }
        else if (TAGIS("TWILIGHT"))
        {
            ok = Parse(t, "%i, %i", &twilight, &twilightsteps);
            if (GameSpeed == 5)
                twilightsteps = ConvertMinutesToFrames(twilight); // Smooth ambient fading
        }
        else if (TAGIS("HEALTHDATA"))
        {
            ok = Parse(t, "%i, %i, %i", &healthperlevel, &healthrecovval, &healthrecovrate);
        }
        else if (TAGIS("FATIGUEDATA"))
        {
            ok = Parse(t, "%i, %i, %i", &fatigueperlevel, &fatiguerecovval, &fatiguerecovrate);
        }
        else if (TAGIS("MANADATA"))
        {
            ok = Parse(t, "%i, %i, %i", &manaperlevel, &manarecovval, &manarecovrate);
        }
        else if (TAGIS("POISONDATA"))
        {
            ok = Parse(t, "%i, %i", &poisondamageval, &poisondamagerate);
        }
        else if (TAGIS("STEALTH"))
        {
            ok = Parse(t, "%i, %i, %i", &maxstealth, &sneakstealth, &minstealth);
        }
        else if (TAGIS("CHARACTER"))
        {

            char charname[MAXNAMELEN];
            ok = Parse(t, "%s\n", charname);
            
            if (ok)
            {
                PSCharData data = new SCharData;

                for (int c = 0; c < chardata.NumItems(); c++)
                {
                    if (!stricmp(chardata[c]->name, charname))
                        t.Error("More than one %s in RULES.DEF file", charname);
                }

                if (!data->Load(charname, t))
                    t.Error("Error loading char data");
                
                chardata.Add(data);

              // Set default char data object (-1 for objtype)  
                if (data->objtype < 0)
                    def = data; 

                t.Get();
            }
        }
        else if (TAGIS("CLASS"))
        {
            char classname[RESNAMELEN];
            ok = Parse(t, "%s\n", classname);

            if (ok)
            {
                PSClassData cl = new SClassData;

                if (!cl->Load(classname, t))
                    t.Error("Error loading class data");
                
                classdata.Add(cl);

                t.Get();
            }
        }
        else
            t.Error("Invalid RULES.DEF block or tag %s", t.Text());

        if (!ok)
            t.Error(errorparsingtag, tag);

        if (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
            t.Error("Return expected");
            
        if (!t.DefineGet())
            t.Error("RULES.DEF syntax error");
    }
    
    fclose(fp);

    return TRUE;
}

PSClassData TRules::GetClass(char *name)
{
    for (int c = 0; c < classdata.NumItems(); c++)
    {
        if (!stricmp(classdata[c]->name, name))
            return classdata[c];
    }

    return NULL;
}

PSCharData TRules::GetCharData(int objtype, int objclass)
{
    if (!initialized)
        return NULL;

    for (int c = 0; c < chardata.NumItems(); c++)
    {
        if (chardata[c]->objtype == objtype && chardata[c]->objclass == objclass)
            return chardata[c];
    }
    
    return def;
}
