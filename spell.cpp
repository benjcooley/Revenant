// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                          Spell.cpp - Spell                            *
// *************************************************************************

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <d3drmwin.h>
#include <math.h>
#include "d3dmacs.h"
#include "d3dmath.h"

#include "revenant.h"
#include "spell.h"
#include "effect.h"
#include "statusbar.h"
#include "parse.h"
#include "mappane.h"
#include "complexobj.h"
#include "character.h"
#include "3dimage.h"

extern TObjectClass EffectClass;
extern TObjectClass TalismanClass;

// ********************
// * SSpellData Object *
// ********************

// Construct SSpellData
SSpellData::SSpellData()
{
    // clear all the values
    flags = SF_NONE;
    memset(name, 0, NAMELEN);
    memset(objname, 0, RESNAMELEN);
    desc = NULL;
    damagetype = 0;
    memset(invoke, 0, RESNAMELEN);
    effectstart = 0;
}

// destructor
SSpellData::~SSpellData()
{
    if(desc)
        delete [] desc;
}

// Loads the spells from the "RULES.DEF" file
BOOL SSpellData::Load(char *aname, TToken &t)
{
    strncpyz(name, aname, RESNAMELEN);

    t.SkipBlanks();
    if (!t.Is("BEGIN"))
        t.Error("Spell def BEGIN expected");
    
  // Now get first trigger token
    t.LineGet();
    
  // Iterate through the triggers and setup trigger list
    while (t.Type() != TKN_EOF && !t.Is("END"))
    {
      // Parse trigger tags now
        if (t.Type() != TKN_IDENT)
            t.Error("Spell def keyword expected");

      // Tags...
        if(t.Is("NAME"))
        {
            if(!Parse(t, "NAME %30s\n", objname))
                t.Error("Error parsing NAME tag");
        }
        else if(t.Is("DESCRIPTION"))
        {
            char buf[1024];
            if(!Parse(t, "DESCRIPTION %s\n", buf))
                t.Error("Error parsing DESCRIPTION tag");

            desc = new char[strlen(buf) + 1];
            strcpy(desc, buf);  
        }
        else if (t.Is("DAMAGETYPE"))
        {
            if(!Parse(t, "DAMAGETYPE %i\n", &damagetype))
              t.Error("Error parsing DAMAGETYPE tag");
        }
        else if (t.Is("FLAGS"))
        {
            if (!Parse(t, "FLAGS %i\n", &flags))
                t.Error("Error parsing FLAGS tag");
        }
        else if (t.Is("ANIMATION"))
        {
            if (!Parse(t, "ANIMATION %30s\n", invoke))
                t.Error("Error parsing ANIMATION tag");
        }
        else if (t.Is("DELAY"))
        {
            if (!Parse(t, "DELAY %i\n", &effectstart))
                t.Error("Error parsing DELAY tag");
        }
        else if(t.Is("VARIANT"))
        {
            SSpellVariant var;
            char temp[256];

            if (!Parse(t, "VARIANT %s, %i, %s, %s, %i, %i, %i, %i, %i, %i, %i, %i\n", var.name, &var.type, temp, var.effect,
              &var.mana, &var.nextspellwait, &var.mindamage, &var.maxdamage, &var.skilllevel, &var.height, &var.facing, &var.ani_delay))
                t.Error("Error parsing VARIANT tag");

            var.nextspellwait *= 24;
            
            for(int i = 0; i < MAXTALISMANLEN; ++i)
                var.talismans[i] = 0;
            
            strncpyz(var.talismans, temp, MAXTALISMANLEN);

            variants.Add(var);
        }
        else
            t.Error("Invalid spell tag %s", t.Text());
    }

    if (!t.Is("END"))
        t.Error("Spell def END expected");

    return TRUE;
}

// ***************************
// *    TSpellList Object    *
// ***************************

// Initializes spell data stuff
BOOL TSpellList::Initialize()
{
    if (initialized)
        return TRUE;

    spelldata.DeleteAll();

    if (!Load())
        return FALSE;

    initialized = TRUE;

    return TRUE;
}

