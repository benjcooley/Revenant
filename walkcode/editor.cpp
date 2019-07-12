// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 editor.cpp - EXILE editor routines                    *
// *************************************************************************

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <process.h>

#include "revenant.h"
#include "3dscene.h"
#include "directdraw.h"
#include "mappane.h"
#include "object.h"
#include "tile.h"
#include "editor.h"
#include "display.h"
#include "font.h"
#include "dls.h"
#include "chunkcache.h"
#include "playscreen.h"
#include "inventory.h"
#include "multictrl.h"
#include "command.h"
#include "cursor.h"
#include "script.h"
#include "multi.h"
#include "savegame.h"
#include "player.h"
#include "template.h"
#include "scroll.h"
#include "exit.h"
#include "textbar.h"
#include "statusbar.h"
#include "spellpane.h"
#include "file.h"
#include "3dimage.h"
#include "sound.h"

extern TObjectClass TileClass;
extern TObjectClass HelperClass;
extern PTBitmap PointerCursor;
//extern CBox ImageryBox;

PTMulti EditorData;
TConsolePane Console;
TScriptPane ScriptEditor;
TScrollEditorPane ScrollEditor;
TEditStatusPane StatusBar;
TEditToolsPane ToolBar;
TEditClassPane ClassPane;
PTTerrainTemplate TerrainTemplates = NULL;

HANDLE TConsolePane::cmdevents[2];    	 	// Events used by the command processor thread
int TConsolePane::cmdchar;					// Character passed to the command processor thread
char *TConsolePane::cmdline; 				// Pointer to the beginning of the command line

extern LPDIRECT3DDEVICE Device;			// Direct3D device
extern BOOL				BlitHardware;

/* enumeration of object flags */
char *objflags[] = OBJFLAGNAMES;

/* enumeration of light flags */
char *lightflags[] = { "DIR", "SUN", "MOON", NULL };

/* enumeration of container flags */
char *contflags[] = { "LOCKED", NULL };

/* general buffer for formated strings */
char buf[1024];

// Reads in handy-dandy location list
BOOL ReadMapLocationList();

// Define the strings needed for writing out CGS files
char *MakeRCInclude = {"#include \"forsaken.hgs\"\r\n\r\n"};
char *MakeRCImagery = {"IMAGERY "};
char *MakeRCBegin   = {"BEGIN\r\n"};
char *MakeRCBitmap  = {"    BITMAP "};
char *MakeRCFlags   = {"imflags ANIIM_UNLIT bmflags BM_8BIT | BM_ZBUFFER | BM_NORMALS | BM_REGPOINT | BM_COMPRESSED | BM_CHUNKED\r\n"};
char *MakeRCEnd     = {"END\r\n\r\n"};

char *RCClassDirs[] =
{
    {"Misc"},       // Item
    {"Equip"},      // Weapon
    {"Equip"},      // Armor
    {"Magic"},      // Talisman
    {"Misc"},       // Food
    {"Misc"},       // Container
    {"Misc"},       // Light Source
    {"Equip"},      // Tool
    {"Misc"},       // Money
    {""},           // Tile
    {"Misc"},       // Exit
    {"Chars"},      // Player
    {"Chars"},      // Character
    {"Misc"},       // Trap
    {"Misc"},       // Shadow
    {"Misc"},       // Helper
    {"Magic"},      // Fireball
    {"Magic"},      // Ice
    {"Magic"},      // Freeze
    {"Magic"},      // Lightning
    {"Magic"},      // Hole
    {"Equip"},      // Ammo
    {"Misc"},       // Scroll
    {"Equip"},      // Ranged Weapon
    {"Misc"}        // Effect
};

char *RCTileDirs[] =   // Directories where the different tile sets
{
    {"Misc"},
    {"Cave"},
    {"Dungeon"},
    {"Forest"},
    {"Keep"},
    {"KeepInt"},
    {"Labyrnth"},
    {"Ruin"},
    {"Swamp"},
    {"Wastelnd"},
    {"Town"},
    {"TownInt"}
};

char *RCPrefixes[] =   // Prefixes for the different tile sets
{
    {""},
    {"Cav"},
    {"Dun"},
    {"For"},
    {"Kep"},
    {"KIn"},
    {"Lab"},
    {"Run"},
    {"Swm"},
    {"Wst"},
    {"Twn"},
    {"TIn"}
};

BOOL IsSectorCommand = FALSE;

#define NUMRCDIRS sizearray(RCTileDirs)

char *sprintbit(int bits, char *enumbits[], char *buf)
{
	for (int i = 0; enumbits[i]; i++)
		if (bits & (1 << i))
			sprintf(buf, "%s %s", buf, enumbits[i]);

	return buf;
}

// Start and shut down editor
void StartEditor(BOOL starting)
{
	memset(buf, 0, 1024);

	EditorData = TMulti::LoadMulti("editor.dat");

	// Pause script processing
	TScript::PauseAllScripts();

	// Turn off full screen mode
	PlayScreen.SetFullScreen(FALSE);

	// Hide interfering panes
	Inventory.Hide();
	QuickSpells.Hide();
	HealthBar.Hide();
	StaminaBar.Hide();
	TextBar.Hide();
	MultiCtrl.Hide();

	// Console - text output
	Console.Initialize();
	PlayScreen.AddPane(&Console);

	// Status bar - handling selected objects
	StatusBar.Initialize();
	PlayScreen.AddPane(&StatusBar);

	// General toolbar - generic object and sector commands
	ToolBar.Initialize();
	PlayScreen.AddPane(&ToolBar);

	// Class pane, allows selecting of all objects in a palette
	ClassPane.Initialize();
	PlayScreen.AddPane(&ClassPane);

	// Script editor
	ScriptEditor.Initialize();
	PlayScreen.AddPane(&ScriptEditor);
	ScriptEditor.Hide();

	// General-purpose text editor
	ScrollEditor.Initialize();
	PlayScreen.AddPane(&ScrollEditor);
	ScrollEditor.Hide();

	ReadMapLocationList();

	Editor = TRUE;

	Console.Output("REVENANT ver 0.5\n");
	Console.Output("(c) 1998 Cinematix Studios, Inc.\n");
	Console.Output(PROMPT);

	NoScrollZBuffer = FALSE;
	MapPane.RedrawAll();

  // Set editor map position	
	if (starting) // If system is starting in editor, start at last saved editor position
	{
		INISetSection("Editor");
		int level = INIGetInt("Level", 0);
		S3DPoint pos;
		INIParse("Pos", "0 0 0", "%i %i %i", &pos.x, &pos.y, &pos.z);
		MapPane.SetMapLevel(level);
		MapPane.SetMapPos(pos);
	}
}

void ShutDownEditor()
{
	// Save INI stuff
	INISetSection("Editor");
	int level = MapPane.GetMapLevel();
	INISetInt("Level", level);
	S3DPoint pos;
	MapPane.GetMapPos(pos);
	INIPrint("Pos", "%d %d %d", pos.x, pos.y, pos.z);

	Console.Close();
	PlayScreen.RemovePane(&Console);

	ScriptEditor.Close();
	PlayScreen.RemovePane(&ScriptEditor);

	ScrollEditor.Close();
	PlayScreen.RemovePane(&ScrollEditor);

	StatusBar.Close();
	PlayScreen.RemovePane(&StatusBar);

	ToolBar.Close();
	PlayScreen.RemovePane(&ToolBar);

	ClassPane.Close();
	PlayScreen.RemovePane(&ClassPane);

	delete EditorData;

	Editor = FALSE;

	// Show game panes
	Inventory.Show();
	QuickSpells.Show();
	HealthBar.Show();
	StaminaBar.Show();
	TextBar.Show();
	MultiCtrl.Show();

	NoScrollZBuffer = TRUE;
	MapPane.RedrawAll();

	// Pause script processing
	TScript::ResumeAllScripts();

	// Recenter on lock (if he's in the game)
	if (Player)
		MapPane.CenterOnObj(Player, FALSE);
}

// Full-screen editing mode toggle
void StartFullScreen()
{
	Console.Hide();
	ToolBar.Hide();
	StatusBar.Hide();
	ClassPane.Hide();

	MapPane.Resize(0, 0, Display->Width(), Display->Height());

	Display->ClearBackgroundAreas();
}

void ShutDownFullScreen()
{
	Console.Show();
	ToolBar.Show();
	StatusBar.Show();
	ClassPane.Show();

	MapPane.Resize(FRAMEMAPPANEX, FRAMEMAPPANEY, FRAMEMAPPANEWIDTH, FRAMEMAPPANEHEIGHT);

	Display->ClearBackgroundAreas();
	PlayScreen.CreateBackgroundAreas();
}

#define TERRAIN_NOTHING		0
#define TERRAIN_GRASS		1
#define TERRAIN_DIRT		2
#define TERRAIN_MOUNTAIN	3
#define TERRAIN_LAKE		4
#define TERRAIN_RIVER		5
#define TERRAIN_OCEAN		6
#define TERRAIN_STREET		7

#define NUMTERRAINTYPES		8
#define TERRAINTYPESHIFT	4

#define OVERLAY_NOTHING		0
#define OVERLAY_PATH		1
#define OVERLAY_ROAD		2
#define OVERLAY_FORESTSTART	16
#define OVERLAY_FORESTEND	32

BYTE GetTerrainType(BYTE *map, int x, int y)
{
	if (x < 0 || x >= (MAXPLATESX-1) || y < 0 || y >= (MAXPLATESY-1))
		return (TERRAIN_OCEAN << TERRAINTYPESHIFT);

	return *(map + (y * MAXPLATESX) + x);
}

#define TERRAIN(dx, dy)		GetTerrainType(map, x + dx, y + dy)

int BaseTerrain(BYTE *map, int x, int y)
{
	int lowest = 1000000;

	for (int y0 = 0; y0 < 2; y0++)
		for (int x0 = 0; x0 < 2; x0++)
			if ((TERRAIN(x0, y0) & 0x0F) < lowest)
				lowest = TERRAIN(x0, y0) & 0x0F;

	return lowest;
}

int GetTileCode(BYTE *map, int x, int y)
{
	int base = BaseTerrain(map, x, y);

	long code = ((TERRAIN(0, 0) - base) << 24) | ((TERRAIN(1, 0) - base) << 16) |
				((TERRAIN(0, 1) - base)  << 8) | ((TERRAIN(1, 1) - base));

	return code;
}

#define WORLD_ZSTART		64			// the minimum z coord for generated objects

int zstart[NUMTERRAINTYPES] = { WORLD_ZSTART, 16 + WORLD_ZSTART, 16 + WORLD_ZSTART, 16 + WORLD_ZSTART,
								WORLD_ZSTART, WORLD_ZSTART - 8, 18 + WORLD_ZSTART, 16 + WORLD_ZSTART };

#define ELEVATIONCHANGE		46

int TerrainHeight(BYTE *map, int x, int y)
{
	int lowest = 1000000;
	int add;

	for (int y0 = 0; y0 < 2; y0++)
		for (int x0 = 0; x0 < 2; x0++)
			if ((TERRAIN(x0, y0) & 0x0F) < lowest)
			{
				lowest = TERRAIN(x0, y0) & 0x0F;
				add = zstart[TERRAIN(x0, y0) >> TERRAINTYPESHIFT];
			}
			else if ((TERRAIN(x0, y0) & 0x0F) == lowest)
			{
				add = min(add, zstart[TERRAIN(x0, y0) >> TERRAINTYPESHIFT]);
			}

	return (lowest * ELEVATIONCHANGE) + add;
}

struct { char ch; int code; } TerrainCodes[] = {
 { 'x', 0x00 }, { 'g', 0x10 }, { 'd', 0x20 }, { 'k', 0x11 },
 { 'i', 0x14 }, { 'm', 0x30 }, { 'r', 0x50 }, { 'o', 0x60 },
 { 0, 0 } // terminator
};

int TerrainToCode(char ch)
{
	if (ch >= 'A' && ch <= 'Z')
		ch += 'a' - 'A';

	for (int i = 0; TerrainCodes[i].ch != 0; i++)
		if (ch == TerrainCodes[i].ch)
			return TerrainCodes[i].code;

	return -1;
}

int FindSuperTile(BYTE *map, int x, int y, BYTE *filled, int *flux)
{
	if (flux)
		*flux = 0;

	for (int i = 0; i < TileClass.NumTypes(); i++)
	{
		if (TileClass.GetObjType(i) == NULL || !TileClass.GetStat(i, "supertile"))
			continue;

		int width = TileClass.GetStat(i, "width");
		int height = TileClass.GetStat(i, "height");
        char *name = TileClass.GetObjType(i)->name + 3;   // Added 3 for the "FOR" (forest) prefix

		if ((size_t)(width * height) != strlen(name))
		{
			Output("width*height != name length for supertile\n");
			continue;
		}

		int base = 1000;
		for (int y0 = 0; y0 < height; y0++)
			for (int x0 = 0; x0 < width; x0++)
				if (BaseTerrain(map, x+x0, y+y0) < base)
					base = BaseTerrain(map, x+x0, y+y0);

		BOOL qualifies = TRUE;
		for (int h = 0; h < height && qualifies; h++)
			for (int w = 0; w < width && qualifies; w++)
			{
				int code = TerrainToCode(*name++);
				if (code < 0)
				{
					Output("bad code character in supertile\n");
					continue;
				}

				if (code != (TERRAIN((w+1)/2, (h+1)/2) - base))
 					qualifies = FALSE;
			}

		if (qualifies)
		{
			for (int h = 0; h < height/2; h++)
				for (int w = 0; w < width/2; w++)
					*(filled+((x+w)*MAXPLATESY)+(y+h)) = 1;

			return i;
		}
	}

	return -1;
}

int overlay(BYTE *overmap, int x, int y)
{
	if (x < 0 || x >= MAXPLATESX || y < 0 || y >= MAXPLATESY)
		return OVERLAY_NOTHING;

	return *(overmap+(y*MAXPLATESX)+x);
}

#define OVERLAY(dx, dy) (overlay(overmap, x+(dx), y+(dy)))

int GetOverlayTile(BYTE *overmap, int x, int y, int code)
{
	if (OVERLAY(0, 0) == OVERLAY_PATH)
	{
		char pathname[80] = "path";

		if (code == 0x10101010)
		{
			if (OVERLAY(0, -1) == OVERLAY_PATH)
				strcat(pathname, "N");
			if (OVERLAY(1, 0) == OVERLAY_PATH)
				strcat(pathname, "E");
			if (OVERLAY(0, 1) == OVERLAY_PATH)
				strcat(pathname, "S");
			if (OVERLAY(-1, 0) == OVERLAY_PATH)
				strcat(pathname, "W");
		}
		else if (code == 0x20102010 && OVERLAY(1, 0) == OVERLAY_PATH)
			strcat(pathname, "BE");
		else if (code == 0x20201010 && OVERLAY(0, 1) == OVERLAY_PATH)
			strcat(pathname, "BS");
		else if (code == 0x10201020 && OVERLAY(-1, 0) == OVERLAY_PATH)
			strcat(pathname, "BW");
		else if (code == 0x10102020 && OVERLAY(0, -1) == OVERLAY_PATH)
			strcat(pathname, "BN");

		return TileClass.FindObjType(pathname);
	}

	return -1;
}

PTObjectInstance GenerateObj(PTSector sect, BYTE *map, BYTE *overmap, int type, int x, int y, int px, int py, int flux = 0)
{
	PTObjectInstance inst;
	SObjectDef def;

	memset(&def, 0, sizeof(SObjectDef));
	def.objclass = OBJCLASS_TILE;

	int tx = (x * PLATESPERSECTX) + px;
	int ty = (y * PLATESPERSECTY) + py;
	int tmp = GetOverlayTile(overmap, tx, ty, TileClass.GetStat(type, "Code"));
	if (tmp >= 0)
		type = tmp;

	def.objtype = type;
	def.flags = OF_GENERATED;
	def.pos.x = (x * SECTORWIDTH) + (px * PLATEWIDTH) + (PLATEWIDTH >> 1);
	def.pos.y = (y * SECTORHEIGHT) + (py * PLATEHEIGHT) + (PLATEHEIGHT >> 1);
	def.pos.z = TerrainHeight(map, (x * PLATESPERSECTX) + px, (y * PLATESPERSECTY) + py) - (flux * ELEVATIONCHANGE);
	def.level = sect->SectorLevel();

	inst = TObjectClass::GetClass(def.objclass)->NewObject(&def);
	if (!inst)
		return NULL;

	sect->AddObject(inst);

	int code = TileClass.GetStat(def.objtype, "Extra");

	if (code != 0xFFFFFFFF)
	{
		int nexttype = TileClass.FindRandStatVal(TileClass.FindStat("Code"), code);
		def.pos.z += (flux * ELEVATIONCHANGE);
		GenerateObj(sect, map, overmap, nexttype, x, y, px, py);
	}

	int over = overlay(overmap, tx, ty);
//	if (over >= OVERLAY_FORESTSTART && over < OVERLAY_FORESTEND)
		TerrainTemplates->ApplyTemplate(inst, sect, random(0, 10));//over - OVERLAY_FORESTSTART + 1);

	return inst;
}

#define QUAD(c, q)	(((c) >> ((q) * 8)) & 0xFF)

