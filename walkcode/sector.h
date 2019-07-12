// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      sector.h - TSector object                        *
// *************************************************************************

#ifndef _SECTOR_H
#define _SECTOR_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

extern char sectorfilename[80];

// ***********************************************************
// * TSector - An independenly loadable area of the game map *
// ***********************************************************

// The TSector object represents a small portion of the game map which contains a list
// of objects on that portion of the map.  The game map is divided into x,y sectors and
// levels, and the name of the sector resource file indicates which level and/or
// x,y sector of the game the sector comes from.

_STRUCTDEF(SLabel)
struct SLabel
{
	BOOL used;
	BOOL isgroup;
	char label[NAMELEN];
	int index;
};

#define MAXWALKMAPS 		32
#define MAXLABELS			64
#define MAXSECTOROBJECTS    4096	// if you increase this number you MUST change MAKEINDEX in mappane.h to handle more bits!
#define MAXPRELOADRECTS		32

#define WALKMAPSIZE			((SECTORWIDTH >> WALKMAPSHIFT) * (SECTORHEIGHT >> WALKMAPSHIFT))

typedef TPointerArray<TObjectInstance, MAXSECTOROBJECTS> TObjectArray;
typedef TVirtualArray<int, 0, MAXSECTOROBJECTS> TObjSetArray;
typedef TObjectArray *PTObjectArray;
typedef TPointerIterator<TObjectInstance> TObjectIterator;

typedef TPointerArray<TSector, 64, 64> TSectorArray;

// Walk flags (info about the current walk grid!)
#define WALKFLAGBITS			2	// Number of walk flag bits

#define WALKFLAG_NOOBSTRUCT		1	// This walk grid does not obstruct line of sight	
#define WALKFLAG_WATER			2	// This walk grid is a water grid (puddle/pool)

#define WALKFLAGMASK	((1 << WALKFLAGBITS) - 1)	


// ***************
// * Object Sets *
// ***************
//
// All objects within a sector are stored in the 'object' array.  This is usually fine,
// for most applications (like drawing tiles, etc.), but can bog the system down when you
// are constantly iterating through massive object arrays to find players, moving objects
// for intersection, etc.  
//
// To make iteration through smaller sets of objects easier, the sector system has a set
// of funcitons which deal with object sets.  Object sets are arrays of indexes into the
// main object array for objects which match the criteria of a given set.  To save time, 
// a function can iterate through the objects in the object set, instead of all the objects
// in a map.

// Currently there is only one object set.  You can add new object sets by increasing this
// number, defining the name below, and expanding the InObjectSet() function to recognize
// whether an object is a member of a set.  That's all there is to it.

#define NUMOBJSETS     4

// Object find/search sets (using a set for a search/find command greatly reduces)
// the time to find the objects)
#define OBJSET_ALL	     0	// All objects
#define OBJSET_MOVING    1	// Moving objects
#define OBJSET_CHARACTER 2	// Player and character objects
#define OBJSET_LIGHTS	 3	// Light objects

_CLASSDEF(TSector)
class TSector
{
  public:
	TSector(int newlevel, int newsectorx, int newsectory);
	~TSector();

	void Clear();

  // Creates and loads a sector (uses preloaded sector if it can find one)
	static PTSector LoadSector(int newlevel, int newsectorx, int newsectory, BOOL preload = TRUE);
		// Loads the sector.. keeps file open so sector is locked
	static void CloseSector(PTSector sector);
		// Save and delete the sector (doesn't really delete it if sector is preload)

  // Load and save the sector (straight load.. don't use preloaded sector list)
	BOOL Load(BOOL lock = FALSE);
		// Loads the sector.. keeps file open so sector is locked
	void Save();
		// Saves the sector..

  // Sector
	int SectorLevel() { return level; }
	  // Returns sector level
	int SectorX() { return sectorx; }
	  // Returns sector x position
	int SectorY() { return sectory; }
	  // Returns sector y position
	void GetMaxScreenRect(RSRect r);
	  // Returns sector max screen rectangle (as determined by furthest tiles)
	void GetMaxMapRect(RSRect r);
	  // Returns sector max map rectangle