// Closes the area manager
void TSpellList::Close()
{
    spelldata.DeleteAll();
}

// Loads all areas from the "SPELL.DEF" file
BOOL TSpellList::Load()
{
    char fname[MAX_PATH];
    sprintf(fname, "%s%s", ClassDefPath, "spell.def");

    FILE *fp = popen(fname, "rb");
    if (!fp)
        FatalError("Unable to find spell info file SPELL.DEF");

    TFileParseStream s(fp, fname);
    TToken t(s);

    if (!t.DefineGet())
        t.Error("Syntax error in header");

    while (t.Type() != TKN_EOF)
    {
        char spellname[MAXNAMELEN];
        if (!Parse(t, "SPELL %s\n", spellname))
            t.Error("SPELL \"name\" expected");

        PSSpellData data = new SSpellData;

        if (!data->Load(spellname, t))
            t.Error("Error loading spell data");
        
        spelldata.Add(data);

        if (!t.DefineGet())
            t.Error("Syntax error between spell blocks");
    }
    
    fclose(fp);

    return TRUE;
}

// return spell by talismans list
PSSpellData TSpellList::GetSpellDataByTalismans(char* talismans)
{
    PSSpellData return_spell = NULL;
    int *tal = (int *)malloc(TalismanClass.NumTypes() * sizeof(int));
    int *cmp = (int *)malloc(TalismanClass.NumTypes() * sizeof(int));
    
    TSpellList::GetTalList(talismans, tal);

    for(int i = 0; i < spelldata.NumItems(); i++)
    {
        for(int j = 0; j < spelldata[i]->variants.NumItems(); j++)
        {
            TSpellList::GetTalList(spelldata[i]->variants[j].talismans, cmp);

            if(TSpellList::CompareTalList(tal, cmp))
                return_spell = spelldata[i];
        }
    }

    free(tal);
    free(cmp);
    
    return return_spell;
}

// return spell data based on name
PSSpellData TSpellList::GetSpellDataByName(char* name)
{
    for(int i = 0; i < spelldata.NumItems(); i++)
    {
        if(!stricmp(spelldata[i]->name, name))
            return spelldata[i];

        for(int j = 0; j < spelldata[i]->variants.NumItems(); j++)
        {           
            if(!stricmp(spelldata[i]->variants[j].name, name))
                return spelldata[i];
        }
    }

    return NULL;
}

// return an talisman array, this is a static function
void TSpellList::GetTalList(char* string, int* tal)
{
    int i, j;

    for(i = 0; i < TalismanClass.NumTypes(); i++)
        tal[i] = 0;

    for(i = 0; i < (signed)strlen(string); i++)
    {
        for(j = 0; j < TalismanClass.NumTypes(); j++)
        {
            char code = TalismanClass.GetStat(j, "Code");

            if(toupper(code) == toupper(string[i]))
            {
                ++tal[j];
                break;
            }
        }
    }
}

// compare tal lists
BOOL TSpellList::CompareTalList(int* tal1, int* tal2)
{
    for(int i = 0; i < TalismanClass.NumTypes(); i++)
    {
        if(tal1[i] != tal2[i])
            return FALSE;
    }

    return TRUE;
}

// return variant data based on talismans
PSSpellVariant TSpellList::GetVariantDataByTalismans(char* talismans)
{
    PSSpellVariant return_variant = NULL;
    int *tal = (int *)malloc(TalismanClass.NumTypes() * sizeof(int));
    int *cmp = (int *)malloc(TalismanClass.NumTypes() * sizeof(int));

    TSpellList::GetTalList(talismans, tal);

    for(int i = 0; i < spelldata.NumItems(); i++)
    {
        for(int j = 0; j < spelldata[i]->variants.NumItems(); j++)
        {
            TSpellList::GetTalList(spelldata[i]->variants[j].talismans, cmp);

            if(TSpellList::CompareTalList(tal, cmp))
                return_variant = &spelldata[i]->variants[j];
        }
    }

    free(tal);
    free(cmp);

    return return_variant;
}

