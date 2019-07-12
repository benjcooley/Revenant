// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                          Spell.cpp - Spell                            *
// *************************************************************************

#include "revenant.h"
#include "spell.h"
#include "parse.h"

extern TObjectClass EffectClass;

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

// Loads the spells from the "SPELL.DEF" file
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
			if (!Parse(t, "FLAGS %i\n", flags))
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

			if (!Parse(t, "VARIANT %s %i %s %s %i %i %i %i %i", var.name, &var.type, temp, var.effect,
			  &var.mana, &var.nextspellwait, &var.mindamage, &var.maxdamage, &skilllevel))
				t.Error("Error parsing VARIANT tag");
			
			for(int i = 0; i < MAXTALISMANLEN; ++i)
				var.talismans[i] = 0;

			for(i = 0; i < strlen(temp) && i < MAXTALISMANLEN; ++i)
				var.talismans[i] = temp[i];

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

// Loads all areas from the "AREA.DEF" file
BOOL TSpellList::Load()
{
	char fname[MAX_PATH];
	sprintf(fname, "%s%s", ClassDefPath, "spell.def");

	FILE *fp = popen(fname, "rb");
	if (!fp)
		FatalError("Unable to find spell info file SPELL.DEF");

	TFileParseStream s(fp);
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

	  // Set default spell data object (-1 for objtype)	
		if (data->objtype < 0)
			def = data; 

		if (!t.DefineGet())
			t.Error("Syntax error between spell blocks");
	}
	
	fclose(fp);

	return TRUE;
}

// return spell by talismans list
PSSpellData TSpellList::GetSpellDataByTalismans(char* talismans)
{
	for(int i = 0; i < spelldata.NumItems(); ++i)
	{
		for(int j = 0; j < variant.NumItems(); ++j)
		{
			BOOL flag = TRUE;
			for(int x = 0; x < MAXTALISMANLEN; ++x)
			{
				if(talismans[x] != spelldata[i].variant[j].talismans[x])
				{
					flag = FALSE;
					break;
				}
				if(!talismans[x])
					break;
			}

			if(flag)
				return spelldata[i];
		}
	}

	return NULL;
}

// return spell data based on name
PSSpellData TSpellList::GetSpellDataByName(char* name)
{
	for(int i = 0; i < spelldata.NumItems(); ++i)
	{
		if(!strcmp(spelldata[i].name, name))
			return spelldata[i];

		for(int j = 0; j < spelldata[i].variant.NumItems(); ++j)
		{			
			if(!strcmp(spelldata[i].variant[j].name, name))
				return spelldata[i];
		}
	}

	return NULL;
}

// return variant data based on talismans
PSSpellVariant TSpellList::GetVariantDataByTalismans(char* talismans)
{
	for(int i = 0; i < spelldata.NumItems(); ++i)
	{
		for(int j = 0; j < variant.NumItems(); ++j)
		{
			BOOL flag = TRUE;
			for(int x = 0; x < MAXTALISMANLEN; ++x)
			{
				if(talismans[x] != spelldata[i].variant[j].talismans[x])
				{
					flag = FALSE;
					break;
				}
				if(!talismans[x])
					break;
			}

			if(flag)
				return spelldata[i].variant[j];
		}
	}

	return NULL;
}

// return variant data based on name
PSSpellVariant TSpellList::GetVariantDataByName(char* name)
{
	for(int i = 0; i < spelldata.NumItems(); ++i)
	{
		for(int j = 0; j < spelldata[i].variant.NumItems(); ++j)
		{			
			if(!strcmp(spelldata[i].variant[j].name, name))
				return spelldata[i].variant[j];
		}
	}

	return NULL;
}

// **************
// *** TSpell ***
// **************

// TSpell Constructor
TSpell::TSpell(PTObjectInstance invoke, PTObjectInstance targ, int numtargs, PSSpellData dat, PSSpellVariant var, PTSpell mtr)
{ 
	invoker = invoke; 
	target = targ ? targ : invoker; 
	spell = dat; 
	variant = var; 
	timer = -1; 
	targetnum = numtargs; 
	master = mtr;
}

// get by name
void TSpell::SetByName(char* name)
{
	spell = SpellList->GetSpellDataByName(name);
	variant = SpellList->GetVariantDataByName(name);
}

// get by talismans
void TSpell::SetByTalismans(char* talismans)
{
	spell = SpellList->GetSpellDataByTalismans(talismans);
	variant = SpellList->GetVariantDataByTalismans(talismans);
}

BOOL TSpell::Timer()
{
	if (timer > 0) 
		--timer; 
	
	if(!effect && spell.effectstart < 0) 
		timer = 0;

	if(spell.effectstart > 0)
		--spell.effectstart;
	else if(spell.effectstart == 0)
	{
		--spell.effectstart;

		// create the effect id
		SObjectDef def;
		memset(&def, 0, sizeof(SObjectDef));
		def.objclass = OBJCLASS_EFFECT;
		def.level = MapPane.GetMapLevel();
		def.pos = Pos();
		def.facing = GetFace();
		def.objtype = EffectClass.FindObjType(variant.effect);

		PTObjectInstance effect = MapPane.GetInstance(MapPane.NewObject(&def));
	}

	return timer == 0; 
}

// *********************
// *** TSpellManager ***
// *********************

// cast a spell by using its name
BOOL TSpellManager::CastByName(char* name, PTObjectInstance invoker, PTObjectInstance target, int numtargs, PTSpell mst)
{
	PSSpellData spell_data = SpellList.GetSpellDataByName(name);
	PSVariantData variant_data = SpellList.GetVariantDataByName(name);

	if(!spell_data || !variant_data)
		return FALSE;

	TSpell spell(invoker, target, numtargs, &spell_data, &variant_data, mst);
	spell.SetByName(name);

	// create the action block
	PTActionBlock ab = new TActionBlock(spell_data.invoke);
	ab->obj = target;
	ab->data = spell;
	ab->priority = TRUE;
	invoker->SetDesired(ab);

	spells.Add(&spell);

	return TRUE;
}

// cast a spell by using the talismans
BOOL TSpellManager::CastByTalismans(char* talismans, PTObjectInstance invoker, PTObjectInstance target, int numtargs, PTSpell mst)
{
	PSSpellData spell_data = SpellList.GetSpellDataByTalismans(name);
	PSVariantData variant_data = SpellList.GetVariantDataByTalismans(name);

	if(!spell_data || !variant_data)
		return FALSE;

	TSpell spell(invoker, target, numtargs, &spell_data, &variant_data, mst);
	spell.SetByTalismans(talismans);

	// create the action block
	PTActionBlock ab = new TActionBlock(spell_data.invoke);
	ab->obj = target;
	ab->data = spell;
	ab->priority = TRUE;
	invoker->SetDesired(ab);

	spells.Add(&spell);

	return TRUE;
}