void GenerateMap(int startx, int starty, int sizex, int sizey)
{
	TObjectImagery::PauseLoader(); // NOTE: if resume isn't called, program will lock

	PTSector sect;
	BYTE filled[MAXPLATESX][MAXPLATESY];
	int x, y, px, py;
	FILE *fp;

	memset(filled, 0, MAXPLATESX*MAXPLATESY);

	if ((fp = popen("map.bmp", "r")) == NULL)
	{
		TObjectImagery::ResumeLoader();
		return;
	}

	BYTE *map = new BYTE[MAXPLATESX*MAXPLATESY];
	fread(map, 14, 1, fp);
	x = *((int *)(map + 10));
	fseek(fp, x, SEEK_SET);
	// fuckin' upsidedown bitmaps...
	for (y = MAXPLATESY-1; y >= 0; y--)
		fread(map+(y*MAXPLATESX), MAXPLATESX, 1, fp);
	fclose(fp);

	if ((fp = popen("overmap.bmp", "r")) == NULL)
	{
		TObjectImagery::ResumeLoader();
		return;
	}

	BYTE *overmap = new BYTE[MAXPLATESX*MAXPLATESY];
	fread(overmap, 14, 1, fp);
	x = *((int *)(overmap + 10));
	fseek(fp, x, SEEK_SET);
	for (y = MAXPLATESY-1; y >= 0; y--)
		fread(overmap+(y*MAXPLATESX), MAXPLATESX, 1, fp);
	fclose(fp);

	MapPane.FreeAllSectors();

	for (int sy = starty; sy < (starty + sizey); sy++)
		for (int sx = startx; sx < (startx + sizex); sx++)
		{
			sect = new TSector(0, sx, sy);
			sect->Load();

			for (TObjectIterator i(sect->ObjectArray()); i; i++)
				if (i.Item() && i.Item()->GetFlags() & OF_GENERATED)
					sect->ObjectArray()->Remove(i);

			for (py = 0; py < PLATESPERSECTY; py++)
				for (px = 0; px < PLATESPERSECTX; px++)
				{
					x = (sx * PLATESPERSECTX) + px;
					y = (sy * PLATESPERSECTY) + py;

					if (filled[x][y] != 0)
						continue;

					int type, flux = 0;
					if ((type = FindSuperTile(map, x, y, &(filled[0][0]), &flux)) < 0)
					{
						DWORD code = GetTileCode(map, x, y);

						// leave out the 'center' of the mountains
						if ((code & 0xF0F0F0F0) == 0x30303030)
							continue;

						// use solid street tile for anything containing street
						for (int i = 0; i < 4; i++)
							if (QUAD(code, i) == 0x70)
								code = 0x70707070;


						if ((type = TileClass.FindRandStatVal(TileClass.FindStat("Code"), code, NULL)) < 0 &&
							(type = TileClass.FindRandStatVal(TileClass.FindStat("Code"), code, &flux)) < 0)
							continue;

						PTObjectInstance inst = GenerateObj(sect, map, overmap, type, sx, sy, px, py, flux);

						if (!inst)
							continue;

						DWORD oldcode = TileClass.GetStat(inst->ObjType(), "Code");

						// build a mask based on the wildcards in the code
						DWORD newcode = 0;
						if (!QUAD(oldcode, 3))
							newcode |= code & 0xFF000000;
						if (!QUAD(oldcode, 2))
							newcode |= code & 0xFF0000;
						if (!QUAD(oldcode, 1))
							newcode |= code & 0xFF00;
						if (!QUAD(oldcode, 0))
							newcode |= code & 0xFF;

						if (newcode != 0)
						{
							if (!QUAD(newcode, 0))
								if (QUAD(newcode, 1))
									newcode |= QUAD(newcode, 1);
								else if (QUAD(newcode, 2))
									newcode |= QUAD(newcode, 2);
								else
									newcode |= QUAD(newcode, 3);

							if (!QUAD(newcode, 1))
								if (QUAD(newcode, 0))
									newcode |= QUAD(newcode, 0) << 8;
								else if (QUAD(newcode, 3))
									newcode |= QUAD(newcode, 3) << 8;
								else
									newcode |= QUAD(newcode, 2) << 8;

							if (!QUAD(newcode, 2))
								if (QUAD(newcode, 0))
									newcode |= QUAD(newcode, 0) << 16;
								else if (QUAD(newcode, 3))
									newcode |= QUAD(newcode, 3) << 16;
								else
									newcode |= QUAD(newcode, 1) << 16;

							if (!QUAD(newcode, 3))
								if (QUAD(newcode, 1))
									newcode |= QUAD(newcode, 1) << 24;
								else if (QUAD(newcode, 2))
									newcode |= QUAD(newcode, 2) << 24;
								else
									newcode |= QUAD(newcode, 0) << 24;

							flux = 1000;
							for (int i = 0; i < 4; i++)
								if ((QUAD(newcode, i) & 0x0F) < (DWORD)flux)
									flux = QUAD(newcode, i) & 0x0F;

							for (i = 0; i < 4; i++)
								newcode -= flux << (i * 8);

							if ((type = TileClass.FindRandStatVal(TileClass.FindStat("Code"), newcode)) < 0)
								continue;

							flux *= -1;
						}
						else
							continue;
					}

					GenerateObj(sect, map, overmap, type, sx, sy, px, py, flux);
				}

			delete sect;
		}

	delete map;
	delete overmap;

	TObjectImagery::ResumeLoader();

	MapPane.ReloadSectors();
}

// *************
// * TTextPane *
// *************

#define TEXTPANE_LINESPACE 2

BOOL TTextPane::Initialize()
{
	textlen   = 0;
	curstartx = 0;
	curstarty = 0;
	windowx = windowy = 0;
	cursorx = cursory = 0;
	curwidth = winwidth = 60;
	curheight = winheight = GetHeight() / (SystemFont->height + TEXTPANE_LINESPACE);
	wrapwidth = -1;

	SetDirty(TRUE);
	offset = curoffset = 0;
	text = new char[TEXTBUFSIZE];
	text[0] = CURSOR;					// set cursor to start
	text[1] = 0;						// null terminate the buffer
	memset(text, 0, TEXTBUFSIZE);

	return TRUE;
}

void TTextPane::Close()
{
	delete text;
}

void TTextPane::DrawBackground()
{
	if (IsDirty())
	{
		Display->Box(0, 0, GetWidth(), GetHeight(), 0, 0xffff, 0, DM_BACKGROUND);
		Display->WriteText(text+offset, 0, 0, winheight, SystemFont, NULL, DM_USEDEFAULT, 
			wrapwidth, 0, JUSTIFY_LEFT, -1, TEXTPANE_LINESPACE);
		SetDirty(FALSE);
	}
}

// a couple of utility functions for the text processing
inline isvalid(int key)
{
	if (((key >= ' ' && key <= '~') || key == '\n') && key != '`' && !CtrlDown && !AltDown)
		return TRUE;

	return FALSE;
}

int endofline(char *text)
{
	int x, count;

	for (x = 0, count = 0; text[x] && text[x] != '\n'; x++)
		if (text[x] != CURSOR)
			count++;

	return count;
}

BOOL TTextPane::InsertText(char *newtext)
{
	textlen = strlen(text);
	int newlen = strlen(newtext);

	if (newlen == 0)
		return TRUE;

	if ((textlen + newlen + 1) >= TEXTBUFSIZE)
		return FALSE;

	// bump old text forwards (+1 for cursor character)
	memmove(text + curoffset + 1 + newlen, text + curoffset + 1, textlen - curoffset);

	// copy in new text
	memmove(text + curoffset, newtext, newlen);

	// set cursor to the end of new text
	curoffset += newlen;
	text[curoffset] = CURSOR;

	// compute new x and y values
	char *ptr = newtext;
	while ((ptr = strchr(ptr, '\n')))
	{
		cursorx = 0;
		cursory++;
		newtext = ++ptr;
	}

	cursorx += strlen(newtext);

	textlen += newlen;

	UpdateWindow();
	Update();

	return TRUE;
}

BOOL TTextPane::RemoveText(int numchars, BOOL forward)
{
	textlen = strlen(text);

	if (!forward && numchars > (cursorx - curstartx))
		numchars = cursorx - curstartx;

	if (numchars < 0)
		return FALSE;

	if ((textlen - numchars) < 1)			// always at least one char (the cursor)
		numchars = textlen - 1;

	if (forward)
		memmove(text + curoffset + 1, text + curoffset + 1 + numchars, textlen - curoffset - numchars + 1);
	else
	{
		memmove(text + curoffset - numchars, text + curoffset, textlen - curoffset + 1);
		curoffset -= numchars;
		cursorx -= numchars;
	}


	return TRUE;
}

char *TTextPane::GetText(int start, int len, char *buf)
{
	textlen = strlen(text);

	if (start >= textlen)
		return NULL;

	strncpy(buf, text + start, len);
	buf[len] = NULL;

	return buf;
}

void TTextPane::UpdateWindow()
{
	while (cursory < windowy)
	{
		while (offset > 0 && text[--offset] != '\n')
			;

		while (offset > 0 && text[offset - 1] != '\n')
			offset--;

		windowy--;
	}

	while (cursory >= windowy + winheight)
	{
		offset += endofline(text + offset);
		if (text[offset])
			offset++;
		windowy++;
	}
}

BOOL TTextPane::SetCursor(int nx, int ny, BOOL ignorebounds)
{
	// do some quick bounds checking
	if (!ignorebounds)
	{
		if (nx < curstartx)
			nx = curstartx;
#if 0
		else if (nx >= curstartx + curwidth)
			nx = curstartx + curwidth - 1;
#endif

		if (ny < curstarty)
			ny = curstarty;
		else if (ny >= curstarty + curheight)
			ny = curstarty + curheight - 1;
	}

	// update needed?
	if (nx == cursorx && ny == cursory)
		return FALSE;

	// remove old cursor
	memmove(text + curoffset, text + curoffset + 1, strlen(text + curoffset));

	// compute new cursor position and update curoffset
	curoffset = 0;
	int x, y;

	for (y = 0; y < ny; y++, x = 0)
	{
		for ( ; text[curoffset] != '\n'; curoffset++, x++)
			if (!text[curoffset])
			{
				// uh oh...new cursor position is invalid
				ny = y;
				nx = x;
				goto insertcursor;		// get ALL the way out
			}
		curoffset++;
	}

	for (x = 0; x < nx; x++, curoffset++)
		if (!text[curoffset] || text[curoffset] == '\n')
		{
			// invalid x position
			nx = x;
			break;
		}

  insertcursor:
	memmove(text + curoffset + 1, text + curoffset, strlen(text) - curoffset + 1);
	text[curoffset] = CURSOR;

	// adjust in case curoffset is less than offset (cursor character was inserted)
	if (curoffset < offset)
		offset++;
	cursorx = nx;
	cursory = ny;
	UpdateWindow();

	return TRUE;
}

void TTextPane::KeyPress(int key, BOOL down)
{
	SObjectDef def;

	TPane::KeyPress(key, down);

	if (down)
	{
		switch (key)
		{
		  case VK_BACK:
			SetDirty(RemoveText(1, FALSE));		// remove 1 char backwards
			break;
		  case VK_DELETE:
			SetDirty(RemoveText(1, TRUE));		// remove 1 char forwards
			break;
		  case VK_UP:
			SetDirty(SetCursor(cursorx, cursory - 1));
			break;
		  case VK_DOWN:
			SetDirty(SetCursor(cursorx, cursory + 1));
			break;
		  case VK_LEFT:
			SetDirty(SetCursor(CtrlDown ? cursorx - 6 : cursorx - 1, cursory));
			break;
		  case VK_RIGHT:
			SetDirty(SetCursor(CtrlDown ? cursorx + 6 : cursorx + 1, cursory));
			break;
		  case VK_HOME:
			SetDirty(SetCursor(0, cursory));
			break;
		  case VK_END:
			SetDirty(SetCursor(cursorx + endofline(text+curoffset), cursory));
			break;
		  case VK_PRIOR:
			SetDirty(SetCursor(cursorx, cursory - winheight));
			break;
		  case VK_NEXT:
			SetDirty(SetCursor(cursorx, cursory + winheight));
			break;
		  case VK_TAB:
			//SetDirty(Input("  "));
			break;
		  case 'Y':
		  case 'y':
			if (CtrlDown)
			{
				int tmpx = cursorx;
				SetCursor(0, cursory);
				RemoveText(endofline(text+curoffset) + 1, TRUE);
				SetCursor(tmpx, cursory);
				SetDirty(TRUE);
			}
			break;
		}
	}
}

void TTextPane::CharPress(int key, BOOL down)
{
	TPane::CharPress(key, down);

	if (down)
	{
		if (key == '\r')
			key = '\n';		// Turn a return into a linefeed

		char keybuf[2];
		keybuf[0] = key;
		keybuf[1] = 0;

		if (isvalid(key))
			SetDirty(Input(keybuf));
	}
}

// ****************
// * TConsolePane *
// ****************