// return variant data based on name
PSSpellVariant TSpellList::GetVariantDataByName(char* name)
{
    for(int i = 0; i < spelldata.NumItems(); i++)
    {
        for(int j = 0; j < spelldata[i]->variants.NumItems(); j++)
        {           
            if(!stricmp(spelldata[i]->variants[j].name, name))
                return &spelldata[i]->variants[j];
        }
    }

    return NULL;
}

// **************
// *** TSpell ***
// **************

// TSpell Constructor
TSpell::TSpell(PTObjectInstance invoke, 
    PTObjectInstance *targ, int numtargs, PS3DPoint sourcepos, 
    PSSpellData dat, PSSpellVariant var, PTSpell mtr)
{ 
    invoker = invoke; 

    if (numtargs < 1)
        numtargs = 1;
    if (numtargs >= MAXSPELLTARGETS)
        numtargs = MAXSPELLTARGETS;
    targetnum = numtargs; 
    if (!targ)
        targets[0] = invoker;
    else
        memcpy(targets, targ, numtargs * sizeof(PTObjectInstance));

    spell = dat; 
    variant = var; 
    timer = -1; 
    master = mtr;
    wait = dat->effectstart;

    if (sourcepos)
        source = *sourcepos;
    else
        source.x = source.y = source.z = -1;    // Means not used
}

// Returns source pos, or right hand pos if 'source' is (-1,-1,-1).
BOOL TSpell::GetSourcePos(RS3DPoint sourcepos)
{
    if (source.x != -1 || source.y != -1 || source.z != -1) // If source used...
    {
        sourcepos = source;
        return TRUE;
    }

  // If no source, use character's hand as the source
    sourcepos.x = sourcepos.y = sourcepos.z = 0;
    if (invoker->GetAnimator() && (invoker->GetImagery()->GetHeader()->imageryid != OBJIMAGE_MESH3D))
    {
        if (!((PT3DAnimator)invoker->GetAnimator())->GetObjectMapPos("rhand", sourcepos))
            return FALSE;
        int z = sourcepos.z;
        ConvertToVector(invoker->GetFace(), Distance(sourcepos), sourcepos);
        sourcepos.z = z;
    }

    return TRUE;
}

// get by name
void TSpell::SetByName(char* name)
{
    spell = SpellList.GetSpellDataByName(name);
    variant = SpellList.GetVariantDataByName(name);
}

// get by talismans
void TSpell::SetByTalismans(char* talismans)
{
    spell = SpellList.GetSpellDataByTalismans(talismans);
    variant = SpellList.GetVariantDataByTalismans(talismans);
}

BOOL TSpell::Timer()
{
    if (timer > 0) 
        --timer; 
    
    if(!effect && wait < 0) 
        timer = 0;

    if(wait > 0)
        --wait;
    else if(wait == 0)
    {
        --wait;

        // create the effect id
        SObjectDef def;
        memset(&def, 0, sizeof(SObjectDef));
        def.objclass = OBJCLASS_EFFECT;
        def.level = MapPane.GetMapLevel();
        invoker->GetPos(def.pos);
        S3DPoint spos;
        if (GetSourcePos(spos))
            def.pos += spos;
        else
            def.pos.z += variant->height;  // Height is only used as a last resort
        if (variant->facing)
            def.facing = invoker->GetFace();
        else
            def.facing = 0;
        def.objtype = EffectClass.FindObjType(variant->effect);

        effect = MapPane.GetInstance(MapPane.NewObject(&def));

        if (effect)
            ((PTEffect)effect)->SetSpell(this);
    }

    return timer == 0; 
}

void TSpell::Damage(PTObjectInstance ch)
{
    int mindam = variant->mindamage;
    int maxdam = variant->maxdamage;

    if (((PTCharacter)ch)->IsMagicResistant())
    {
        // take the magic resistance as the percentage of damage to allow...
        mindam -= (int)(mindam * ((PTCharacter)ch)->GetMagicResistance());
        maxdam -= (int)(maxdam * ((PTCharacter)ch)->GetMagicResistance());
    }

    ((PTCharacter)ch)->Damage(random(mindam, maxdam), spell->damagetype);
}