  // Object manipulation functions
	PTObjectInstance GetInstance(int item)
		{ return objects[item]; }
	int AddObject(PTObjectInstance oi, int item = -1);
	  // Adds an object to the sector
	int SetObject(PTObjectInstance oi, int item);
	  // Sets an object into the sector at the given item index
	PTObjectInstance RemoveObject(int item);
	  // Removes an object from the sector
	int NumItems() { return objects.NumItems(); }
	  // Returns number of items in sector array
	PTObjectArray ObjectArray()
	  { return &objects; }
	  // Returns pointer to the main object array

  // Object set functions
  // --------------------
  // Object sets allow super fast iteration through subsets of objects.  To add a new 
  // object set, add to NUMOBJSETS macro, add a test in InObjSet, and create a new OBJSET_xxx
  // macro.
  //
  // All MapPane object functions use object sets, and most (including the map iterator), 
  // allow you to pass the object set parameter to the various search functions.
 	BOOL InObjSet(PTObjectInstance oi, int objset)
		{ if (objset == OBJSET_MOVING) return oi->IsMoving();
		  else if (objset == OBJSET_CHARACTER) return oi->IsCharacter();
		  else if (objset == OBJSET_LIGHTS) return oi->IsLight();
		  else return TRUE; }
	int NumObjSetItems(int objset) { if (!objset) return objects.NumItems(); else return objsets[objset-1].NumItems(); }
	  // Returns number of moving objects (objects with OF_MOVING flag set)
	int GetObjSetIndex(int objset, int item)
		{ if (!objset) return item; else return (objsets[objset-1])[item]; }
	PTObjectInstance GetObjSetInstance(int objset, int item)
		{ if (!objset) return objects[item]; else return objects[(objsets[objset-1])[item]]; }

  // Walkmap stuff
    /*int ReturnWalkmap(int x, int y)
	  { return walkmap ? (walkmap[(y << (SECTORWSHIFT - WALKMAPSHIFT)) + x] >> WALKFLAGBITS) : 0; }
	  // Returns walkmap value given sector position
	DWORD ReturnWalkmapFlags(int x, int y)
	  { return walkmap ? walkmap[(y << (SECTORWSHIFT - WALKMAPSHIFT)) + x] & WALKFLAGMASK : 0; }
    void SetWalkmap(int x, int y, int height, DWORD flags = 0)
	  { if (walkmap) walkmap[(y << (SECTORWSHIFT - WALKMAPSHIFT)) + x] = (WORD)(height << WALKFLAGBITS) | (WORD)(flags & WALKFLAGMASK); }
	  // Sets walkmap at x, y to have height of height
    void WalkmapHandler(int mode, BYTE *walk, int zpos, int x, int y, int width, int length, int stride, BOOL override = FALSE);*/
	  // Handler for various walkmap manipulations (clips world wm pos to sector)

	// Walkmap stuff
    int ReturnWalkmap(int x, int y)
	  { return walkmap ? walkmap[(y << (SECTORWSHIFT - WALKMAPSHIFT)) + x] : 0; }
	  // Returns walkmap value given sector position
    void SetWalkmap(int x, int y, int height)
	  { if (walkmap) walkmap[(y << (SECTORWSHIFT - WALKMAPSHIFT)) + x] = (WORD)height; }
	  // Sets walkmap at x, y to have height of height
    void WalkmapHandler(int mode, BYTE *walk, int zpos, int x, int y, int width, int length, int stride, BOOL override = FALSE);
	  // Handler for various walkmap manipulations (clips world wm pos to sector)

  // Preloaded sector system
	static BOOL LoadPreloadSectors(int level, int numrects, SRect *rects);
	  // Preload sectors in this area
	static void ClearPreloadSectors();
	  // Clear preload sectors
	static PTSector FindPreloadSector(int level, int sectorx, int sectory);
	  // find a particular preloaded sector
	static int PreloadSectorLevel() { return preloadlevel; }
	  // Get preload sector level
	static void NumPreloadRects(SRect &r) { numpreloadrects; }
	  // Get preload sector rectangle
	static SRect &GetPreloadRect(int rectnum) { return preloadrects[rectnum]; }
	  // Get preload sector rectangle
	static BOOL InPreloadArea(RS3DPoint p, int level);
	  // Are we in the preload area?

  private:
	WORD *walkmap;
	int  level, sectorx, sectory;
	BOOL preloaded;
	char filename[FILENAMELEN];

	TObjectArray objects;
	TObjSetArray objsets[NUMOBJSETS];

	static TSectorArray preloads;
	static int preloadlevel;
	static int numpreloadrects;
	static SRect preloadrects[MAXPRELOADRECTS];
};

#endif