BOOL TConsolePane::Initialize()
{
	TTextPane::Initialize();

	curstartx = strlen(PROMPT);
	curheight = 1;
	head = tail = chained = NULL;

	box = axis = NULL;

	wrapwidth = GetWidth();

	// Create the kill event
	if ((cmdevents[KILL_EVENT] = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
		FatalError("Could not create kill event for command processor!");

	// Create the character available event
	if ((cmdevents[CHAR_AVAILABLE_EVENT] = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
		FatalError("Could not create character event for command processor!");

	// Begin the Command Processor thread
	unsigned threadid; // Dummy id handle
	cmdthreadhandle = (HANDLE)_beginthreadex(
		NULL, 0,
		TConsolePane::CommandThread,
		(void *)this, TRUE,
        &threadid );

	SetThreadPriority(cmdthreadhandle, THREAD_PRIORITY_ABOVE_NORMAL);

	return TRUE;
}

void TConsolePane::Close()
{
	// Trigger the Kill event and wait for the command processor thread to close
	SetEvent(cmdevents[KILL_EVENT]);
	WaitForSingleObject(cmdthreadhandle, INFINITE);

	// Close the events
	CloseHandle(cmdevents[CHAR_AVAILABLE_EVENT]);
	CloseHandle(cmdevents[KILL_EVENT]);
}

void TConsolePane::SetupMouseBitmap(int xspan, int yspan)
{
	if (xspan && yspan)
		SetMouseBitmap(PointerCursor);

	if (xspan && yspan == 0)
		SetMouseBitmap(EditorData->Bitmap("lrarrow"));

	if (yspan && xspan == 0)
		SetMouseBitmap(EditorData->Bitmap("udarrow"));
}

void TConsolePane::ChainMouse(void (*cfunc)(int, int), int x0, int x1, int xcur,
													   int y0, int y1, int ycur)
{
	// max sure values are in the right order
	if (x1 < x0)
	{
		int tmp = x0;
		x0 = x1;
		x1 = tmp;
	}
	if (y1 < y0)
	{
		int tmp = y0;
		y0 = y1;
		y1 = tmp;
	}

	// set up the vars
	PSChained c = new SChained;
	c->func = cfunc;
	c->xmin = x0;
	c->xspan = x1 - x0;
	c->ymin = y0;
	c->yspan = y1 - y0;
	c->oldxval = xcur;
	c->oldyval = ycur;

	// add to the end of the list
	c->prev = tail;
	c->next = NULL;

	if (tail)
		tail->next = c;
	else
	{
		head = chained = c;
		oldbuflen = 0;
		oldcx = x0 + 1;
		oldcy = y0 + 1;

		SetupMouseBitmap(c->xspan, c->yspan);
		PlayScreen.SetExclusivePane(&Console);
	}

	tail = c;
}

void TConsolePane::SetBounds()
{
	PTObjectInstance inst = MapPane.GetInstance(StatusBar.GetSelectedObj());
	if (!inst)
		return;

	SObjectDef def;
	memset(&def, 0, sizeof(SObjectDef));
	def.objclass = OBJCLASS_HELPER;
	def.objtype = HelperClass.FindObjType("box");
	inst->GetPos(def.pos);

	if (def.objtype < 0)
		return;

	box = MapPane.GetInstance(MapPane.NewObject(&def));
	if (!box)
		return;

	int w, l, h;
	inst->GetImagery()->GetWorldBoundBox(inst->GetState(), w, l, h);
	box->GetImagery()->SetWorldBoundBox(box->GetState(), w << WALKMAPSHIFT, l << WALKMAPSHIFT, h << WALKMAPSHIFT);

	int x = inst->GetImagery()->GetWorldRegX(inst->GetState());
	int y = inst->GetImagery()->GetWorldRegY(inst->GetState());
	int z = inst->GetImagery()->GetWorldRegZ(inst->GetState());
	box->GetImagery()->SetWorldReg(box->GetState(), x << WALKMAPSHIFT, y << WALKMAPSHIFT, z << WALKMAPSHIFT);

	PlayScreen.SetExclusivePane(this);

	sprintf(buf, "offset: (%d, %d, %d)  size: (%d, %d, %d)", x, y, z, w, l, h);
	Output(buf);
}

BOOL TConsolePane::AddAxis(PTObjectInstance inst)
{
	SObjectDef def;
	memset(&def, 0, sizeof(SObjectDef));
	def.objclass = OBJCLASS_HELPER;
	def.objtype = HelperClass.FindObjType("axis");
	inst->GetPos(def.pos);
	def.level = MapPane.GetMapLevel();

	if (def.objtype < 0)
		return FALSE;

	axis = MapPane.GetInstance(MapPane.NewObject(&def));

	return (axis != NULL);
}

void TConsolePane::ClearAxis()
{
	if (axis)
	{
		PTObjectInstance oi = MapPane.GetInstance(axis->GetMapIndex());
		if (oi) MapPane.DeleteObject(oi);
		axis = NULL;
	}
}

void TConsolePane::KeyPress(int key, BOOL down)
{
	TTextPane::KeyPress(key, down);

	if (box && down)
	{
		PTObjectInstance inst;

		int width, length, height;
		box->GetImagery()->GetWorldBoundBox(box->GetState(), width, length, height);

		int x = box->GetImagery()->GetWorldRegX(box->GetState());
		int y = box->GetImagery()->GetWorldRegY(box->GetState());
		int z = box->GetImagery()->GetWorldRegZ(box->GetState());

		int w, l, h;
		w = l = h = 0;

		switch (key)
		{
		  case VK_UP:
			l -= 1;
			break;
		  case VK_PRIOR:
			l -= 1;
			w += 1;
			break;
		  case VK_RIGHT:
			w += 1;
			break;
		  case VK_NEXT:
			l += 1;
			w += 1;
			break;
		  case VK_DOWN:
			l += 1;
			break;
		  case VK_END:
			l += 1;
			w -= 1;
			break;
		  case VK_LEFT:
			w -= 1;
			break;
		  case VK_HOME:
			l -= 1;
			w -= 1;
			break;
		  case VK_INSERT:
			h += 1;
			break;
		  case VK_DELETE:
			h -= 1;
			break;

		  case VK_ESCAPE:
			if (box)
			{
				if (box->GetImagery())
				{
					box->GetImagery()->SetWorldReg(box->GetState(), 0, 0, 0);
					box->GetImagery()->SetWorldBoundBox(box->GetState(), 0, 0, 0);
					box->GetImagery()->SetHeaderDirty(FALSE);
				}

				PTObjectInstance oi = MapPane.GetInstance(box->GetMapIndex());
				if (oi) MapPane.DeleteObject(oi);
				box = NULL;
			}

			inst = MapPane.GetInstance(StatusBar.GetSelectedObj());
			if (inst)
			{
				inst->GetImagery()->SetWorldReg(inst->GetState(), x >> WALKMAPSHIFT, y >> WALKMAPSHIFT, z >> WALKMAPSHIFT);
				inst->GetImagery()->SetWorldBoundBox(inst->GetState(), width >> WALKMAPSHIFT, length >> WALKMAPSHIFT, height >> WALKMAPSHIFT);
			}

			PlayScreen.ReleaseExclusivePane(this);

			Output("\n");
			Output(PROMPT);

			return;

		  default:
			return;
		}

		l *= (1 << WALKMAPSHIFT);
		w *= (1 << WALKMAPSHIFT);
		h *= (1 << WALKMAPSHIFT);

		if (CtrlDown)
		{
			x = max(0, x - w);
			y = max(0, y - l);
			z = max(0, z - h);
			box->GetImagery()->SetWorldReg(box->GetState(), x, y, z);
		}
		else
		{
			width  = max(0, width + w);
			length = max(0, length + l);
			height = max(0, height + h);
			box->GetImagery()->SetWorldBoundBox(box->GetState(), width, length, height);
		}

		sprintf(buf, "offset: (%d, %d, %d)  size: (%d, %d, %d)", x >> WALKMAPSHIFT, y >> WALKMAPSHIFT, z >> WALKMAPSHIFT, width >> WALKMAPSHIFT, length >> WALKMAPSHIFT, height >> WALKMAPSHIFT);
		SetCursor(0, cursory);
		RemoveText(endofline(text+curoffset) + 1, TRUE);
		Output(buf);
	}
}

void TConsolePane::CharPress(int key, BOOL down)
{
	TTextPane::CharPress(key, down);

	// Flag the command processor thread that a key has been typed
	// Unless the key is a Return (Returns are signaled in Input() which is called
	// by the TTextPane::CharPress())
	if (key != '\r')
	{
		cmdchar = key;
		SetEvent(cmdevents[CHAR_AVAILABLE_EVENT]);
	}
}

void TConsolePane::MouseMove(int button, int x, int y)
{
	if (chained)
	{
		cx = ((cursorx * chained->xspan) / Display->Width()) + chained->xmin;
		cy = ((cursory * chained->yspan) / Display->Height()) + chained->ymin;
	}
}

void TConsolePane::MouseClick(int button, int x, int y)
{
	if (chained)
	{
		if (button == MB_LEFTUP)
		{
			chained = chained->next;
			if (chained)
			{
				oldbuflen = 0;
				oldcx = chained->xspan + chained->xmin + 1;
				oldcy = chained->yspan + chained->ymin + 1;
				Console.Output(" ");
				SetupMouseBitmap(chained->xspan, chained->yspan);
				return;
			}
			else
			{
				// list traversal complete, nuke the list
				while (head)
				{
					PSChained tmp = head->next;
					delete head;
					head = tmp;
				}
				chained = head = tail = NULL;
				Console.Input("\n");			// send the command
			}
		}
		else if (button == MB_RIGHTUP)
		{
			// reset to default values and nuke the list
			while (head)
			{
				PSChained tmp = head->next;
				(*(head->func))(head->oldxval, head->oldyval);
				delete head;
				head = tmp;
			}
			Console.Output("\n");
			Console.Output(PROMPT);
		}
		else
			return;

		chained = head = tail = NULL;
		SetMouseBitmap(PointerCursor);
		PlayScreen.ReleaseExclusivePane(&Console);
		StatusBar.StopMoving();
	}
}

void TConsolePane::DrawBackground()
{
	TTextPane::DrawBackground();

	if (chained && (cx != oldcx || cy != oldcy))
	{
		RemoveText(oldbuflen, FALSE);

		if (chained->xspan)
		{
			if (chained->yspan)
				sprintf(buf, "(%d, %d)", cx, cy);
			else
				itoa(cx, buf, 10);

			(*(chained->func))(cx, cy);
		}
		else
		{
			itoa(cy, buf, 10);

			(*(chained->func))(cy, cx);
		}

		InsertText(buf);

		oldbuflen = strlen(buf);
		oldcx = cx;
		oldcy = cy;
	}
}

BOOL TConsolePane::Input(char *string)
{
	char tmpbuf[256];
	char *ptr = string;

	// check for paging
	if (strlen(text)+strlen(string) >= TEXTBUFSIZE)
	{/*
		int newoffset = offset - (TEXTBUFSIZE / 4);
		while (text[newoffset++] != '\n')
			if (newoffset >= offset)
			{
				newoffset = offset - (TEXTBUFSIZE / 4);
				break;
			}

		memmove(text, text + (TEXTBUFSIZE / 4), newoffset);
		memset(text + (TEXTLEN - newoffset), 0, newoffset);
		curoffset -= newoffset - offset;
		offset = newoffset;*/
	}

	// console needs to process each line individually
	while ((ptr = strchr(ptr, '\n')))
	{
		char *textoff = string;
		for (int i = 0; *textoff && *textoff != '\n' && i < 255; textoff++, i++)
			tmpbuf[i] = *textoff;
		tmpbuf[i] = 0;

		SetCursor(cursorx + endofline(text + curoffset), cursory, TRUE);
		InsertText(tmpbuf);
		InsertText("\n");

		// find the current input line in the buffer
		SetCursor(curstartx, cursory - 1, TRUE);
		textoff = text + curoffset + 1;

		SetCursor(0, cursory + 1, TRUE);
		curstarty++;

		if (*ptr)
			ptr++;
		string = ptr;

		// Flag the command processor thread that a line has been entered
		cmdchar = '\r';
		SetEvent(cmdevents[CHAR_AVAILABLE_EVENT]); // This causes the command thread to run

	  // Command processed, now reset the command line pointer to end of buffer
		cmdline = text + curoffset;
	}

	InsertText(string);

	return TRUE;
}

BOOL TConsolePane::Output(char *string)
{
	char *ptr = string;

	while ((ptr = strchr(ptr, '\n')))
	{
		curstarty++;
		ptr++;
	}

	BOOL dirty = InsertText(string);

  // Sending text to output resets beginning of current command line
	cmdline = text + curoffset;

	return dirty;
}

// This cannot be called from the main thread
int TConsolePane::GetChar()
{
	DWORD wait;

	wait = WaitForMultipleObjects(2, cmdevents, FALSE, INFINITE);

	if (wait != WAIT_OBJECT_0 + 1)
		return -1;

	ResetEvent(cmdevents[CHAR_AVAILABLE_EVENT]);

	return cmdchar;
}

// This cannot be called from the main thread
BOOL TConsolePane::GetLine(char *buffer, int buffersize)
{
	int key;

	// Wait until they enter a character
	while((key = GetChar()) != -1)
	{
		// See if they just pressed return to end a line
		if (key == '\r')
		{
			char *textoff = cmdline;
			if (!textoff)
				continue;

			if (strchr(textoff, '\n') != NULL)
			{
				// Copy the line to the buffer passed in
				for (int i = 0; *textoff && *textoff != '\n' && i < buffersize - 1; textoff++, i++)
					buffer[i] = *textoff;
				buffer[i] = 0;

				return TRUE;
			}
		}

	}

	return FALSE;
}

unsigned _stdcall TConsolePane::CommandThread(void *arg)
{
	PTConsolePane console = (PTConsolePane)arg;
	char tmpbuf[256];

	while (console->GetLine(tmpbuf, 256))
	{
	   	TStringParseStream s(tmpbuf, strlen(tmpbuf));
		TToken t(s);
		DWORD pos = s.GetPos();
		t.Get();

		if (StatusBar.GetSelectedObj() < 0)
			CommandInterpreter(NULL, t, MINCMDABREV);
		else
		{
			int list[MAXSELECTEDOBJS];
			int j = 0;

			// make a copy in case one of the commands corrupts the list
			for (int i = StatusBar.GetFirstObj(); i >= 0; i = StatusBar.GetNextObj())
				list[j++] = i;

			for (int n = 0; n < j; n++)
			{
				PTObjectInstance inst = MapPane.GetInstance(list[n]);
				if (inst)
					CommandInterpreter(inst, t, MINCMDABREV);

				s.SetPos(pos);
				t.Get();
			}
		}

		StatusBar.Validate();
		StatusBar.SetDirty(TRUE);

		if (console->cursorx != 0)				// make sure we're on a new line
			console->Output("\n");
		console->Output(PROMPT);
	}

	_endthreadex(0);

	return 0;
}

// ***************
// * TScriptPane *
// ***************

BOOL TScriptPane::Initialize()
{
	TTextPane::Initialize();
	inst = NULL;
	return TRUE;
}

void TScriptPane::LoadScript(PTObjectInstance oi)
{
	inst = oi;
	strcpy(text, ScriptManager.ObjectScript(oi)->Text());
	cursorx = windowx = cursory = windowy = offset = curoffset = 0;
}

void TScriptPane::SaveScript()
{
	if (inst && inst->GetScript())
	{
		// get rid of the cursor
		for (char *ptr = text; *ptr; )
			if (*ptr == CURSOR)
				memmove(ptr, ptr+1, strlen(ptr+1)+1);
			else
				ptr++;

		inst->GetScript()->SetText(text);
	}
}

void TScriptPane::KeyPress(int key, BOOL down)
{
	TTextPane::KeyPress(key, down);

	if (down)
	{
		switch (key)
		{
		  case VK_ESCAPE:
			SaveScript();
			PlayScreen.ReleaseExclusivePane(this);
			this->Hide();
			Console.Show();
			break;
		}
	}
}

// *********************
// * TScrollEditorPane *
// *********************

BOOL TScrollEditorPane::Initialize()
{
	TTextPane::Initialize();
	scroll = NULL;
	wrapwidth = GetWidth();
	return TRUE;
}

void TScrollEditorPane::SetScroll(PTObjectInstance s)
{
	scroll = (PTScroll)s;

	cursorx = windowx = cursory = windowy = offset = curoffset = 0;

	if (scroll && scroll->GetText())
		InsertText(scroll->GetText());
}

void TScrollEditorPane::SaveText()
{
	if (scroll)
	{
		// get rid of the cursor
		for (char *ptr = text; *ptr; )
			if (*ptr == CURSOR)
				memmove(ptr, ptr+1, strlen(ptr+1)+1);
			else
				ptr++;

		scroll->SetText(text);
	}
}

void TScrollEditorPane::KeyPress(int key, BOOL down)
{
	TTextPane::KeyPress(key, down);

	if (down)
	{
		switch (key)
		{
		  case VK_ESCAPE:
			SaveText();
			PlayScreen.ReleaseExclusivePane(this);
			this->Hide();
			Console.Show();
			break;
		}
	}
}

// ***************************
// * Various Editor Commands *
// ***************************

COMMAND(CmdSelect)
{
	if (t.Is("next"))
	{
		if (!StatusBar.Next())
			Output("Already at end of selection list.\n");
		return 0;
	}
	else if (t.Is("prev"))
	{
		if (!StatusBar.Prev())
			Output("Already at start of selection list.\n");
		return 0;
	}
	else if (t.Is("all"))
	{
		for (TMapIterator i; i; i++)
		{
			if (!StatusBar.Select(i.Item()->GetMapIndex(), TRUE))
			{
				Output("Selection overflowed, max objects selected.\n");
				break;
			}
		}

		return 0;
	}

	if (t.Type() == TKN_NUMBER)
	{
		// make the index number specified the current object
		int index;
		if (!Parse(t, "%d", &index))
			return CMD_BADPARAMS;

		if (!StatusBar.SetCurObj(index))
			Output("Invalid selection index.\n");

		return 0;
	}

	PTObjectInstance inst = MapPane.FindClosestObject(t.Text(), NULL, TRUE);

	if (!inst)
		Output("Can't find any object by that name.\n");
	else if (!StatusBar.Select(inst->GetMapIndex()))
		Output("Unable to select any more objects.\n");

	t.WhiteGet();
	return 0;
}

COMMAND(CmdDeselect)
{
	if (t.Is("all"))
		while (StatusBar.Deselect());
	else
		if (!StatusBar.Deselect())
			Output("Nothing to deselect.\n");

	t.WhiteGet();
	return 0;
}

COMMAND(CmdAdd)
{
	int objtype = -1;
	int number = 1;

	S3DPoint pos;
	MapPane.GetMapPos(pos);
	if (t.Type() == TKN_NUMBER)
	{
		if (!Parse(t, "%i %i %i", &pos.x, &pos.y, &pos.z))
			return CMD_BADPARAMS;
	}

	if (t.Type() == TKN_NUMBER)
	{
		number = t.Index();
		t.WhiteGet();
	}

	if (t.Is("next"))
	{
		S3DPoint zero;
		memset(&zero, 0, sizeof(S3DPoint));
		ClassPane.SelObjType(ClassPane.GetObjType() + 1);
		ClassPane.PutObject(zero);
		t.WhiteGet();
		return 0;
	}
	else if (t.Is("prev"))
	{
		S3DPoint zero;
		memset(&zero, 0, sizeof(S3DPoint));
		ClassPane.SelObjType(ClassPane.GetObjType() - 1);
		ClassPane.PutObject(zero);
		t.WhiteGet();
		return 0;
	}
	else if (t.Is("light", 1))
	{
		SObjectDef def;
		int intensity = 220, mult = 12;
		memset(&def, 0, sizeof(SObjectDef));

		def.objclass = OBJCLASS_TILE;
		def.objtype = TileClass.FindObjType("light");
        def.flags = OF_LIGHT | OF_EDITOR;

		def.pos = pos;
		def.pos.z = 60;
		def.level = MapPane.GetMapLevel();

		//t.WhiteGet();
		//if (!Parse(t, "<%d> <%d>", &intensity, &mult))
		//	return CMD_BADPARAMS;

		int index = MapPane.NewObject(&def);
		if (index < 0)
			Output("ERROR: Unable to add light\n");
		else
		{
			SColor color = { 255, 255, 255 };
			MapPane.GetInstance(index)->SetLightIntensity(intensity);
			MapPane.GetInstance(index)->SetLightColor(color);
			MapPane.GetInstance(index)->SetLightMultiplier(mult);

			S3DPoint lightpos(0, 0, 0);
			MapPane.GetInstance(index)->SetLightPos(lightpos);
			StatusBar.Select(index);
		}
	}
	else
	{
		PTObjectClass cl = TObjectClass::GetClass(TObjectClass::FindClass(t.Text()));

		if (cl)
		{
			t.WhiteGet();

			if (t.Is("type"))
			{
				t.WhiteGet();

				// Add a new type to this class
				char buf2[80];
				if (!Parse(t, "%s %s", buf, buf2))  // name, filename
					return CMD_BADPARAMS;

				objtype = cl->AddType(buf, buf2);
				if (objtype < 0)
					Output("ERROR: Adding object type\n");
				else
				{
					Output("New type %s added to class %s.\n", buf, cl->ClassName());
				}
			}
			else if (t.Is("stat"))
			{
				t.WhiteGet();

				if (!cl->ParseNewStat(t))
					return CMD_BADPARAMS;

				Output("Stat added.");
			}
			else if (t.Is("objstat"))
			{
				t.WhiteGet();

				if (!cl->ParseNewObjStat(t))
					return CMD_BADPARAMS;

				Output("Object stat added.");
			}
			else
			{
				// Add a new instance of a given object type in the map pane
				objtype = cl->FindObjType(t.Text(), FALSE);
				if (objtype < 0)
					objtype = cl->FindObjType(t.Text(), TRUE);

				if (objtype < 0)
				{
					Output("%s has no object named '%s'.\n", cl->ClassName(), t.Text());
				}
				else
				{
				    addobject:

					SObjectDef def;
					memset(&def, 0, sizeof(SObjectDef));

					def.objclass = cl->ClassId();
					def.objtype = objtype;
                    def.flags = 0;
					def.level = MapPane.GetMapLevel();

					def.pos = pos;

					int index = MapPane.NewObject(&def);
					if (index < 0)
						Output("ERROR: Creating object\n");
					else
					{
						StatusBar.Select(index);

						Output("%s:%s added at (%d, %d, %d).\n", cl->ClassName(),
							cl->GetObjType(objtype)->name, def.pos.x, def.pos.y, def.pos.z);

						if (number != 1)
							MapPane.GetInstance(index)->SetAmount(number);

						// If we added a player, add him to the player manager
						if (def.objclass == OBJCLASS_PLAYER)
						{
							PTObjectInstance oi = MapPane.GetInstance(index);
							if (oi)
							{
								PlayerManager.AddPlayer((PTPlayer)oi);
								PlayerManager.SetMainPlayer((PTPlayer)oi);
							}
						}
					}
				}

				t.WhiteGet();
			}
		}
		else
		{
			for (int i = 0; i < MAXOBJECTCLASSES; i++)
			{
				cl = TObjectClass::GetClass(i);
				if (cl && (objtype = cl->FindObjType(t.Text())) >= 0)
					break;
			}

			if (objtype >= 0)
				goto addobject;

			Output("No class named '%s'.\n", t.Text());
		}
	}

	return 0;
}

COMMAND(CmdAddInv)
{
	int objtype = -1;
	int number = 1;

	if (t.Type() == TKN_NUMBER)
	{
		number = t.Index();
		t.WhiteGet();
	}
	
	if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
		return CMD_BADPARAMS;

	if (!context->AddToInventory(t.Text(), number))
	{
		Output("Unable to add %s", t.Text());
		t.SkipLine();
		return 0;
	}

	t.WhiteGet();

	return 0;
}

COMMAND(CmdDelInv)
{
	int number = 1;

	if (t.Type() == TKN_NUMBER)
	{
		number = t.Index();
		t.WhiteGet();
	}
	
	if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
		return CMD_BADPARAMS;

	int count = context->DeleteFromInventory(t.Text(), number);
	Output("%d %s deleted", count, t.Text());

	t.WhiteGet();
	
	return 0;
}

COMMAND(CmdGive)
{
	int number = 1;

	if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
		return CMD_BADPARAMS;

	PTObjectInstance inst = MapPane.FindClosestObject(t.Text(), context);
	if (inst < 0)
	{
		Output("Unable to find object %s\n", t.Text());
		t.SkipLine();
		return 0;
	}

	t.WhiteGet();

	if (t.Type() == TKN_NUMBER)
	{
		number = t.Index();
		t.WhiteGet();
	}
	
	if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
		return CMD_BADPARAMS;

	int count = context->GiveInventoryTo(inst, t.Text(), number);
	Output("%d %s given", count, t.Text());

	t.WhiteGet();
	
	return 0;
}

COMMAND(CmdTake)
{
	int number = 1;

	if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
		return CMD_BADPARAMS;

	PTObjectInstance inst = MapPane.FindClosestObject(t.Text(), context);
	if (!inst)
	{
		Output("Unable to find object %s\n", t.Text());
		t.SkipLine();
		return 0;
	}

	t.WhiteGet();

	if (t.Type() == TKN_NUMBER)
	{
		number = t.Index();
		t.WhiteGet();
	}
	
	if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
		return CMD_BADPARAMS;

	int count = inst->GiveInventoryTo(context, t.Text(), number);
	Output("%d %s taken", count, t.Text());

	t.WhiteGet();
	
	return 0;
}

COMMAND(CmdGenerate)
{
	int sizex = 2;
    int sizey = 2;

	S3DPoint pos;
	MapPane.GetMapPos(pos);

	if (!Parse(t, "%d %d", &sizex, &sizey))
	{
		int x = pos.x & (SECTORWIDTH - 1);
		int y = pos.y & (SECTORHEIGHT - 1);

		if (x < 256)
			pos.x -= 256;
		else if (x >= (SECTORWIDTH - 256))
			pos.x += 256;
		else
			sizex = 1;

		if (y < 256)
			pos.y -= 256;
		else if (y >= (SECTORHEIGHT - 256))
			pos.y += 256;
		else
			sizey = 1;

	}

	pos.x >>= SECTORWSHIFT;
	pos.y >>= SECTORHSHIFT;

	GenerateMap(pos.x, pos.y, sizex, sizey);

	return 0;
}

COMMAND(CmdCalcWalkmap)
{
	MapPane.CalculateWalkmap();
	return 0;
}

COMMAND(CmdWalkmap)
{
	int deltaz;
	BOOL nonzero = FALSE, absolute = FALSE;

	while (t.Type() == TKN_IDENT)
	{
		if (t.Is("nonzero", 1))
			nonzero = TRUE;
		else if (t.Is("absolute", 1))
			absolute = TRUE;
		else if (t.Is("reset", 1))
		{
			MapPane.SetWalkmapReveal(0, 0, SECTORWIDTH >> WALKMAPSHIFT, SECTORHEIGHT >> WALKMAPSHIFT);
			t.WhiteGet();
			return 0;
		}
		else
			return CMD_BADPARAMS;

		t.WhiteGet();
	}

	if (!Parse(t, "%d", &deltaz))
		return CMD_BADPARAMS;

	MapPane.AdjustWalkmap(deltaz, nonzero);

	if (context)
		MapPane.SaveWalkmap(context);

	return 0;
}

COMMAND(CmdTileWalkmap)
{
	context->GetImagery()->SetWorldReg(context->GetState(), PLATEWIDTH >> (WALKMAPSHIFT+1), PLATEHEIGHT >> (WALKMAPSHIFT+1), 0);
	context->GetImagery()->SetWorldBoundBox(context->GetState(), PLATEWIDTH >> WALKMAPSHIFT, PLATEHEIGHT >> WALKMAPSHIFT, 0);
	return 0;
}

COMMAND(CmdState)
{
	int newstate;

	if (!Parse(t, "%d", &newstate))
		return CMD_BADPARAMS;

	if (!context->SetState(newstate))
		Output("Invalid state\n");

	return CMD_WAIT;
}


COMMAND(CmdTry)
{
	char newstate[80];

	if (!context->IsComplex())
		return 0;

	if (!Parse(t, "%t", &newstate))
		return CMD_BADPARAMS;

	((PTComplexObject)context)->Try(newstate);

	return CMD_WAIT;
}

COMMAND(CmdForce)
{
	char newstate[80];

	if (!context->IsComplex())
		return 0;

	if (!Parse(t, "%t", &newstate))
		return CMD_BADPARAMS;

	((PTComplexObject)context)->Force(newstate);

	return CMD_WAIT;
}

COMMAND(CmdReveal)
{
	if (context && context->GetFlags() & OF_REVEAL)
		context->ResetFlags(context->GetFlags() & ~OF_REVEAL);

	else if (context)
		context->SetFlags(OF_REVEAL);

	return 0;
}

COMMAND(CmdLevel)
{
	int level;
	if (!Parse(t, "%d", &level))
		return CMD_BADPARAMS;

	MapPane.SetMapLevel(level);
	return 0;
}

COMMAND(CmdTemplate)
{
	int index = TerrainTemplates->NewTemplate(context->GetTypeName());
	if (index < 0)
	{
		Output("Max number of templates reached\n");
		return 0;
	}

	S3DPoint pos;
	context->GetPos(pos);

	int objlist[MAXOBJECTREFS];
	int numfound = MapPane.FindObjectsInRange(pos, objlist, PLATEWIDTH, PLATEHEIGHT, OBJCLASS_TILE, MAXOBJECTREFS);
	int numadded = 0;

	for (int i = 0; i < numfound; i++)
	{
		PTObjectInstance inst = MapPane.GetInstance(objlist[i]);
		if (inst && inst != context)
		{
			S3DPoint ipos;
			inst->GetPos(ipos);
			ipos.x -= pos.x;
			ipos.y -= pos.y;
			ipos.z -= pos.z;

			if (TerrainTemplates->AddObject(index, inst, ipos) < 0)
			{
				Output("Too many objects nearby, the last %d were left out.\n", numfound - i);
				break;
			}

			numadded++;
		}
	}

	Output("%d objects added to template for '%s'.\n", numadded, context->GetTypeName());

	return 0;
}

COMMAND(CmdSmoothScroll)
{
	int retval = 0;

	if (t.Is("on"))
	{
		if (!SmoothScroll)
		{
			SmoothScroll = TRUE;
			MapPane.RedrawAll();
		}
	}
	else if (t.Is("off"))
	{
		if (SmoothScroll)
		{
			SmoothScroll = FALSE;
			MapPane.RedrawAll();
		}
	}
	else
		retval = CMD_BADPARAMS;

	t.Get();

	return retval;
}

COMMAND(CmdDLight)
{
	int intensity;

	if (!Parse(t, "%d", &intensity))
		return CMD_BADPARAMS;

	MapPane.SetDLightIntensity(intensity);
	return 0;
}


COMMAND(CmdText)
{
	Console.Hide();
	ScrollEditor.Show();
	PlayScreen.SetExclusivePane(&ScrollEditor);
	ScrollEditor.SetScroll(context);

	return 0;
}

COMMAND(CmdExit)
{
	if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
		return CMD_BADPARAMS;

	strcpy(buf, t.Text());
	t.WhiteGet();

	BOOL getamb = TRUE;
	if (t.Type() == TKN_IDENT && t.Is("noambient", 1))
	{
		getamb = FALSE;
		t.WhiteGet();
	}

	TExit::AddExit(buf, context, getamb);

	Output("Exit from '%s' added.\n", buf);

	TExit::WriteExitList();		// write immediately just to be safe

	return 0;
}

COMMAND(CmdFollow)
{
	if (!((PTExit)context)->Activate())
		Output("Nothing defined for this exit, can't follow\n");

	return 0;
}

COMMAND(CmdRestore)
{
	((PTCharacter)context)->RestoreHealth();
	return 0;
}

COMMAND(CmdGet)
{
	if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
		return CMD_BADPARAMS;

	int index = -1;
	PTObjectInstance inst = MapPane.FindClosestObject(t.Text(), NULL, TRUE);

	if (inst == NULL)
		Output("Can't find any object by that name.\n");
	else
	{
		if (inst->IsInInventory())
			inst->RemoveFromInventory();
		else
			inst->RemoveFromMap();

		context->AddToInventory(inst);
	}

	t.WhiteGet();

	return 0;
}

COMMAND(CmdSectorCommand)
{
	char command[512];
	int level;

	strcpy(command, (char *)t.GetPos());

	if (!Parse(t, "%d", &level))
		return CMD_BADPARAMS;

	if (t.Type() == TKN_EOF || t.Type() == TKN_RETURN)
		return CMD_BADPARAMS;

	sprintf(buf, "Processing command \"%s\" on level %d...\n", command, level);
	Output(buf);

	if (MapPane.GetMapLevel() == level)
	{
		MapPane.FreeAllSectors();
		MapPane.RedrawAll();
	}

	IsSectorCommand = TRUE;

	for (int sy = 0; sy < MAXSECTORY; sy++)
		for (int sx = 0; sx < MAXSECTORX; sx++)
		{
			PTSector sector = new TSector(level, sx, sy);
			sector->Load(FALSE);

			for (int i = 0; i < sector->NumItems(); i++)
			{
				PTObjectInstance inst = sector->GetInstance(i);
				if (inst)
				{
					TStringParseStream s(command, strlen(command));
					TToken t0(s);
					t0.Get();
					CommandInterpreter(inst, t0, MINCMDABREV);
				}
			}

			sector->Save();
			delete sector;
		}

	while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
		t.WhiteGet();

	IsSectorCommand = FALSE;

	return 0;
}

COMMAND(CmdSwap)
{
	if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
		return CMD_BADPARAMS;

	PTObjectInstance inst = MapPane.FindClosestObject(t.Text(), NULL, TRUE);

	if (inst == NULL)
		Output("Can't find any object by that name.\n");
	else
		if (!MapPane.SwapDrawOrder(context, inst))
			Output("Hrm...the objects are in different sectors.  I guess you're screwed.\n");

	t.WhiteGet();

	return 0;
}

COMMAND(CmdToFront)
{
	MapPane.PushToFront(context);
	return 0;
}

COMMAND(CmdToBack)
{
	MapPane.PushToBack(context);
	return 0;
}

/*COMMAND(CmdAlias)
{
	if (t.Type() == TKN_IDENT)
	{
		// Get a list of aliases
		if (t.Is("list"))
		{
			t.WhiteGet();
			for (int c = 0; c < NumAliases(); c++)
			{
				sprintf(buf, "%s = %s\n", GetAliasName(c), GetAlias(c));
				Output(buf);
			}
			return 0;
		}
	
		// Set a new alias
		char aliasname[30], alias[30];
		if (!Parse(t, "%29s", aliasname))
			return CMD_BADPARAMS;

		if (t.Type() == TKN_IDENT)
			if (!Parse(t, "%30s", aliasname))
				return CMD_BADPARAMS;
		if (t.Type() == TKN_TEXT)
			if (!Parse(t, "%30t", aliasname))
				return CMD_BADPARAMS;
			
		if (SetAlias(aliasname, alias) >= 0)
			Output("Alias set");
		else
			Output("Too many aliases");
	}

	return 0;
}
*/
COMMAND(CmdGroup)
{
	int group;

	if (t.Type() == TKN_NUMBER)			// Set our group
	{
		if (!Parse(t, "%d", &group))
			return CMD_BADPARAMS;

		if (context)
			context->SetGroup(group);
	}
	else								// Show what group we're in
	{
		if (context)
		{
			sprintf(buf, "Group: %d\n", context->GetGroup());
			Output(buf);
		}
	}
	return 0;
}

COMMAND(CmdPlay)
{
	if (!Parse(t, "%t", buf))
		return CMD_BADPARAMS;

	PLAY(buf);

	return 0;
}

COMMAND(CmdTrigger)
{
	if (t.Type() != TKN_IDENT)
		return CMD_BADPARAMS;

	if (context->GetScript())
		context->GetScript()->Trigger(TRIGGER_TRIGGER, t.Text());

	t.WhiteGet();

	return 0;
}

COMMAND(CmdNewGame)
{
	PlayScreen.NewGame();

	return 0;
}

COMMAND(CmdCurPlayer)
{
	PlayerManager.SetMainPlayer((PTPlayer)context);
	
	Output("%s is now current player\n", context->GetName());

	return 0;
}

COMMAND(CmdUndo)
{
	StatusBar.Undo();
	return 0;
}

COMMAND(CmdResetScreenRect)
{
	if (!context->GetAnimator())
		Output("Context has no animator\n");
	else
	{
		int state;
		if (!Parse(t, "%d", &state))
			state = -1;

		((PT3DAnimator)context->GetAnimator())->RecordNewExtents(context, state);
	}

	return 0;
}

COMMAND(CmdBounds)
{
	int regx, regy, width, length;

	if (!Parse(t, "%d %d %d %d", &regx, &regy, &width, &length))
		return CMD_BADPARAMS;

	int state = context->GetState();
	if (t.Type() == TKN_NUMBER)
		Parse(t, "%d", &state);

	if (!context->GetImagery())
		Output("Sorry, context has no imagery\n");
	else
	{
		context->GetImagery()->SetWorldBoundBox(state, 0, 0, 1);
		context->GetImagery()->SetWorldReg(state, 0, 0, 0);
		context->GetImagery()->SetWorldBoundBox(state, width, length, 1);
		context->GetImagery()->SetWorldReg(state, regx, regy, 0);
		if (StatusBar.EditWalkmap() && state == context->GetState())
			MapPane.SnapWalkDisplay(context->GetMapIndex());
	}

	return 0;
}

COMMAND(CmdToggle)
{
	if (t.Type() != TKN_IDENT)
		return CMD_BADPARAMS;

	char *flagname = NULL;

	for (int i = 0; i < context->GetNumFlags(); i++)
	{
		flagname = context->GetFlagName(i);
		if (t.Is(flagname, 3))
			break;
	}

	if (!flagname)
	{
		Output("No object flag by that name\n");
		while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
			t.Get();
		return 0;
	}
	else
	{
		t.WhiteGet();
		
		if (t.Is("="))
			t.WhiteGet();

		BOOL value;
		if (t.Is("on") || t.Is("1") || t.Is("true"))
		{
			value = 1;
			t.WhiteGet();
		}
		else if (t.Is("off") || t.Is("0") || t.Is("false"))
		{
			value = 0;
			t.WhiteGet();
		}
		else
		{
			value = !context->IsFlagSet(flagname);
		}

		context->SetFlag(flagname, value);
	}

	return 0;
}

COMMAND(CmdMove)
{
	int newx = 0, newy = 0, newz = 0;

	if (!Parse(t, "%i %i", &newx, &newy))
		return CMD_BADPARAMS;

	if (t.Type() == TKN_NUMBER)
		if (!Parse(t, "%i", &newz))
			return CMD_BADPARAMS;

	S3DPoint oldpos;
	context->GetPos(oldpos);
	S3DPoint newpos = oldpos;

	newpos.x += newx;
	newpos.y += newy;
	newpos.z += newz;

	MapPane.AddObjectUpdateRect(context->GetMapIndex());
	context->SetPos(newpos, -1, IsSectorCommand);
	MapPane.AddObjectUpdateRect(context->GetMapIndex());

	return 0;
}

COMMAND(CmdPos)
{
	S3DPoint newpos;
	context->GetPos(newpos);
	int newlevel = -1;

	if (!Parse(t, "%i %i", &newpos.x, &newpos.y))
		return CMD_BADPARAMS;

	if (t.Type() == TKN_NUMBER)
		if (!Parse(t, "%i", &newpos.z))
			return CMD_BADPARAMS;

	if (t.Type() == TKN_NUMBER)
		if (!Parse(t, "%i", &newlevel))
			return CMD_BADPARAMS;

	MapPane.AddObjectUpdateRect(context->GetMapIndex());
	context->SetPos(newpos, newlevel, IsSectorCommand);
	MapPane.AddObjectUpdateRect(context->GetMapIndex());

	return 0;
}

COMMAND(CmdDelete)
{
	MapPane.DeleteObject(context);
	return CMD_DELETED;
}

COMMAND(CmdStat)
{
	PTObjectClass cl = TObjectClass::GetClass(context->ObjClass());

	if (t.Type() == TKN_IDENT)
	{
		if (t.Is("list"))
		{
			int c;
			t.WhiteGet();
			Output("Class stats\n");
			for (c = 0; c < cl->NumStats(); c++)
			{
				Output("%s = %d\n", cl->GetStatDefString(c, buf), cl->GetStat(context->ObjType(), c));
				if (c != 0 && (c % 5) == 0)
				{
					Output("Press any key");
					Console.GetChar();
				}
			} 
			Output("Object stats\n");
			Console.GetChar();
			for (c = 0; c < cl->NumObjStats(); c++)
			{
				Output("%s (%d) = %d\n", cl->GetObjStatDefString(c, buf), cl->GetObjStat(context->ObjType(), c), context->GetObjStat(c));
				if (c != 0 && (c % 5) == 0)
				{
					Output("Press any key");
					Console.GetChar();
				}
			} 
			return 0;
		}

		strcpy(buf, t.Text());
		t.WhiteGet();

		int statid = cl->FindStat(buf);
		int objstatid = cl->FindObjStat(buf);
		if (statid < 0 && objstatid < 0)
		{
			Output("No stat by that name in that object's class\n");
			return 0;
		}

		if (t.Is("delete"))
		{
			t.WhiteGet();

			if (statid >= 0)
				cl->DeleteStat(statid);
			if (objstatid >= 0)
				cl->DeleteObjStat(objstatid);

			Output("Stat Deleted\n");
			return 0;
		}

		if (t.Is("="))
		{
			t.WhiteGet();

			int val;
			if (!Parse(t, "%i", &val))
				return CMD_BADPARAMS;
	
			context->SetStat(buf, val);

			Output("Stat Set: ");
		}

		if (t.Is("reset"))
		{
			t.WhiteGet();

			if (statid >= 0)
				cl->ResetStat(context->ObjType(), statid);
			else if (objstatid >= 0)
				context->ResetStat(objstatid);

			Output("Stat Reset: ");
		}

		if (t.Type() != TKN_EOF)
			return CMD_BADPARAMS;

		if (statid >= 0)
			Output("%s = %d\n", cl->GetStatDefString(statid, buf), cl->GetStat(context->ObjType(), statid));
		else if (objstatid >= 0)
			Output("%s (%d) = %d\n", cl->GetObjStatDefString(objstatid, buf), cl->GetObjStat(context->ObjType(), objstatid), context->GetObjStat(objstatid));
		return 0;
	}

	// if no params, just display the object's stats
	S3DPoint pos;
	context->GetPos(pos);
	sprintf(buf, "\"%s\" %s:%s (%d, %d, %d)\n", context->GetName(),
			context->GetTypeName(), context->GetClassName(), pos.x, pos.y, pos.z);
	Output(buf);

	strcpy(buf, "Flags:");
	if (context->GetFlags())
		sprintbit(context->GetFlags(), objflags, buf);
	else
		strcat(buf, " (none)");
	strcat(buf, "\n");
	Output(buf);

    PTObjectImagery i = context->GetImagery();
	int s = context->GetState();
    sprintf(buf, "Registration (%d, %d, %d)  AnimReg (%d, %d, %d)\n",
			i->GetRegX(s), i->GetRegY(s), i->GetRegZ(s),
			i->GetAnimRegX(s), i->GetAnimRegY(s), i->GetAnimRegZ(s));
	Output(buf);

	if (context->GetFlags() & OF_LIGHT)
	{
		PSLightDef def;
		def = context->GetLightDef();

		sprintf(buf, "Light: intensity %d, multiplier %d, flags [", def->intensity, def->multiplier);

		if (context->GetLightFlags())
			sprintbit(context->GetLightFlags(), lightflags, buf);
		else
			strcat(buf, "--");

		sprintf(buf, "%s]\n       color (%d, %d, %d), pos (%d, %d, %d)\n"
					 "       3d index: %d lightid: %d\n",
				buf, def->color.red, def->color.green, def->color.blue,
				def->pos.x, def->pos.y, def->pos.z,
				def->lightindex, def->lightid);
		Output(buf);
	}

	return 0;
}

COMMAND(CmdScript)
{
	if (context->GetScript())
	{
		Console.Hide();
		ScriptEditor.Show();
		PlayScreen.SetExclusivePane(&ScriptEditor);
		ScriptEditor.LoadScript(context);
	}

	return 0;
}

int CenterOnFunc(PTObjectInstance context, TToken &t, BOOL scroll)
{
	S3DPoint pos;

	if (Editor && context && t.Type() != TKN_IDENT && t.Type() != TKN_NUMBER)
	{
		context->GetPos(pos);
		MapPane.SetMapPos(pos);
	}
	else
	{
		if (t.Type() == TKN_IDENT)
		{
			PTObjectInstance inst = MapPane.FindClosestObject(t.Text());
			if (!inst)
			{
				Output("Object not found");
				return 0;
			}
			if (Editor)
			{
				inst->GetPos(pos);
				MapPane.SetMapPos(pos);
			}
			else
				MapPane.CenterOnObj(inst, scroll);

			t.WhiteGet();
		}
		else if (t.Type() == TKN_NUMBER)
		{
			if (!Parse(t, "%i %i %i", &pos.x, &pos.y, &pos.z))
				return CMD_BADPARAMS;
			int level = MapPane.GetMapLevel();
			if (t.Type() == TKN_NUMBER)
				if (!Parse(t, "%i", &level))
					return CMD_BADPARAMS;
			if (Editor)
			{
				MapPane.SetMapPos(pos);
			}
			else
				MapPane.CenterOnPos(pos, level, scroll);
		}
		else
			return CMD_BADPARAMS;
	}

	return 0;
}

COMMAND(CmdCenterOn)
{
	return CenterOnFunc(context, t, FALSE);
}

COMMAND(CmdScrollTo)
{
	return CenterOnFunc(context, t, TRUE);
}

#define MAX_MAP_LOCATIONS		32

struct { char name[RESNAMELEN]; struct { int x, y, z; } pos; int level; } MapLocations[MAX_MAP_LOCATIONS];
int numlocations = 0;

BOOL ReadMapLocationList()
{
	char fname[MAX_PATH];
	sprintf(fname, "%slocation.def", ClassDefPath);

	FILE *fp = TryOpen(fname, "rb");
	if (!fp)
		return FALSE;

	TFileParseStream s(fp);
	TToken t(s);

	t.Get();

	numlocations = 0;

	do
	{
		if (numlocations >= MAX_MAP_LOCATIONS)
			break;

		if (t.Type() == TKN_RETURN || t.Type() == TKN_WHITESPACE)
			t.LineGet();

		if (t.Type() == TKN_EOF)
			break;

		if (!Parse(t, "%t (%d, %d, %d) level %d", MapLocations[numlocations].name, &MapLocations[numlocations].pos.x, &MapLocations[numlocations].pos.y, &MapLocations[numlocations].pos.z, &MapLocations[numlocations].level))
			return FALSE;

		t.SkipLine();		// skip past any other garbage on the line, including the newline

		numlocations++;
	} while (t.Type() != TKN_EOF);

	fclose(fp);
	return TRUE;
}

COMMAND(CmdMapPos)
{
	S3DPoint pos;
	int level = MapPane.GetMapLevel();

	if (t.Type() == TKN_IDENT)
	{
		if (t.Is("list"))
		{
			Output("The following map locations are availible:\n");

			for (int i = 0; i < numlocations; i++)
			{
				sprintf(buf, "%20s (%5d, %5d, %3d) level %2d\n", MapLocations[i].name, MapLocations[i].pos.x, MapLocations[i].pos.y, MapLocations[i].pos.z, MapLocations[i].level);
				Output(buf);
			}

			t.WhiteGet();
			return 0;
		}

		for (int i = 0; i < numlocations; i++)
			if (t.Is(MapLocations[i].name, 3))
			{
				pos.x = MapLocations[i].pos.x;
				pos.y = MapLocations[i].pos.y;
				pos.z = MapLocations[i].pos.z;
				level = MapLocations[i].level;
				break;
			}

		t.WhiteGet();

		if (i >= numlocations)
		{
			Output("No map location by that name exists.  Try 'map list' for a listing.\n");
			return CMD_BADPARAMS;
		}
	}
	else if (!Parse(t, "%d %d %d", &pos.x, &pos.y, &pos.z))
		return CMD_BADPARAMS;

	MapPane.SetMapPos(pos);
	MapPane.SetMapLevel(level);
	return 0;
}

BOOL ShowObjList(PTObjectClass cl, char *match)
{
	BOOL ismatch = FALSE;
	int cnt = 0;

	for (int i = 0; i < cl->NumTypes(); i++)
	{
		if (cl->GetObjType(i) == NULL || (match != NULL && abbrevcmp(match, cl->GetObjType(i)->name) < 1))
			continue;

		ismatch = TRUE;

		sprintf(buf, "%s %-19s", buf, cl->GetObjType(i)->name);
		if (++cnt >= 3)
		{
			strcat(buf, "\n");
			Output(buf);
			buf[0] = 0;
			cnt = 0;
		}
	}

	if (cnt)
	{
		strcat(buf, "\n");
		Console.Output(buf);
	}

	return ismatch;
}

void ShowObjTypeInfo(PTObjectClass cl, int objtype)
{
	PSObjectInfo inf = cl->GetObjType(objtype);

	if (!cl || !inf)
		return;
/*
	sprintf(buf, "Name: %s, Class: %s, Imagery: %s, Usecount: %d\n",
			inf->name, cl->ClassName(), img->GetUseCount(), img->Get);
	Output(buf);
  */
	if (cl->NumStats())
	{
		Output("Statistics:\n");

		int len = 0, add;

		for (int i = 0; i < cl->NumStats(); i++)
		{
			sprintf(buf+len, "%s = %d, ", cl->StatName(i), cl->GetStat(objtype, i));
			add = strlen(buf) - len;
			if ((len+add) >= Console.winwidth && len != 0)
			{
				if (i == (cl->NumStats() - 1))
					len -= 2;
				buf[len] = '\n';
				buf[len+1] = 0;
				Output(buf);
				strcpy(buf, buf+len+2);
				len = add;
			}
			else
				len += add;
		}

		if (len > 0)
		{
			len -= 2;
			buf[len] = '\n';
			buf[len+1] = 0;
			Output(buf);
		}
	}
}

COMMAND(CmdShow)
{
	PTObjectClass cl;
	buf[0] = 0;

	if (t.Is("classes"))
	{
		Output("Object Classes:\n");
		for (int i = 0, cnt = 0; i < TObjectClass::NumClasses(); i++)
		{
			cl = TObjectClass::GetClass(i);
			if (cl == NULL)
				continue;

			sprintf(buf, "%s %-19s", buf, cl->ClassName());
			if (++cnt >= 3)
			{
				strcat(buf, "\n");
				Output(buf);
				buf[0] = 0;
				cnt = 0;
			}
		}

		if (cnt)
		{
			strcat(buf, "\n");
			Output(buf);
		}
	}
	else
	{
		if ((cl = TObjectClass::GetClass(TObjectClass::FindClass(t.Text()))))
		{
			t.WhiteGet();
			if (t.Type() == TKN_IDENT)
			{
				int objtype = cl->FindObjType(t.Text());
				if (objtype < 0)
				{
					if (!ShowObjList(cl, t.Text()))
					{
						sprintf(buf, "%s contains no type named '%s'.\n", cl->ClassName(), t.Text());
						Output(buf);
					}
				}
				else
					ShowObjTypeInfo(cl, objtype);
			}
			else
				ShowObjList(cl, NULL);
		}
		else
			Output("No class by that name.\n");
	}

	t.WhiteGet();
	return 0;
}

COMMAND(CmdAmbient)
{
	int amb;
	if (!Parse(t, "%d", &amb))
		return CMD_BADPARAMS;

	MapPane.SetAmbientLight(amb);
	MapPane.RedrawAll();

	return 0;
}

COMMAND(CmdAmbColor)
{
	SColor color;
	if (!Parse(t, "%b %b %b", &color.red, &color.green, &color.blue))
		return CMD_BADPARAMS;

	MapPane.SetAmbientColor(color);
	MapPane.RedrawAll();

	return 0;
}

void Mono(int percent, int dummy = -1)
{
	SetMonoPercent(percent);
	MapPane.RedrawAll();
}

COMMAND(CmdMono)
{
	int percent;
	if (!Parse(t, "%d", &percent))
		return CMD_BADPARAMS;

	Mono(percent);
	return 0;
}

COMMAND(CmdDirLight)
{
	return 0;
}

COMMAND(CmdBaseLight)
{
	SColor color;
	int id, mult = -1;

	if (!Parse(t, "%d %b %b %b <%d>", &id, &color.red, &color.green, &color.blue, &mult))
		return CMD_BADPARAMS;

	SetLightColor(id, color, mult);
	return 0;
}

COMMAND(CmdReplace)
{
	PTObjectClass cl = TObjectClass::GetClass(TObjectClass::FindClass(t.Text()));
	if (cl)
		t.WhiteGet();
	else
		cl = TObjectClass::GetClass(context->ObjClass());

	if (cl)
	{
		// Add a new instance of a given object type in the map pane
		int objtype = cl->FindObjType(t.Text());
		if (objtype < 0)
		{
			sprintf(buf, "%s has no object named '%s'.\n", cl->ClassName(), t.Text());
			Output(buf);
		}
		else
		{
			SObjectDef def;
			memset(&def, 0, sizeof(SObjectDef));

			def.objclass = cl->ClassId();
			def.objtype = objtype;
			def.level = MapPane.GetMapLevel();

			context->GetPos(def.pos);
			int index = MapPane.NewObject(&def);
			if (index < 0)
				Console.Output("ERROR: Creating object\n");
			else
			{
				int oldindex = context->GetMapIndex();
				StatusBar.Deselect(oldindex);
				PTObjectInstance oi = MapPane.GetInstance(oldindex);
				MapPane.DeleteObject(oi);
				StatusBar.Select(index, TRUE);
			}
		}
	}
	else
	{
		sprintf(buf, "No class named '%s'.\n", t.Text());
		Output(buf);
	}

	return 0;
}

COMMAND(CmdLight)
{
	if (t.Is("on"))
	{
		if (!(context->GetFlags() & OF_LIGHT))
		{
			context->SetFlags(OF_LIGHT);
			MapPane.AddObjectUpdateRect(context->GetMapIndex());
		}
		t.WhiteGet();
		return 0;
	}

	if (t.Is("off"))
	{
		if (context->GetFlags() & OF_LIGHT)
		{
			context->ResetFlags(context->GetFlags() & (~OF_LIGHT));
			MapPane.AddObjectUpdateRect(context->GetMapIndex());
		}
		t.WhiteGet();
		return 0;
	}

	if (t.Type() == TKN_NUMBER)
	{
		t.WhiteGet();
		int intensity;
		if (!Parse(t, "%d", &intensity))
			return CMD_BADPARAMS;

		context->SetLightIntensity(intensity);
		return 0;
	}

	if (t.Is("directional", MINCMDABREV))
	{
		t.WhiteGet();
		DWORD oldflags = context->GetFlags();

		if (t.Is("on"))
			context->SetLightFlags(LIGHT_DIR);
		else if (t.Is("off"))
			context->ResetLightFlags(context->GetLightFlags() & (~LIGHT_DIR));
		else
		{
			t.WhiteGet();
			return CMD_BADPARAMS;
		}

		if (oldflags != context->GetFlags())
			MapPane.AddObjectUpdateRect(context->GetMapIndex());

		t.WhiteGet();
		return 0;
	}

	if (t.Is("intensity", MINCMDABREV))
	{
		t.WhiteGet();
		int intensity;
		if (!Parse(t, "%d", &intensity))
			return CMD_BADPARAMS;

		context->SetLightIntensity(intensity);
		return 0;
	}

	if (t.Is("color", MINCMDABREV))
	{
		t.WhiteGet();
		SColor color;
		if (!Parse(t, "%b %b %b", &color.red, &color.green, &color.blue))
			return CMD_BADPARAMS;

		context->SetLightColor(color);
		return 0;
	}

	if (t.Is("position", MINCMDABREV))
	{
		t.WhiteGet();
		S3DPoint lightpos;
		if (!Parse(t, "%d %d %d", &lightpos.x, &lightpos.y, &lightpos.z))
			return CMD_BADPARAMS;

		context->SetLightPos(lightpos);
		return 0;
	}

	if (t.Is("multiplier", MINCMDABREV))
	{
		t.WhiteGet();
		int mult;
		if (!Parse(t, "%d", &mult))
			return CMD_BADPARAMS;

		context->SetLightMultiplier(mult);
		return 0;
	}

	return CMD_BADPARAMS;
}

void ZOffset(int zoffset, int dummy = -1)
{
	PTObjectInstance oi = MapPane.GetInstance(StatusBar.GetSelectedObj());

	if (oi)
	{
		PTObjectImagery img = oi->GetImagery();

		if (img)
		{
			int x = img->GetRegX(oi->GetState());
			int y = img->GetRegY(oi->GetState());
			img->SetReg(oi->GetState(), x, y, zoffset);
		}
	}
}

COMMAND(CmdZOffset)
{
	if (t.Type() == TKN_RETURN || t.Type() == TKN_EOF)
	{
		sprintf(buf, "Current zoffset: %d\n", context->GetImagery()->GetRegZ(context->GetState()));
		Output(buf);
		t.WhiteGet();
		return 0;
	}

	int zoff;
	if (!Parse(t, "%d", &zoff))
		return CMD_BADPARAMS;

	PTObjectImagery img = context->GetImagery();

	if (img)
	{
		int x = img->GetRegX(context->GetState());
		int y = img->GetRegY(context->GetState());
		img->SetReg(context->GetState(), x, y, zoff);
	}

	MapPane.AddObjectUpdateRect(context->GetMapIndex());
	return 0;
}

COMMAND(CmdRegistration)
{
	int dx, dy;
	if (!Parse(t, "%d %d", &dx, &dy))
		return CMD_BADPARAMS;

	PTObjectImagery img = context->GetImagery();
	if (!img)
		return 0;

	int x = img->GetRegX(context->GetState());
	int y = img->GetRegY(context->GetState());
	int z = img->GetRegZ(context->GetState());
	img->SetReg(context->GetState(), x - dx, y - dy, z);
	MapPane.AddObjectUpdateRect(context->GetMapIndex());
	return 0;
}

COMMAND(CmdAnimRegistration)
{
	int dx, dy;
	if (!Parse(t, "%d %d", &dx, &dy))
		return CMD_BADPARAMS;

	PTObjectImagery img = context->GetImagery();
	if (!img)
		return 0;

	int x = img->GetAnimRegX(context->GetState());
	int y = img->GetAnimRegY(context->GetState());
	int z = img->GetAnimRegZ(context->GetState());
	img->SetAnimReg(context->GetState(), x - dx, y - dy, z);
	return 0;
}

COMMAND(CmdAnimZ)
{
	int dz;
	if (!Parse(t, "%d", &dz))
		return CMD_BADPARAMS;

	PTObjectImagery img = context->GetImagery();
	if (!img)
		return 0;

	int x = img->GetAnimRegX(context->GetState());
	int y = img->GetAnimRegY(context->GetState());
	img->SetAnimReg(context->GetState(), x, y, dz);
	return 0;
}

COMMAND(CmdName)
{
	if (t.Type() != TKN_IDENT)
		return CMD_BADPARAMS;

	if (t.Is("type"))
	{
		t.WhiteGet();
		if (t.Type() != TKN_IDENT)
			return CMD_BADPARAMS;

		context->SetTypeName(t.Text());
	}
	else if (t.Is("clear"))
		context->SetName("");
	else
		context->SetName(t.Text());

	t.WhiteGet();
	return 0;
}

COMMAND(CmdFlip)
{
	MapPane.AddObjectUpdateRect(context->GetMapIndex());

	if (context->GetFlags() & OF_DRAWFLIP)
		context->ResetFlags(context->GetFlags() & ~(OF_DRAWFLIP));
	else
		context->SetFlags(OF_DRAWFLIP);

	MapPane.AddObjectUpdateRect(context->GetMapIndex());
	return 0;
}

COMMAND(CmdLock)
{
	context->SetFlags(OF_EDITORLOCK);
	return 0;
}

COMMAND(CmdUnlock)
{
	context->ResetFlags(context->GetFlags() & (~OF_EDITORLOCK));
	return 0;
}

COMMAND(CmdImmobile)
{
	context->SetFlags(OF_IMMOBILE);
	return 0;
}

COMMAND(CmdUnimmobile)
{
	context->ResetFlags(context->GetFlags() & (~OF_IMMOBILE));
	return 0;
}

COMMAND(CmdLoad)
{
	Output("Reloading...\n");
	MapPane.ReloadSectors();
	return 0;
}

COMMAND(CmdSave)
{
	// suspend all activity while saving to avoid any nastiness
	TObjectImagery::PauseLoader();

	BOOL savegame, savemap, saveheaders, saveclasses, saveexits;

	savegame = FALSE; // This must be set specifically

	if (t.Type() != TKN_IDENT)
		savemap = saveheaders = saveclasses = saveexits = TRUE;
	else
	{
		savemap = saveheaders = saveclasses = saveexits = FALSE;

		while (t.Type() == TKN_IDENT)
		{
			if (t.Is("game"))
				savegame = TRUE;
			if (t.Is("map"))
				savemap = TRUE;
			if (t.Is("headers"))
				saveheaders = TRUE;
			if (t.Is("classes"))
				saveclasses = TRUE;
			if (t.Is("exits"))
				saveexits = TRUE;
			t.WhiteGet();
		}
	}

	if (savegame)
	{
		Output("Saving game...\n");
		SaveGame.WriteGame();
	}

	if (savemap)
	{
		Output("Saving map sectors...\n");
		MapPane.SaveAllSectors();
		MapPane.SaveCurMap();  // Copies new sector files to main game map dir
		MapPane.ClearCurMap(); // Clears all map sectors from the 'curmap' directory
	}

	if (saveheaders)
	{
		if (!Console.AdjustingBounds())
		{
			TObjectImagery::SaveAllHeaders();
			Output("Saving object headers...\n");
		}
		else
			Output(" (imagery headers not saved - currently being edited)\n");
	}

	if (saveclasses)
	{
		Output("Saving classes..\n");
		TObjectClass::SetClassesDirty(); // Force class def to save
		TObjectClass::SaveClasses();
		TerrainTemplates->Save();
	}

	if (saveexits)
	{
		Output("Saving exits...\n");
		TExit::WriteExitList();
	}
	
	TObjectImagery::ResumeLoader();

	return 0;
}

COMMAND(CmdDXStats)
{
	Output("Direct X Stats.\n");
	sprintf(buf, "Display bits per pixel: %d\n", Display->BitsPerPixel());
	Output(buf);
	sprintf(buf, "Video Memory Total    : %3.1f K\n",
		(float)(GetFreeVideoMem()) / 1024.0);
	Output(buf);

	sprintf(buf, "Video Memory Available: %3.1f K\n",
		(float)(GetFreeVideoMem()) / 1024.0);

	Output(buf);

	if (BlitHardware)
		Output("\nBlit hardware available.\n");
	else
		Output("\nBlit hardware not available.\n");

	sprintf(buf, "Number of 3D objects  : %d\n", Scene3D.GetNumAnimators());
	Output(buf);

	sprintf(buf, "Number of 3D lights   : %d\n", Scene3D.GetNumLights());
	Output(buf);

	if (GetColorMode() == MONO)
		Output("\nRAMP emulation mode on.\n");

	else if (GetColorMode() == COLOR)
		Output("\nRGB emulation mode on.\n");
	else
		Output("\nEmulation mode currently undefined.\n");

	if (IsUsingHardware())
		Output("Hardware 3D support.\n");
	else
		Output("Software 3D support.\n");

	return 0;
}

COMMAND(CmdMemory)
{
	extern MEMORYSTATUS StartMemory;
	extern DWORD ImageryMemUsage;
	extern TotalAllocated, MaxAllocated;

	MEMORYSTATUS mem;
	mem.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&mem);

	Output("Starting memory usage:\n");
	sprintf(buf, "   Percent used: %d%%\n", StartMemory.dwMemoryLoad);
	Output(buf);
	sprintf(buf, "   Physical: %4.1fM  Free: %4.1fM\n", (float)StartMemory.dwTotalPhys / 1024 / 1024, (float)StartMemory.dwAvailPhys / 1024 / 1024);
	Output(buf);
	sprintf(buf, "   Paged:    %4.1fM  Free: %4.1fM\n", (float)StartMemory.dwTotalPageFile / 1024 / 1024, (float)StartMemory.dwAvailPageFile / 1024 / 1024);
	Output(buf);
	sprintf(buf, "   Virtual:  %4.1fM  Free: %4.1fM\n", (float)StartMemory.dwTotalVirtual / 1024 / 1024, (float)StartMemory.dwAvailVirtual / 1024 / 1024);
	Output(buf);

	Output("Current memory usage:\n");
	sprintf(buf, "   Percent used: %d%%\n", mem.dwMemoryLoad);
	Output(buf);
	sprintf(buf, "   Physical: %4.1fM  Free: %4.1fM\n", (float)mem.dwTotalPhys / 1024 / 1024, (float)mem.dwAvailPhys / 1024 / 1024);
	Output(buf);
	sprintf(buf, "   Paged:    %4.1fM  Free: %4.1fM\n", (float)mem.dwTotalPageFile / 1024 / 1024, (float)mem.dwAvailPageFile / 1024 / 1024);
	Output(buf);
	sprintf(buf, "	Virtual:  %4.1fM  Free: %4.1fM\n", (float)mem.dwTotalVirtual / 1024 / 1024, (float)mem.dwAvailVirtual / 1024 / 1024);
	Output(buf);

	sprintf(buf, "Memory usaged by object imagery: %4.1fM\n", (float)ImageryMemUsage / 1024 / 1024);
	Output(buf);
	sprintf(buf, "Memory usaged by chunk cache: %4.1fM\n", (float)ChunkCache.MemUsed() / 1024 / 1024);
	Output(buf);
	sprintf(buf, "Memory allocated:  %4.1fM  Max: %4.1fM\n", (float)TotalAllocated / 1024 / 1024, (float)MaxAllocated / 1024 / 1024);
	Output(buf);

	return 0;
}

// *******************
// * TEditStatusPane *
// *******************

void BtnInvert()
{
	StatusBar.InvertAxisButtons();
}

void BtnPrev()
{
	StatusBar.Prev();
}

void BtnNext()
{
	StatusBar.Next();
}

void BtnDeselect()
{
	StatusBar.Deselect();
}

void BtnCenterOn()
{
	S3DPoint pos;
	PTObjectInstance inst = MapPane.GetInstance(StatusBar.GetSelectedObj());
	if (inst)
	{
		inst->GetPos(pos);
		MapPane.SetMapPos(pos);
	}
}

void BtnUndo()
{
	StatusBar.Undo();
}

void BtnWalkmap()
{
	MapPane.RedrawAll();
	MapPane.ResetDrag();

	int index = StatusBar.GetSelectedObj();
	PTObjectInstance oi = MapPane.GetInstance(index);
	if (oi)
		MapPane.SaveWalkmap(oi);
}

void BtnScroll()
{
	SmoothScroll = !SmoothScroll;
	MapPane.RedrawAll();
}

BOOL TEditStatusPane::Initialize()
{
	TButtonPane::Initialize();

	iterator = curobj = -1;
	numobjs = 0;

	// Grid, X, Y, and Z buttons must be the first buttons created in this pane
	NewButton("Grid",	0,			   0, 32, GetHeight(), 0xC0, NULL, NULL, NULL,  FALSE, TRUE);
	NewButton("X",		32,			   0, 16, GetHeight(), 'X', NULL, NULL, NULL, FALSE, TRUE);
	NewButton("Y",		48,			   0, 16, GetHeight(), 'Y', NULL, NULL, NULL,  FALSE, TRUE);
	NewButton("Z",		64,			   0, 16, GetHeight(), 'H', NULL, NULL, NULL,  FALSE, TRUE);
	NewButton("Inv",	80,			   0, 32, GetHeight(), VK_TAB, BtnInvert);
	NewButton("/\\",    GetWidth() - 200, 0, 20, GetHeight(),   0, BtnPrev);
	NewButton("\\/",    GetWidth() - 180, 0, 20, GetHeight(),   0, BtnNext);
	NewButton("Cen",	GetWidth() - 160, 0, 32, GetHeight(), 'C', BtnCenterOn);
	NewButton("Undo",	GetWidth() - 128, 0, 32, GetHeight(), 'Z', BtnUndo);
	NewButton("Des",	GetWidth() -  96, 0, 32, GetHeight(), 'D', BtnDeselect);
	NewButton("Walk",	GetWidth() -  64, 0, 32, GetHeight(), 'W', BtnWalkmap, NULL, NULL, FALSE, TRUE);
	NewButton("Scr",	GetWidth() -  32, 0, 32, GetHeight(), 'L', BtnScroll);

	Button(0)->SetState(TRUE);
	Button(1)->SetState(TRUE);
	Button(2)->SetState(TRUE);

	SetDirty(TRUE);
	canundo = FALSE;

	// Terrain template controller
	TerrainTemplates = new TTerrainTemplate;

	return TRUE;
}

void TEditStatusPane::Close()
{
	TButtonPane::Close();

	if (TerrainTemplates)
		delete TerrainTemplates;

	TerrainTemplates = NULL;
}

void TEditStatusPane::DrawBackground()
{
	TButtonPane::DrawBackground();

	if (IsDirty())
	{
		SetDirty(FALSE);
		SColor color;
		color.red = color.green = 10; color.blue = 50;
		WORD c = TranslateColor(color);
		Display->Box(112, 0, GetWidth() - 200 - 112, GetHeight(), c, 0xffff, 0, DM_BACKGROUND);

		if (curobj >= 0)
		{
			PTObjectInstance oi = MapPane.GetInstance(selected[curobj]);
			if (!oi)
				StatusBar.Validate();
			else
			{
				S3DPoint pos;
				oi->GetPos(pos);

				sprintf(buf, "\"%s\" %s : %s (%d, %d, %d)", oi->GetName(),
						TObjectClass::GetClass(oi->ObjClass())->ClassName(),
						oi->GetTypeName(), pos.x, pos.y, pos.z);
				Display->WriteText(buf, 270, 0, 1, SystemFont, NULL, DM_USEDEFAULT, -1, 0, JUSTIFY_CENTER);
			}
		}
		else
		{
			S3DPoint pos;
			MapPane.GetMapPos(pos);
			sprintf(buf, "Level %d - (%d, %d, %d)", MapPane.GetMapLevel(), pos.x, pos.y, pos.z);
			Display->WriteText(buf, 270, 0, 1, SystemFont, NULL, DM_USEDEFAULT, -1, 0, JUSTIFY_CENTER);
		}
	}
}

void TEditStatusPane::InvertAxisButtons()
{
	Button(1)->Invert();
	Button(2)->Invert();
	Button(3)->Invert();
}

BOOL TEditStatusPane::Select(int index, BOOL add)
{
	if (index < 0 || (add && numobjs >= MAXSELECTEDOBJS))
		return FALSE;

	canundo = FALSE;

	// first make sure we're not duplicating a selection
	for (int i = 0; i < numobjs; i++)
		if (selected[i] == index)
		{
			// just set it to be the current object
			SetCurObj(i);
			ClassPane.SelectSame(index);
			return TRUE;
		}

	if (!add)
	{
		// remove the selection highlights for the objects we are deselecting
		for (int i = 0; i < numobjs; i++)
			MapPane.AddObjectUpdateRect(selected[i]);

		numobjs = 0;
	}

	curobj = numobjs++;
	selected[curobj] = index;
	SetDirty(TRUE);
	ClassPane.SelectSame(index);
	MapPane.AddObjectUpdateRect(index);

	return TRUE;
}

BOOL TEditStatusPane::Deselect()
{
	if (curobj < 0)
		return FALSE;

	StopMoving();			// clear any drag flags that might be set

	if (MapPane.GetInstance(selected[curobj]))
		MapPane.AddObjectUpdateRect(selected[curobj]);

	memmove(&(selected[curobj]), &(selected[curobj+1]), (numobjs - curobj - 1) * sizeof(int));
	numobjs--;
	if (curobj >= numobjs)
		curobj--;
	SetDirty(TRUE);
	return TRUE;
}

BOOL TEditStatusPane::Deselect(int index)
{
	canundo = FALSE;

	for (int i = 0; i < numobjs; i++)
		if (selected[i] == index)
		{
			MapPane.AddObjectUpdateRect(selected[i]);
			memmove(&(selected[i]), &(selected[i+1]), (numobjs - i - 1) * sizeof(int));
			numobjs--;
			if (i <= curobj)
				curobj--;
			SetDirty(TRUE);
			return TRUE;
		}

	return FALSE;
}

BOOL TEditStatusPane::Reselect(int oldindex, int newindex)
{
	canundo = FALSE;

	for (int i = 0; i < numobjs; i++)
		if (selected[i] == oldindex)
		{
			selected[i] = newindex;
			return TRUE;
		}

	return FALSE;
}

BOOL TEditStatusPane::SetCurObj(int newcurobj)
{
	if ((DWORD)(newcurobj >= numobjs))
		return FALSE;

	curobj = newcurobj;
	SetDirty(TRUE);
	return TRUE;
}

BOOL TEditStatusPane::Next()
{
	if (curobj >= (numobjs - 1))
		return FALSE;

	curobj++;
	SetDirty(TRUE);
	return TRUE;
}

BOOL TEditStatusPane::Prev()
{
	if (curobj <= 0)
		return FALSE;

	curobj--;
	SetDirty(TRUE);
	return TRUE;
}

void TEditStatusPane::Validate()
{
	for (int i = 0; i < numobjs; i++)
		if (MapPane.GetInstance(selected[i]) == NULL)
			Deselect(selected[i]);
}

void TEditStatusPane::Delete()
{
	canundo = FALSE;

	while (numobjs > 0)
	{
		PTObjectInstance oi = MapPane.GetInstance(selected[--numobjs]);
		if (!oi)
			Console.Output("\nERROR: Index is invalid\n");
		else
			MapPane.DeleteObject(oi);
	}
	curobj = -1;
	SetDirty(TRUE);
}

void StartObjMoving(PTObjectInstance oi)
{
	if (oi && !(oi->GetFlags() & OF_EDITORLOCK))
	{
		S3DPoint pos;
		oi->GetPos(pos);
		if (!(oi->GetFlags() & OF_SELDRAW))
		{
			MapPane.AddObjectUpdateRect(oi->GetMapIndex());
			oi->SetFlags(OF_SELDRAW);
		}
	}
}

void TEditStatusPane::StartMoving()
{
	for (int i = 0; i < numobjs; i++)
	{
		canundo = TRUE;
		PTObjectInstance oi = MapPane.GetInstance(selected[i]);

		if (oi)
		{
			oi->GetPos(lastpos[i]);
			StartObjMoving(oi);

			if (oi->GetShadow() >= 0)
			{
				PTObjectInstance s = MapPane.GetInstance(oi->GetShadow());
				StartObjMoving(s);
			}
		}
	}
}

void StopObjMoving(PTObjectInstance oi)
{
	if (oi && (oi->GetFlags() & OF_SELDRAW))
	{
		oi->ResetFlags(oi->GetFlags() & (~OF_SELDRAW));
		if (!(oi->GetFlags() & OF_MOVING))
			MapPane.AddObjectUpdateRect(oi->GetMapIndex());
	}
}

void TEditStatusPane::StopMoving()
{
	for (int i = 0; i < numobjs; i++)
	{
		PTObjectInstance oi = MapPane.GetInstance(selected[i]);

		if (oi)
		{
			StopObjMoving(oi);

			if (oi->GetShadow() >= 0)
			{
				PTObjectInstance s = MapPane.GetInstance(oi->GetShadow());
				StopObjMoving(s);
			}
		}
	}

	SetDirty(TRUE);
}

void TEditStatusPane::Undo()
{
	for (int i = 0; i < numobjs; i++)
	{
		PTObjectInstance oi = MapPane.GetInstance(selected[i]);
		oi->SetPos(lastpos[i]);
	}
}

// ******************
// * TEditToolsPane *
// ******************

// Button functions for the edit tools pane
void BtnFullScr()
{
	Output("\nCurrently disabled\n");
	Output(PROMPT);
	/*
	if (FullScreen)
		ShutDownFullScreen();
	else
		StartFullScreen();
	*/
}

void BtnSave()
{
	Console.Input("save\n");
}

void BtnReveal()
{
	Console.Input("reveal\n");
}

void BtnStat()
{
	Console.Input("stat\n");
}

void BtnZOff()
{
	PTObjectInstance oi = MapPane.GetInstance(StatusBar.GetSelectedObj());
	if (oi)
	{
		StatusBar.StartMoving();
		Console.Output("zoffset ");
		Console.ChainMouse(ZOffset, -1, -1, -1, -80, 400, oi->GetImagery()->GetRegZ(oi->GetState()));
	}
}

int startx, starty;
S3DPoint startpos;

void Registration(S3DPoint pos)
{
	int dx, dy;

	PTObjectInstance inst = MapPane.GetInstance(StatusBar.GetSelectedObj());
	if (inst)
	{
		inst->GetPos(pos);
		WorldToScreen(pos, dx, dy);
		dx -= startx;
		dy -= starty;

		int x, y, z;
		x = inst->GetImagery()->GetRegX(inst->GetState());
		y = inst->GetImagery()->GetRegY(inst->GetState());
		z = inst->GetImagery()->GetRegZ(inst->GetState());

		inst->GetImagery()->SetReg(inst->GetState(), x - dx, y - dy, z);
		inst->SetPos(startpos);
	}

	StatusBar.StopMoving();
	Console.ClearAxis();
}

void RegistrationAbort()
{
	PTObjectInstance inst = MapPane.GetInstance(StatusBar.GetSelectedObj());
	if (inst)
		inst->SetPos(startpos);

	StatusBar.StopMoving();
	Console.ClearAxis();
}

void BtnReg()
{
	PTObjectInstance inst = MapPane.GetInstance(StatusBar.GetSelectedObj());
	if (inst)
	{
		inst->GetPos(startpos);				// save for later comparisson
		WorldToScreen(startpos, startx, starty);

		if (!Console.AddAxis(inst))
			return;

		StatusBar.StartMoving();
		MapPane.SetMode(Registration, RegistrationAbort, MODE_MOVE);
	}
}

void BtnDel()
{
	StatusBar.Delete();
}

void BtnAmb()
{
}

void BtnAmbCol()
{
}

void BtnMono()
{
}

void Clone(S3DPoint pos)
{
	StatusBar.StopMoving();

	for (int i = StatusBar.GetFirstObj(); i >= 0; i = StatusBar.GetNextObj())
	{
		PTObjectInstance inst = MapPane.GetInstance(i);
		if (!inst)
			Output("ERROR: Aquiring instance in clone\n");
		else
		{
			SObjectDef def;
			memset(&def, 0, sizeof(SObjectDef));
			def.objclass = inst->ObjClass();
			def.objtype = inst->ObjType();
			def.flags = inst->GetFlags();
			def.level = inst->GetLevel();
			inst->GetPos(def.pos);
			def.facing = inst->GetFace();
			def.state = inst->GetState();

			int index = MapPane.NewObject(&def);
			if (index < 0)
				Output("ERROR: Cloning object\n");
			else
			{
				// copy light stuff
				PTObjectInstance newinst = MapPane.GetInstance(index);
				if (newinst)
				{
					newinst->SetLightIntensity(inst->GetLightIntensity());
					S3DPoint pos;
					inst->GetLightPos(pos);
					newinst->SetLightPos(pos);
					SColor color;
					inst->GetLightColor(color);
					newinst->SetLightColor(color);
					newinst->ResetLightFlags(inst->GetLightFlags());
					newinst->SetLightMultiplier(inst->GetLightMultiplier());
				}

				StatusBar.Reselect(i, index);
			}

		}
	}

	StatusBar.StartMoving();
}

void CloneAbort()
{
	StatusBar.Delete();
}

void BtnClone()
{
	if (StatusBar.GetSelectedObj() >= 0)
	{
		S3DPoint dummy;
		Clone(dummy);
		MapPane.SetMode(Clone, CloneAbort, MODE_CLONE);
	}
}

void BtnFollow()
{
	Console.Input("follow\n");
}

void BtnCalc()
{
	MapPane.CalculateWalkmap();
}

void BtnSavWM()
{
}

void BtnBound()
{
	Console.SetBounds();
}

void AnimRegistration(int x, int y)
{
	PTObjectInstance inst = MapPane.GetInstance(StatusBar.GetSelectedObj());
	if (inst && inst->GetImagery())
	{
		int z = inst->GetImagery()->GetAnimRegZ(inst->GetState());
		inst->GetImagery()->SetAnimReg(inst->GetState(), x, y, z);
	}
}

void AnimZ(int z, int dummy = -1)
{
	PTObjectInstance inst = MapPane.GetInstance(StatusBar.GetSelectedObj());
	if (inst && inst->GetImagery())
	{
		int x = inst->GetImagery()->GetAnimRegX(inst->GetState());
		int y = inst->GetImagery()->GetAnimRegY(inst->GetState());
		inst->GetImagery()->SetAnimReg(inst->GetState(), x, y, z);
	}
}

void BtnAnReg()
{
	PTObjectInstance inst = MapPane.GetInstance(StatusBar.GetSelectedObj());
	if (inst && inst->GetImagery())
	{
		Console.Output("animreg ");
		Console.ChainMouse(AnimRegistration, -320, 320, inst->GetImagery()->GetAnimRegX(inst->GetState()),
							-320, 320, inst->GetImagery()->GetAnimRegY(inst->GetState()));
	}
}

void BtnAnimZ()
{
	PTObjectInstance inst = MapPane.GetInstance(StatusBar.GetSelectedObj());
	if (inst && inst->GetImagery())
	{
		Console.Output("animz ");
		Console.ChainMouse(AnimZ, -1, -1, -1, 0, 480, inst->GetImagery()->GetAnimRegZ(inst->GetState()));
	}
}

void BtnTemplate()
{
	Console.Input("template\n");
}

#define TOOLBUTTONWIDTH		36
#define TOOLBUTTONHEIGHT	20

struct { char *name; void (*btnfunc)(); int key; } ToolList[] =
{
	{ "Full",	BtnFullScr,	0			},
	{ "Save",	BtnSave,	'S'			},
	{ "Clone",	BtnClone,	VK_INSERT	},
	{ "Stat",	BtnStat,	'T'			},
	{ "Amb",	BtnAmb,		0			},
	{ "Amb C",	BtnAmbCol,	0			},
	{ "Mono",	BtnMono,	0			},
	{ "Del",	BtnDel,		VK_DELETE	},
	{ "Folow",	BtnFollow,	'F'			},
	{ "Revl",	BtnReveal,	0			},
	{ "Templ",	BtnTemplate,0,			},
	{ "Z Off",	BtnZOff,	0			},
	{ "Reg",	BtnReg,		0			},
	{ "Calc",	BtnCalc,	0			},
	{ "SavWM",	BtnSavWM,	0			},
	{ "Bound",	BtnBound,	0			},
	{ "AnReg",	BtnAnReg,	0			},
	{ "AnimZ",	BtnAnimZ,	0			},
	{ NULL,		NULL,		0			}			// end of list marker
};

BOOL TEditToolsPane::Initialize()
{
	TButtonPane::Initialize();

	int i = 0;

	for (int y = 0; y < GetHeight(); y += TOOLBUTTONHEIGHT)
		for (int x = 0; x < GetWidth(); x += TOOLBUTTONWIDTH)
		{
			NewButton(ToolList[i].name, x, y, TOOLBUTTONWIDTH, TOOLBUTTONHEIGHT,
					  ToolList[i].key, ToolList[i].btnfunc);

			if (ToolList[++i].name == NULL)
				return TRUE;
		}

	return (ToolList[i].name == NULL);
}

void TEditToolsPane::DrawBackground()
{
	if (IsDirty())
	{
		Display->Box(0, 0, GetWidth(), GetHeight(), 0, 0xffff, 0, DM_BACKGROUND);
		SetDirty(FALSE);
	}

	TButtonPane::DrawBackground();
}

void TEditToolsPane::KeyPress(int key, BOOL down)
{
	TButtonPane::KeyPress(key, down);

	if (down)
	{
		key -= VK_F1;
		if (key >= 0 && key <= NUMBOOKMARKS)
		{
			if (CtrlDown)
			{
				MapPane.GetMapPos(bookmark[key]);
				bookmarklev[key] = MapPane.GetMapLevel();
			}
			else
			{
				if (bookmark[key].x >= 0 || bookmark[key].y >= 0)
				{
					MapPane.SetMapPos(bookmark[key]);
					MapPane.SetMapLevel(bookmarklev[key]);
					StatusBar.SetDirty(TRUE);
				}
			}
		}
	}
}

void TEditToolsPane::AddBookmark(int mark, S3DPoint pos, int lev)
{
	if (mark < 0 || mark >= NUMBOOKMARKS)
		return;

	memcpy(&bookmark[mark], &pos, sizeof(S3DPoint));
	bookmarklev[mark] = lev;
}

void TEditToolsPane::GetBookmark(int mark, RS3DPoint pos, int &lev)
{
	if (mark < 0 || mark >= NUMBOOKMARKS)
		return;

	memcpy(&pos, &bookmark[mark], sizeof(S3DPoint));
	lev = bookmarklev[mark];
}

void TEditToolsPane::ClearBookmarks()
{
	for (int i = 0; i < NUMBOOKMARKS; i++)
		bookmark[i].x = bookmark[i].y = bookmark[i].z = bookmarklev[i] = -1;
}

// ******************
// * TEditClassPane *
// ******************

void BtnMaximize()
{
	static BOOL maximized = FALSE;

	if (maximized)
	{
		Console.Show();
		ToolBar.Show();
		ClassPane.Resize(EDMULTIPANEX, ClassPane.GetPosY(),
			Display->Width() - CONSOLEX - EDMULTIPANEX, ClassPane.GetHeight());
	}
	else
	{
		Console.Hide();
		ToolBar.Hide();
		ClassPane.Resize(CONSOLEX, ClassPane.GetPosY(), 
			Display->Width() - CONSOLEX - CONSOLEX, ClassPane.GetHeight());
	}

	int i = 0;
	for ( ; i < 5; i++)
		ClassPane.Button(i)->SetPosX(ClassPane.GetWidth() - 16);
	for ( ; i < 13; i++)
		ClassPane.Button(i)->SetPosX(ClassPane.GetWidth() - 32);

	ClassPane.Update();

	maximized = !maximized;
}

void BtnScrollUp()
{
	ClassPane.Scroll(-(ClassPane.GetMaxLines()));
}

void BtnScrollDown()
{
	ClassPane.Scroll(ClassPane.GetMaxLines());
}

void BtnTileClass()
{
	ClassPane.SelClass(OBJCLASS_TILE);
}

void BtnCharacterClass()
{
	ClassPane.SelClass(OBJCLASS_CHARACTER);
}

void BtnWeaponClass()
{
	ClassPane.SelClass(OBJCLASS_WEAPON);
}

void BtnArmorClass()
{
	ClassPane.SelClass(OBJCLASS_ARMOR);
}

void BtnTalismanClass()
{
	ClassPane.SelClass(OBJCLASS_TALISMAN);
}

void BtnFoodClass()
{
	ClassPane.SelClass(OBJCLASS_FOOD);
}

void BtnLightSourceClass()
{
	ClassPane.SelClass(OBJCLASS_LIGHTSOURCE);
}

void BtnToolClass()
{
	ClassPane.SelClass(OBJCLASS_TOOL);
}

void BtnContainerClass()
{
	ClassPane.SelClass(OBJCLASS_CONTAINER);
}

void BtnExitClass()
{
	ClassPane.SelClass(OBJCLASS_EXIT);
}

void ObjDownCallback(S3DPoint pos)
{
	ClassPane.PutObject(pos);
}

void ObjDownAbortCallback()
{
	StatusBar.Delete();
}

BOOL TEditClassPane::Initialize()
{
	TButtonPane::Initialize();

	NewButton("T", GetWidth() - 16, 28, 16, 16, 0, BtnTileClass);
	NewButton("C", GetWidth() - 16, 44, 16, 16, 0, BtnCharacterClass);
	NewButton("W", GetWidth() - 16, 60, 16, 16, 0, BtnWeaponClass);
	NewButton("A", GetWidth() - 16, 76, 16, 16, 0, BtnArmorClass);
	NewButton("T", GetWidth() - 16, 92, 16, 16, 0, BtnTalismanClass);

	NewButton("F", GetWidth() - 32, 28, 16, 16, 0, BtnFoodClass);
	NewButton("L", GetWidth() - 32, 44, 16, 16, 0, BtnLightSourceClass);
	NewButton("T", GetWidth() - 32, 60, 16, 16, 0, BtnToolClass);
	NewButton("C", GetWidth() - 32, 76, 16, 16, 0, BtnContainerClass);
	NewButton("E", GetWidth() - 32, 92, 16, 16, 0, BtnExitClass);

	NewButton("=", GetWidth() - 32,  0, 32, 12, VK_F9, BtnMaximize);
	NewButton("/\\", GetWidth()-32, 12, 32, 16, VK_PRIOR, BtnScrollUp);
	NewButton("\\/", GetWidth()-32,108, 32, 16, VK_NEXT, BtnScrollDown);

	curobj = -1;
	curclass = OBJCLASS_TILE;
	firstobj = 0;
	maxlines = GetHeight() / THUMBNAILHEIGHT;
	accumulator = 0;
	thumbnails = NULL;
	clicked = FALSE;

	SetDirty(TRUE);
	return TRUE;
}

void TEditClassPane::Close()
{
	TButtonPane::Close();

	PSBitmapList nextptr;
	for ( ; thumbnails; thumbnails = nextptr)
	{
		nextptr = thumbnails->next;
		if (thumbnails->bm)
			delete thumbnails->bm;

		delete thumbnails;
	}
}

#define OBJWIDTH	(143 - 32)
#define OBJHEIGHT	(THUMBNAILHEIGHT)

void TEditClassPane::DrawBackground()
{
	TButtonPane::DrawBackground();

	if (IsDirty())
	{
		Display->SetClipRect(GetPosX(), GetPosY(), GetWidth() - 32, GetHeight());
		Display->Box(0, 0, GetWidth() - 32, GetHeight(), 0, 0xffff, 0, DM_BACKGROUND);

		PTObjectClass cl = TObjectClass::GetClass(curclass);
		if (cl)
		{
			int col = 0, lines = 0;
			for (int o = firstobj; o < cl->NumTypes() && lines < maxlines; o++)
			{
				PSObjectInfo inf = cl->GetObjType(o);
				if (!inf)
					continue;

				int x = col * OBJWIDTH, y = lines * OBJHEIGHT;

				if (o == curobj)
				{
					SColor color;
					color.red = color.green = 20; color.blue = 120;
					Display->Box(x, y, OBJWIDTH, OBJHEIGHT, TranslateColor(color), 0xffff, 0, DM_BACKGROUND);
				}

				PSImageryEntry ie = TObjectImagery::GetImageryEntry(inf->imageryid);

				if (ie)
				{
					PTBitmap tn = GetThumbnail(ie->filename);
					if (tn)
						Display->Put(x, y, tn, DM_TRANSPARENT | DM_BACKGROUND);
				}

				Display->WriteText(inf->name, x+THUMBNAILWIDTH+2, y);

				col++;
				if ((col * OBJWIDTH) > (GetWidth() - 32 - OBJWIDTH))
				{
					lines++;
					col = 0;
				}
			}
		}

		SetDirty(FALSE);
	}
}

void TEditClassPane::MouseClick(int button, int x, int y)
{
	TButtonPane::MouseClick(button, x, y);

	if (x >= (GetWidth() - 32))
		return;

	if (button == MB_LEFTDOWN)
	{
		curobj = ((y / OBJHEIGHT) * ((GetWidth() - 32) / OBJWIDTH)) + (x / OBJWIDTH) + firstobj;
		clicked = TRUE;
		PlayScreen.SetExclusivePane(&ClassPane);
		SetDirty(TRUE);
	}
	else if (button == MB_LEFTUP && clicked)
	{
		if (InPane(x, y))
		{
			curobj = ((y / OBJHEIGHT) * ((GetWidth() - 32) / OBJWIDTH)) + (x / OBJWIDTH) + firstobj;
			S3DPoint center;
			MapPane.GetMapPos(center);
			PutObject(center);
			MapPane.SetMode(&ObjDownCallback, &ObjDownAbortCallback, MODE_CLONE);
		}
		PlayScreen.ReleaseExclusivePane(&ClassPane);
		clicked = FALSE;
		scrollrate = 0;
	}
}

void TEditClassPane::MouseMove(int button, int x, int y)
{
	if (button == MB_LEFTDOWN && clicked)
	{
		if (y <= 0)
			scrollrate = -3;
		else if (y >= (GetHeight() - (GetHeight() % OBJHEIGHT) - 1))
			scrollrate = 3;
		else
		{
			scrollrate = 0;
			int oldcurobj = curobj;
			curobj = ((y / OBJHEIGHT) * ((GetWidth() - 32) / OBJWIDTH)) + (x / OBJWIDTH) + firstobj;
			if (curobj != oldcurobj)
				SetDirty(TRUE);
		}
	}
}

void TEditClassPane::Animate(BOOL draw)
{
	if (clicked && scrollrate)
	{
		if (accumulator++ >= absval(scrollrate))
		{
			accumulator = 0;
			Scroll(scrollrate > 0 ? 1 : -1);

			curobj = curobj % ((GetWidth() - 32) / OBJWIDTH);
			if (scrollrate < 0)
				curobj = firstobj + curobj;
			else
				curobj = firstobj + ((maxlines - 1) * ((GetWidth() - 32) / OBJWIDTH)) + curobj;
		}
	}
}

PTBitmap TEditClassPane::GetThumbnail(char *fname)
{
	strcpy(buf, TObjectImagery::GetImageryPath());

	if (strncmp(fname, "IMAGERY", 7) == 0)
		sprintf(buf, "%sthumbnail.%s", buf, fname + 8);
	else
	{
		int j = strlen(buf);
		while (*fname != '.' && *fname)
			buf[j++] = *fname++;
		buf[j++] = '.';
		buf[j++] = 't';
		buf[j++] = 'n';
		buf[j++] = 0;
	}

	PSBitmapList i, prev = NULL;
	for (i = thumbnails; i; prev = i, i = i->next)
		if (stricmp(buf, i->filename) == 0)
			return i->bm;

	// not found - load it in from disk
	PSBitmapList tn = new SBitmapList;
	strcpy(tn->filename, buf);

	//if (ImageryBox.BoxOpen)
	/*
	if (0)
	{
		int fh = ImageryBox.GetFileHandle(tn->filename + 9);

		if (fh != -1)
		{
			tn->bm = TBitmap::NewBitmap(THUMBNAILWIDTH, THUMBNAILHEIGHT, BM_8BIT | BM_PALETTE);
			read(fh, tn->bm->palette, 256 * sizeof(WORD));
			read(fh, tn->bm->data8, tn->bm->width * tn->bm->height);
			ConvertPal15to16(tn->bm);
		}
		else
			tn->bm = NULL;			// make a blank entry
	}
	else*/
	{
		char filename[MAX_PATH];

		sprintf(filename, "%s%s", ResourcePath, tn->filename);

		FILE *fp = fopen(filename, "rb");
		if (fp)
		{
			tn->bm = TBitmap::NewBitmap(THUMBNAILWIDTH, THUMBNAILHEIGHT, BM_8BIT | BM_PALETTE);
			fread(tn->bm->palette, 256, sizeof(WORD), fp);
			fread(tn->bm->data8, tn->bm->width * tn->bm->height, sizeof(BYTE), fp);
			ConvertPal15to16(tn->bm);
			fclose(fp);
		}
		else
			tn->bm = NULL;			// make a blank entry
	}

	tn->next = i;
	if (prev)
		prev->next = tn;
	else
		thumbnails = tn;

	return tn->bm;
}

int TEditClassPane::PutObject(S3DPoint pos)
{
	SObjectDef def;
	memset(&def, 0, sizeof(SObjectDef));

	def.objclass = curclass;
	def.objtype = curobj;
	def.level = MapPane.GetMapLevel();
	def.pos = pos;

	int index = MapPane.NewObject(&def);
	if (index < 0)
	{
		Console.Output("ERROR: Creating object\n");
		return -1;
	}

	StatusBar.Select(index);
	PTObjectInstance oi = MapPane.GetInstance(index);

	return index;
}

void TEditClassPane::Scroll(int lines)
{
	firstobj += (GetWidth() / OBJWIDTH) * lines;
	if (firstobj < 0)
		firstobj = 0;

	SetDirty(TRUE);
}


void TEditClassPane::SelectSame(int index)
{
	PTObjectInstance inst = MapPane.GetInstance(index);
	if (inst && (curclass != inst->ObjClass() || curobj != inst->ObjType()))
	{
		curclass = inst->ObjClass();
		curobj = inst->ObjType();
		if (firstobj > curobj || firstobj <= (curobj - maxlines))
		{
			firstobj = curobj - (maxlines / 2);
			if (firstobj < 0)
				firstobj = 0;
		}

		SetDirty(TRUE);
	}
}


//==============================================================================
//    Function : UpdateCGSFile.
//------------------------------------------------------------------------------
// Description : This open the appropriate CGS file (as specified by 'dir') and
//               see if the 'type' Imagery entry is already in the file.  If it
//               is, the entry will be replaced with the new 'graphicname',
//               keeping the flags intact.  If it is not in the file, a new
//               entry will be added to the end of the file.
//
//  Parameters : dir         = Name of the CGS file (and directory where the
//                             imagery is to be stored).
//
//               type        = Name of the type being added.
//
//               graphicname = Name of the graphic file used for the type.
//
//     Returns : If successful, returns TRUE, otherwise, returns FALSE.
//
//==============================================================================

BOOL UpdateCGSFile(char *dir, char *type, char *graphicname)
{
	char string[256];
	char cgsname[MAX_PATH];
	char cgsnewname[MAX_PATH];
	char cgsbakname[MAX_PATH];
	char *buf;
	char *l;
	char *p;
	int depth = 0;
	int len;
	long size;
	FILE *fp;
	BOOL done = FALSE;
	BOOL found = FALSE;

	// Determine which master CGS file to update
	sprintf(cgsname, "%s%s.CGS", ExileRCPath, dir);

	buf = (char *)LoadFile(cgsname, (void *)NULL, &size);

	// Create the New Name
	strcpy(cgsnewname, cgsname);
	p = strchr(cgsnewname, '.');
	if (p != NULL)
		*p = 0;
	strcat(cgsnewname, ".New");

	// Create the BAK Name
	strcpy(cgsbakname, cgsname);
	p = strchr(cgsbakname, '.');
	if (p != NULL)
		*p = 0;
	strcat(cgsbakname, ".Bak");

	// Try to open the file
	fp = TryOpen(cgsnewname, "w+b");

	// If the file didn't open, then return with failure
	if (fp == NULL)
		return FALSE;

	// Create the Imagery string that we're looking for
	sprintf(string, "IMAGERY \"%s\\%s.I2D\"", dir, type);
	len = strlen(string);

	l = buf;

	do
	{
		// See if we found IMAGERY "dir\name" at the beginning of the line
		if ((*(DWORD *)l == *(DWORD *)string) && (!strnicmp(l, string, len)))
		{
			found = TRUE;

			// Find the end of the line
			while ((*l != 10) && (*l != 13) && (l - buf < size))
				l++;
			// Move to the next line
			while (((*l == 10) || (*l == 13)) && (l - buf < size))
				l++;

			// Write everything up to and including the imagery name
			fwrite(buf, 1, l - buf, fp);

			do
			{
				p = l;

				// Skip any white space at the beginning of a line
				while (((*p == 9) || (*p == 32)) && (p - buf < size))
					p++;

				// Check for BEGIN, END, & NEXT Tokens
				if (!strnicmp(p, "BEGIN", 5))
				{
					depth++;
				}
				else
				if (!strnicmp(p, "END", 3))
				{
					depth--;
					if (depth <= 0)
						done = TRUE;
				}
				else
				if (!strnicmp(p, "NEXT", 4))
				{
				}
				else
				{
					// p probably points to a BITMAP, INVENTORY, or similar
					// token, so find the filename after the token
					while ((*p != '\"') && (p - buf < size))
						p++;
					p++;

					// Write out the line up until the file name
					fwrite(l, 1, p - l, fp);

					// Write out the new file name
					fwrite(graphicname, 1, strlen(graphicname), fp);

					// find the closing quote
					while ((*p != '\"') && (p - buf < size))
						p++;

					// Reset l to p so the code below will write out the
					// rest of line
					l = p;
				}

				// Find the end of the line
				while ((*p != 10) && (*p != 13) && (p - buf < size))
					p++;
				// Move to the next line
				while (((*p == 10) || (*p == 13)) && (p - buf < size))
					p++;

				// Write out the last line
				fwrite(l, 1, p - l, fp);

				l = p;
			} while (!done);

			// Write everything after the END of the imagery statements
			fwrite(l, 1, size - (l - buf), fp);

		}
		else
		{
			// Find the end of the line
			while ((*l != 10) && (*l != 13) && (l - buf < size))
				l++;
			// Move to the next line
			while (((*l == 10) || (*l == 13)) && (l - buf < size))
				l++;
			if (l - buf >= size)
				done = TRUE;
		}

	} while (!done);

	// If we didn't find the imagery entry, add it to the end of the CGS
	if (!found)
	{
		// Write the existing CGS file
		fwrite(buf, 1, size, fp);

		// Add on the new entry
		size = strlen(MakeRCImagery);
		fwrite(MakeRCImagery, 1, size, fp);

		sprintf(string, "\"%s\\%s.I2D\"\r\n", dir, type);
		size = strlen(string);
		fwrite(string, 1, size, fp);

		size = strlen(MakeRCBegin);
		fwrite(MakeRCBegin, 1, size, fp);

		size = strlen(MakeRCBitmap);
		fwrite(MakeRCBitmap, 1, size, fp);

		sprintf(string, "\"%s\" ", graphicname);
		size = strlen(string);
		fwrite(string, 1, size, fp);

		size = strlen(MakeRCFlags);
		fwrite(MakeRCFlags, 1, size, fp);

		size = strlen(MakeRCEnd);
		fwrite(MakeRCEnd, 1, size, fp);
	}

	fclose(fp);

	free(buf);

	// Delete the CGS.Bak file (if it exists)
	if (TryDelete(cgsbakname) == FALSE)
		return FALSE;

	// Rename the CGS file to the BAK file
	if (TryRename(cgsname, cgsbakname) == FALSE)
		return FALSE;

	// Rename the NEW file to CGS file
	if (TryRename(cgsnewname, cgsname) == FALSE)
		return FALSE;

	return TRUE;
}


//==============================================================================
//    Function : CmdAddRC.
//------------------------------------------------------------------------------
// Description : This will prompt the user for all the information needed to
//               add a new resource to the game. It then calls the resource
//               compiler to generate the new resource and adds it to the
//               appropriate object class.
//
//==============================================================================

#define MAX_NEW_RESOURCES 50
COMMAND(CmdAddRC)
{
	SNewRC rclist[MAX_NEW_RESOURCES];
	char name[MAX_PATH];
	char tmpcgsname[MAX_PATH];
	char string[256];
    char *p;
	int numrcs = 0;
	long size;
	FILE *fp;
	BOOL done = FALSE;
	BOOL proceed;
	PTObjectClass cl;

	// Create the temporary CGS file
	sprintf(tmpcgsname, "%sTmpRC%c%c%c.CGS", ExileRCPath, random(65, 90), random(65, 90), random(65, 90));
	if ((fp = fopen(tmpcgsname, "w+b")) == NULL)
		return 0;

	size = strlen(MakeRCInclude);
	fwrite(MakeRCInclude, 1, size, fp);

	fclose(fp);


	// -------------------------------------------------------------------------
	//                     Get the class that they are adding to
	// -------------------------------------------------------------------------

	do
	{
		Output("Which Class are you adding to ?\n");
		Output("[1]  Item         [2]  Weapon    [3]  Armor\n");
		Output("[4]  Talisman     [5]  Food      [6]  Container\n");
		Output("[7]  Light Source [8]  Tool      [9]  Money\n");
		Output("[10] Tile         [11] Exit      [12] Player\n");
		Output("[13] Character    [14] Trap      [15] Shadow\n");
		Output("[16] Helper       [17] Fireball  [18] Ice\n");
		Output("[19] Freeze       [20] Lightning [21] Hole\n");
		Output("[22] Ammo         [23] Scroll    [24] Ranged Weapon\n");
		Output("[25] Effect\n");
		Output("> ");
		if (TConsolePane::GetLine(name, MAX_PATH) == FALSE)
			return 0;
		// If they didn't enter anything, return immediately
		if (!name[0])
			return 0;
		rclist[numrcs].objclass = atoi(name);
	} while((rclist[numrcs].objclass < 1) || (rclist[numrcs].objclass > 25));
	rclist[numrcs].objclass--;



	// See if they are inputing a Tile
	if (rclist[numrcs].objclass == 9)
	{
		// ---------------------------------------------------------------------
		//               Get which tile set they are insert into
		// ---------------------------------------------------------------------

		do
		{
			Output("\nWhat Tile Set does it belong to ?\n");
			Output("[1]  Misc       [2]  Cave\n");
			Output("[3]  Dungeon    [4]  Forest\n");
			Output("[5]  Keep       [6]  Keep Interior\n");
			Output("[7]  Labyrinth  [8]  Ruin\n");
			Output("[9]  Swamp      [10] Wasteland\n");
			Output("[11] Town       [12] Town Interior\n");
			Output("> ");
			if (TConsolePane::GetLine(name, MAX_PATH) == FALSE)
				return 0;
			// If they didn't enter anything, return immediately
			if (!name[0])
				return 0;
			rclist[numrcs].set = atoi(name);
		} while((rclist[numrcs].set < 1) || (rclist[numrcs].set > 12));
		rclist[numrcs].set--;

        strcpy(rclist[numrcs].dir, RCTileDirs[rclist[numrcs].set]);
	}
	else
	{
		rclist[numrcs].set = 0;
        strcpy(rclist[numrcs].dir, RCClassDirs[rclist[numrcs].objclass]);
	}


	do
	{
		// Set the Object Class & Tile Set for the new resource so they are the
		// same as the first resource added
		rclist[numrcs].objclass = rclist[0].objclass;
		rclist[numrcs].set = rclist[0].set;
		strcpy(rclist[numrcs].dir, rclist[0].dir);


		// ---------------------------------------------------------------------
		//               Get the name of the object for the editor
		// ---------------------------------------------------------------------

		Output("\nName of Object Type (ie. WallSW) ?\n");
		Output("> ");
		if (TConsolePane::GetLine(rclist[numrcs].type, MAXNAMELEN) == FALSE)
			return 0;
		// If they didn't enter anything, return immediately
		if (!rclist[numrcs].type[0])
			return 0;

		// Add the tile set prefix (if any) to the beginning of the name
		if (rclist[numrcs].set)
		{
            sprintf(name, "%s%s", RCPrefixes[rclist[numrcs].set], rclist[numrcs].type);
			strcpy(rclist[numrcs].type, name);
		}


		// ---------------------------------------------------------------------
		//           See if that type name already exists in the class
		// ---------------------------------------------------------------------

		proceed = TRUE;
		cl = TObjectClass::GetClass(rclist[numrcs].objclass);

		if (cl)
		{
			if (cl->FindObjType(rclist[numrcs].type) != -1)
			{
				do
				{
					Output("\nThis Object Type already exists.\n");
					Output("Do you wish to overwrite it (Y/N) ?\n");
					Output("> ");

					if (TConsolePane::GetLine(string, 256) == FALSE)
						return 0;
					// If they didn't enter anything, return immediately
					if (!string[0])
						return 0;

					string[0] = toupper(string[0]);
				} while ((*string != 'Y') && (*string != 'N'));

				if (*string == 'N')
					proceed = FALSE;
			}
		}


		// Only proceed if the Object type wasn't in the class or it was in
		// the class and they want to overwrite it
		if (proceed)
		{
            proceed = FALSE;

            do
            {
                // -------------------------------------------------------------
                //        Get which file they are using for the imagery
                // -------------------------------------------------------------

                Output("\nName of the Graphic file (ie. File.BMP) ?\n");
                Output("> ");
                if (TConsolePane::GetLine(rclist[numrcs].graphic, MAXNAMELEN) == FALSE)
                    return 0;
                // If they didn't enter anything, return immediately
                if (!rclist[numrcs].graphic[0])
                    return 0;

                // Default to .BMP if there is no extension
                if (strchr(rclist[numrcs].graphic, '.') == NULL)
                    strcat(rclist[numrcs].graphic, ".BMP");

                // Add the directory to the beginning of the filename
                sprintf(name, "%s\\%s", rclist[numrcs].dir, rclist[numrcs].graphic);
                strcpy(rclist[numrcs].graphic, name);


                // -------------------------------------------------------------
                //       See if the files exists in the source directory
                // -------------------------------------------------------------

                // Check for the Graphic file
                sprintf(string, "%s%s", ExileRCPath, rclist[numrcs].graphic);
                if (!FileExists(string))
                {
                    do
                    {
                        Output("\nThe specified file name does not exist.\n");
                        Output("Do you wish to re-enter it (Y/N) ?\n");
                        Output("> ");

                        if (TConsolePane::GetLine(string, 256) == FALSE)
                            return 0;
                        // If they didn't enter anything, return immediately
                        if (!string[0])
                            return 0;

                        string[0] = toupper(string[0]);
                    } while ((*string != 'Y') && (*string != 'N'));

                    if (*string == 'N')
                        break;
                }
                else
                {
                    // Check for the Z Buffer file
                    strcpy(name, string);
                    p = strchr(name, '.');
                    strcpy(p, ".ZBF");
                    p = strchr(string, '.');
                    strcpy(p, ".ZPIC");

                    if ((FileExists(name)) || (FileExists(string)))
                        proceed = TRUE;
                    else
                    {
                        do
                        {
                            Output("\nCannot find the matching Z buffer file.\n");
                            Output("Do you wish to re-enter it (Y/N) ?\n");
                            Output("> ");

                            if (TConsolePane::GetLine(string, 256) == FALSE)
                                return 0;
                            // If they didn't enter anything, return immediately
                            if (!string[0])
                                return 0;

                            string[0] = toupper(string[0]);
                        } while ((*string != 'Y') && (*string != 'N'));

                        if (*string == 'N')
                            break;
                    }
                }
            } while (!proceed);


            if (proceed)
            {
                // -------------------------------------------------------------
                //                  Write the TempRC CGS file
                // -------------------------------------------------------------

                if ((fp = fopen(tmpcgsname, "ab")) == NULL)
                    return 0;

                size = strlen(MakeRCImagery);
                fwrite(MakeRCImagery, 1, size, fp);

                sprintf(string, "\"%s\\%s.I2D\"\r\n", rclist[numrcs].dir, rclist[numrcs].type);
                size = strlen(string);
                fwrite(string, 1, size, fp);

                size = strlen(MakeRCBegin);
                fwrite(MakeRCBegin, 1, size, fp);

                size = strlen(MakeRCBitmap);
                fwrite(MakeRCBitmap, 1, size, fp);

                sprintf(string, "\"%s\" ", rclist[numrcs].graphic);
                size = strlen(string);
                fwrite(string, 1, size, fp);

                size = strlen(MakeRCFlags);
                fwrite(MakeRCFlags, 1, size, fp);

                size = strlen(MakeRCEnd);
                fwrite(MakeRCEnd, 1, size, fp);

                fclose(fp);
            }
		}


        // Only increment the number of resources added if they didn't abort out above
        if (proceed)
            numrcs++;

		// ---------------------------------------------------------------------
		//                See if they want to add another resource
		// ---------------------------------------------------------------------

        if (numrcs < MAX_NEW_RESOURCES)
		{
			do
			{
				Output("\nDo you want to add another resource (Y/N) ?\n");
				Output("> ");

				if (TConsolePane::GetLine(string, 256) == FALSE)
					return 0;
				// If they didn't enter anything, return immediately
				if (!string[0])
					return 0;

				string[0] = toupper(string[0]);
			} while ((*string != 'Y') && (*string != 'N'));

			if (*string == 'Y')
				Output("\n");
			else
				done = TRUE;
		}
		else
			done = TRUE;

	} while (!done);




	// ---------------------------------------------------------------------
	//                      Compile the TempRC CGS file
	// ---------------------------------------------------------------------

    if (numrcs)
    {
        Output("Compiling resource...");

        STARTUPINFO StartUpInfo;
        PROCESS_INFORMATION ProcessInfo;

        // Setup the StartUpInfo struct
        memset(&StartUpInfo, 0, sizeof(STARTUPINFO));
        StartUpInfo.cb = sizeof(STARTUPINFO);
        StartUpInfo.dwFlags = STARTF_USESHOWWINDOW;
        StartUpInfo.wShowWindow = SW_HIDE;

        // Create the command line
        sprintf(string, "%sExileRC.EXE %s %s%s normals false",
            ExileRCPath, tmpcgsname, ResourcePath, NONORMALPATH);

        // Run ExileRC to create the new resource
        CreateProcess(
            NULL,
            string,
            NULL,
            NULL,
            FALSE,
            HIGH_PRIORITY_CLASS,
            NULL,
            ExileRCPath,
            &StartUpInfo,
            &ProcessInfo);

        // Wait for ExileRC to finish
        WaitForSingleObject(ProcessInfo.hProcess, INFINITE);

        // Delete the TempRC.CGS file
        remove(tmpcgsname);

        Output("Done.\n");
    }


	// -------------------------------------------------------------------------
	//                     Add the new types to their classes
	// -------------------------------------------------------------------------

    for (int n = 0; n < numrcs; n++)
	{
		// Update the appropriate master CGS file
		UpdateCGSFile(rclist[n].dir, rclist[n].type, rclist[n].graphic);

		cl = TObjectClass::GetClass(rclist[n].objclass);

		if (cl)
		{
			// See if the Object Type already exists
			int objtype = cl->FindObjType(rclist[n].type);
			if (objtype != -1)
            {
				// Object Type already exists, so just change the imagery file name

				// Create the filename of the imagery to use
				sprintf(name, "%s\\%s.I2D", rclist[n].dir, rclist[n].type);

                // Rename the imagery file name to the new name
				TObjectImagery::RenameImageryFile(cl->GetObjType(objtype)->imageryid, name);

				for (int i = 0; i < cl->NumStats(); i++)
					cl->ResetStat(objtype, i);

				sprintf(buf, "Type %s updated in class %s.\n", rclist[n].type, cl->ClassName());
				Output(buf);
            }
            else
            {
                // Create the filename of the imagery to use
                sprintf(name, "%s\\%s.I2D", rclist[n].dir, rclist[n].type);

                objtype = cl->AddType(rclist[n].type, name, 0);
                if (objtype < 0)
                    Output("ERROR: Adding object type\n");
                else
                {
                    for (int i = 0; i < cl->NumStats(); i++)
                        cl->ResetStat(objtype, i);
                    sprintf(buf, "New type %s added to class %s.\n", rclist[n].type, cl->ClassName());
                    Output(buf);
                }
            }
		}
	}

	return 0;
}


//==============================================================================
//    Function : CmdDeleteRC.
//------------------------------------------------------------------------------
// Description : This will prompt the user for all the information needed to
//               delete a resource from the game.
//
//==============================================================================

COMMAND(CmdDeleteRC)
{
	char name[MAX_PATH];
    char imageryfilename[MAX_PATH];
	char string[256];
    char *p;
    int classnum;
    int objtype;
	PTObjectClass cl;

	// -------------------------------------------------------------------------
    //                  Get the class that they are deleting from
	// -------------------------------------------------------------------------

	do
	{
        Output("Which Class are you deleting from ?\n");
		Output("[1]  Item         [2]  Weapon    [3]  Armor\n");
		Output("[4]  Talisman     [5]  Food      [6]  Container\n");
		Output("[7]  Light Source [8]  Tool      [9]  Money\n");
		Output("[10] Tile         [11] Exit      [12] Player\n");
		Output("[13] Character    [14] Trap      [15] Shadow\n");
		Output("[16] Helper       [17] Fireball  [18] Ice\n");
		Output("[19] Freeze       [20] Lightning [21] Hole\n");
		Output("[22] Ammo         [23] Scroll    [24] Ranged Weapon\n");
		Output("[25] Effect\n");
		Output("> ");
		if (TConsolePane::GetLine(name, MAX_PATH) == FALSE)
			return 0;
		// If they didn't enter anything, return immediately
		if (!name[0])
			return 0;
        classnum = atoi(name);
    } while((classnum < 1) || (classnum > 25));
    classnum--;
    cl = TObjectClass::GetClass(classnum);

    if (cl)
    {
        // ---------------------------------------------------------------------
        //            Get the name of the resource they are deleting
        // ---------------------------------------------------------------------

        BOOL done = FALSE;
        do
        {
            Output("Enter the name of the resource to delete ?\n");
            Output("> ");
            if (TConsolePane::GetLine(name, MAX_PATH) == FALSE)
                return 0;
            // If they didn't enter anything, return immediately
            if (!name[0])
                return 0;

            // See if the resource exists
            objtype = cl->FindObjType(name);
            if (objtype == -1)
            {
                do
                {
                    Output("\nThe specified resource does not exist.\n");
                    Output("Do you wish to re-enter it (Y/N) ?\n");
                    Output("> ");

                    if (TConsolePane::GetLine(string, 256) == FALSE)
                        return 0;
                    // If they didn't enter anything, return immediately
                    if (!string[0])
                        return 0;

                    string[0] = toupper(string[0]);
                } while ((*string != 'Y') && (*string != 'N'));

                if (*string == 'N')
                    return 0;
            }
            else
                done = TRUE;
        } while (!done);


        // Get the imagery file name so we can remove it later
        strcpy(imageryfilename, cl->GetObjType(objtype)->imagery->GetResFilename());

        cl->RemoveType(objtype);

        // ---------------------------------------------------------------------
        //                       Remove the imagery files
        // ---------------------------------------------------------------------

        // See if they are deleting from the Tile class
        if (classnum == 9)
        {
            // Find which tile set they are deleting from
            name[0] = 0;
            for (int n = 0; n < 12; n++)
                if (!strnicmp(imageryfilename, RCPrefixes[n], 3))
                    sprintf(name, "%s%s\\%s", ResourcePath, RCTileDirs[n], imageryfilename);
            if (name[0] == 0)
            {
                Output("\nCould not determine graphics path.\n");
                Output("\nImagery files were not deleted.\n");
                return 0;
            }
        }
        else
            sprintf(name, "%s%s\\%s", ResourcePath, RCClassDirs[classnum], imageryfilename);

        // Add the I2D extension if there wasn't one
        if (strchr(name, '.') == NULL)
            strcat(name, ".I2D");
        remove(name);

        // Now remove the thumbnail
        p = strchr(name, '.');
        *p = 0;
        strcat(name, ".TN");
        remove(name);

    }

	return 0;
}


//==============================================================================
//    Function : CmdDeleteRC.
//------------------------------------------------------------------------------
// Description : This function saves the imagery for a tile into a BMP and ZBF 
//				 file.
//==============================================================================

COMMAND(CmdSaveTileBM)
{
	char filename[FILENAMELEN];
	char exists[FILENAMELEN];
	char buf[80];

	if (!context)
		return 0;

	char *objname = context->GetTypeName();

	for (int rctype = 0; rctype < NUMRCDIRS; rctype++)
	{
		if (!strnicmp(objname, RCPrefixes[rctype], 3))
			break;
	}
	if (rctype >= NUMRCDIRS)
		rctype = 0;

	sprintf(filename, "%s%s\\%s", ExileRCPath, RCTileDirs[rctype], context->GetTypeName());

  // Does file exist?
	strcpy(exists, filename);
	strcat(exists, ".BMP");
	FILE *f = fopen(exists, "rb");
	if (f)
	{
		fclose(f);
		sprintf(buf, "%s already exists.. Overwrite (Y/N)\n"
		        "> ", exists);
		Output(buf);
		char string[256];
        if (TConsolePane::GetLine(string, 256) == FALSE)
            return 0;
		if (!(string[0] == 'y' || string[0] == 'Y'))
			return 0;
	}

	if (context->GetImagery()->SaveBitmap(filename, 0, TRUE))
		sprintf(buf, "%s.BMP successfully saved.\n", filename);
	else
		sprintf(buf, "Error writing %s.BMP!\n", filename);

	Output(buf);

	return 0;
}