void TSpell::ManaDrain()
{   
    ((PTCharacter)invoker)->SetMana(((PTCharacter)invoker)->Mana() - variant->mana);
    if(((PTCharacter)invoker)->Mana() > ((PTCharacter)invoker)->MaxMana())
        ((PTCharacter)invoker)->SetMana(((PTCharacter)invoker)->MaxMana());
    if(((PTCharacter)invoker) == ((PTCharacter)Player))
        StaminaBar.ChangeLevel(((PTCharacter)invoker)->Mana() * 1000 / ((PTCharacter)invoker)->MaxMana());
}

// *********************
// *** TSpellManager ***
// *********************

// cast a spell by using its name
BOOL TSpellManager::CastByName(char* name, PTObjectInstance invoker, 
    PTObjectInstance *targets, int numtargs, PS3DPoint sourcepos, PTSpell mst)
{
    PSSpellData spell_data = SpellList.GetSpellDataByName(name);
    PSSpellVariant variant_data = SpellList.GetVariantDataByName(name);

    if(!spell_data || !variant_data || wait || ((PTCharacter)invoker)->Mana() < variant_data->mana)
        return FALSE;

    PTSpell spell = new TSpell(invoker, targets, numtargs, sourcepos, spell_data, variant_data, mst);
    spell->SetByName(name);

    ((PTCharacter)invoker)->SetCast(spell_data->invoke, (targets)?(targets[0]):NULL, variant_data->ani_delay);
    wait = variant_data->nextspellwait;

    spell->ManaDrain();

    spells.Add(spell);

    return TRUE;
}

// cast a spell by using the talismans
BOOL TSpellManager::CastByTalismans(char* talismans, PTObjectInstance invoker, 
    PTObjectInstance *targets, int numtargs, PS3DPoint sourcepos, PTSpell mst)
{
    PSSpellData spell_data = SpellList.GetSpellDataByTalismans(talismans);
    PSSpellVariant variant_data = SpellList.GetVariantDataByTalismans(talismans);

    if(!spell_data || !variant_data || wait || ((PTCharacter)invoker)->Mana() < variant_data->mana)
        return FALSE;

    PTSpell spell = new TSpell(invoker, targets, numtargs, sourcepos, spell_data, variant_data, mst);
    spell->SetByTalismans(talismans);

    ((PTCharacter)invoker)->SetCast(spell_data->invoke, (targets)?(targets[0]):NULL, variant_data->ani_delay);
    wait = variant_data->nextspellwait;

    spell->ManaDrain();
    
    spells.Add(spell);

    return TRUE;
}

void TSpellManager::Pulse()
{
    if(wait > 0)
        --wait;

    int size = spells.NumItems();
    
    for(int i = 0; i < size; ++i) 
    {
        if(!spells.Used(i))
            continue;

        spells[i]->Pulse(); 
        
        if(spells[i]->Timer())
            spells.Remove(i); 
    }
}

int TSpellManager::GetDefense()
{
    int defense = 0;

    int size = spells.NumItems();
    
    for(int i = 0; i < size; ++i)
    {
        if(!spells.Used(i))
            continue;

        defense += spells[i]->GetDefense();
    }

    return defense;
}

int TSpellManager::GetOffense()
{
    int offense = 0;

    int size = spells.NumItems();
    
    for(int i = 0; i < size; ++i)
    {
        if(!spells.Used(i))
            continue;

        offense += spells[i]->GetOffense();
    }

    return offense;
}

int TSpellManager::GetSpellCount(char* spell)
{
    int count = 0;

    int size = spells.NumItems();

    for(int i = 0; i < size; ++i)
    {
        if(!spells.Used(i))
            continue;

        if(!stricmp(spells[i]->VariantData()->name, spell))
            ++count;
    }

    return count;
}