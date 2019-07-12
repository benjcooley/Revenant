// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      revtypes.h - System Types                        *
// *************************************************************************

#ifndef _REVTYPES_H
#define _REVTYPES_H

#include "string.h"

#ifdef min
#undef min
#undef max
#endif

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define abs(a)      (((a) < 0) ? -(a) : (a))

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned size_t;
#endif

/*
 * Virtual Keys, Standard Set
 */
#ifndef VK_RETURN

#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_CANCEL         0x03
#define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */
#define VK_BACK           0x08
#define VK_TAB            0x09
#define VK_CLEAR          0x0C
#define VK_RETURN         0x0D
#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_MENU           0x12
#define VK_PAUSE          0x13
#define VK_CAPITAL        0x14
#define VK_ESCAPE         0x1B
#define VK_SPACE          0x20
#define VK_PRIOR          0x21
#define VK_NEXT           0x22
#define VK_END            0x23
#define VK_HOME           0x24
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_PRINT          0x2A
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E
#define VK_HELP           0x2F
#define VK_LWIN           0x5B
#define VK_RWIN           0x5C
#define VK_APPS           0x5D
#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_MULTIPLY       0x6A
#define VK_ADD            0x6B
#define VK_SEPARATOR      0x6C
#define VK_SUBTRACT       0x6D
#define VK_DECIMAL        0x6E
#define VK_DIVIDE         0x6F
#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F5             0x74
#define VK_F6             0x75
#define VK_F7             0x76
#define VK_F8             0x77
#define VK_F9             0x78
#define VK_F10            0x79
#define VK_F11            0x7A
#define VK_F12            0x7B
#define VK_F13            0x7C
#define VK_F14            0x7D
#define VK_F15            0x7E
#define VK_F16            0x7F
#define VK_F17            0x80
#define VK_F18            0x81
#define VK_F19            0x82
#define VK_F20            0x83
#define VK_F21            0x84
#define VK_F22            0x85
#define VK_F23            0x86
#define VK_F24            0x87
#define VK_NUMLOCK        0x90
#define VK_SCROLL         0x91
#define VK_LSHIFT         0xA0
#define VK_RSHIFT         0xA1
#define VK_LCONTROL       0xA2
#define VK_RCONTROL       0xA3
#define VK_LMENU          0xA4
#define VK_RMENU          0xA5
#define VK_PROCESSKEY     0xE5
#define VK_ATTN           0xF6
#define VK_CRSEL          0xF7
#define VK_EXSEL          0xF8
#define VK_EREOF          0xF9
#define VK_PLAY           0xFA
#define VK_ZOOM           0xFB
#define VK_NONAME         0xFC

#endif // Virtual key codes defined

#ifndef _WINDEF_

/******* Common definitions and typedefs ***********************************/

#define VOID                void

#define PASCAL              _pascal

#define CDECL               _cdecl

#define WINAPI              _far _pascal

#define CALLBACK            _far _pascal

/****** Simple types & common helper macros *********************************/

typedef int                 BOOL;
#define NULL                0
#define FALSE               0
#define TRUE                1

typedef unsigned char       BYTE;
typedef unsigned short      WORD;

typedef struct tagRGBTRIPLE {
        BYTE    rgbtBlue;
        BYTE    rgbtGreen;
        BYTE    rgbtRed;
} RGBTRIPLE;

typedef unsigned long       DWORD;

typedef unsigned int        UINT;

typedef signed long         LONG;

#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)((UINT)(w) >> 8))

#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)((DWORD)(l) >> 16))

#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))


/* Types use for passing & returning polymorphic values */
typedef UINT WPARAM;
typedef LONG LPARAM;
typedef LONG LRESULT;

#define MAKELPARAM(low, high)   ((LPARAM)MAKELONG(low, high))
#define MAKELRESULT(low, high)  ((LRESULT)MAKELONG(low, high))


/****** Handles and Such ****************************************************/

typedef UINT                    HANDLE;
typedef HANDLE                  HINSTANCE;
typedef HANDLE                  HWND;
typedef HANDLE                  HDC;
typedef HANDLE                  HBITMAP;

#endif

typedef unsigned __int64        QWORD;


//******************************** EXILE TYPES ******************************

// Offset variable points to location in current data block
class OFFSET
{
  public:
    OFFSET() {}
    void set(void *p)
      { if (p) offset = ((DWORD)p - (DWORD)this); else offset = 0; }
    void *ptr()
      { return offset ? (void *)((long)this + offset) : NULL; }
    OFFSET &operator = (unsigned long l)
      { offset = l; return *this; }
    OFFSET &operator = (OFFSET &o)
      { offset = o.offset; return *this; }
    operator void *()
      { return offset ? (void *)((long)this + offset) : NULL; }
    operator unsigned long()
      { return offset; }
    unsigned long offset;
};

template<class T>
class TOffset : public OFFSET
{
  public:
    operator T *()
      { return offset ? (T *)((long)this + offset) : NULL; }
    T *operator -> ()
      { return offset ? (T *)((long)this + offset) : NULL; }
    T &operator [] (int i) { return ((T *)((long)this + offset))[i]; }
    T *alloc(void* &p, int n = 1) { p = (void *)((unsigned long)p + 3 & 0xFFFFFFFC); set(p); p = (char *)p + sizeof(T) * n; return (T *)ptr(); }
    T *alloc(unsigned char* &p, int n = 1) { return alloc((void* &)p, n); }
    T *alloc(char* &p, int n = 1) { return alloc((void* &)p, n); }
};

template <class T>
class TIterator;

template <class T>
class TSizableIterator;

template <class T>
class TPointerIterator;

template <class T>
class TVirtualIterator;

// ********************************************************************************************
// * TArray - Statically allocated array type (just a small wrapper around an ordinary array) *
// ********************************************************************************************

template <class T, int size = 64>
class TArray
{
    friend class TIterator<T>;

    int numitems;       // Number of items in array
    T   items[size];    // Array

  public:

    TArray() { numitems = 0; }

    int Add(T &newitem)
    {
        int i = min(numitems, size - 1);
        T *item = &items[i];
        for (; i >= 0; i--, item--)
        {
            if (i >= numitems || !item->used)
            {
                memcpy(item, &newitem, sizeof(T));
                item->used = TRUE;
                if (i >= numitems)
                    numitems = i + 1;
                return i;
            }
        }

        return -1;
    }
      // Adds items
    int Set(T &item, int index)
    {
        if (index >= size)
            return -1;

        memcpy(&items[index], &item, sizeof(T));

        if (index >= numitems)
            numitems = index + 1;

        return index;
    }
      // Sets item
    int Set(T &item, TPointerIterator<T> i)
    {
        return Set(item, i.ItemNum());
    }
      // Sets item
    void Remove(int itemnum)
    {
        items[itemnum].used = FALSE;
        while (numitems > 0 && !items[numitems - 1].used)
            numitems--;
    }
      // Removes items
    void Remove(TIterator<T> i)
    {
        Remove(i.ItemNum());
    }
      // Removes items
    int NumItems() { return numitems; }
      // Returns current itemnum.
    int MaxItems() { return size; }
      // Returns size of array.
    void Clear() { numitems = 0; }
      // Clears all elements in array
    BOOL Used(int itemnum)
        { if ((DWORD)itemnum >= (DWORD)numitems) return FALSE; else return items[itemnum].used; }
      // Returns TRUE if array element used or FALSE if not
    T   *Get(int itemnum) { return &items[itemnum]; }
      // Returns specific item
    T   &operator [] (int itemnum) { return items[itemnum]; }
      // Returns address of specific item
};

template <class T>
class TIterator
{
    TArray<T> *array;
    T   *item;
    int itemnum;

  public:
    TIterator() { array = NULL; item = NULL; itemnum = 0; }
    TIterator(void *a) { SetArray(a); }
    void SetArray(void *a) { array = (TArray<T> *)a; item = &(array->items[0]); itemnum = 0; }
    T    *Item() { return item; }
      // Returns current item.
    int  ItemNum() { return itemnum; }
      // Returns current itemnum.
    BOOL Used() { return item->used; }
      // Returns if item is used or not
    BOOL operator ++ (int) { item++; itemnum++; return array && itemnum < array->numitems; }
      // Allows moving forwards through array in sequential order
    BOOL operator -- (int) { item--; itemnum--; return itemnum >= 0; }
      // Allows moving backwards through array in sequential order
    operator BOOL () { return array && (DWORD)itemnum < (DWORD)array->numitems; }
      // Returns FALSE if past end of array
};

// ************************************************************************
// * TSizableArray - Resizable array of objects (not pointers to objects) *
// ************************************************************************

// Great class for storing small simple objects (int's, DWORD's, etc.).  Objects are simply
// placed in an array allocated to store them.  All elements in the array below 'numitems'
// are presumed to be used.  Used Collapse to remove items from the list.  Array takes
// up 8 bytes when empty.  Items stored in this array should NOT have destructors and/or
// constructors.

template <class T, int defsize = 64, int grow = 64>
class TSizableArray
{
    friend class TSizableIterator<T>;

    short numitems; // Number of items in array
    short size;     // Size of the array
    T   *items;     // Array elements

    friend class T;

  public:

    TSizableArray() {
        numitems = 0;
        size = defsize;
        items = NULL;
        if (size > 0)
        {
            items = new T[size];
            memset(items, 0, sizeof(T) * size);
        }
     }
    TSizableArray(int arysize) {
        numitems = 0;
        size = arysize;
        items = NULL;
        if (size > 0)
        {
            items = new T[size];
            memset(items, 0, sizeof(T) * size);
        }
     }
    ~TSizableArray() { if (items) delete items; }

    void Grow(int newsize = -1)
    {
        if (newsize < 0)
            newsize = size + grow;
        else if (newsize < size)
            return;
        T *newitems = new T[newsize];
        memset(newitems + size, 0, sizeof(T) * (newsize - size));
        if (items)
        {
            memcpy(newitems, items, sizeof(T) * size);
            delete items;
        }
        items = newitems;
        size = newsize;
    }

    void SetNumItems(int newnumitems)
    {
        Grow(newnumitems);
        numitems = newnumitems;
    }

    int New()
    {
        if (numitems >= size)
            Grow();
        return numitems++;
    }

    int Add(T &newitem)
    {
        if (numitems >= size)
            Grow();
        items[numitems++] = newitem;
        return numitems - 1;
    }

    int Set(T &item, int index)
    {
        while ((DWORD)index >= (DWORD)size)
            Grow();

        items[index] = item;

        if (index >= numitems)
            numitems = index + 1;

        return index;
    }
      // Sets item
    int Set(T *item, TSizableIterator<T> i)
    {
        return Set(item, i.ItemNum());
    }
      // Remove item
    void Remove(int itemnum)
    {
        if ((DWORD)itemnum >= (DWORD)numitems)
            return;
        if (itemnum == numitems - 1)
            numitems--;
        if (numitems <= 0)
            Clear();
    }
      // Removes items
    void Remove(TSizableIterator<T> i)
    {
        Remove(i.ItemNum());
    }
      // Inserts an item
    int Insert(T &newitem, int itemnum)
    {
        if (numitems >= size)
            Grow();
        if ((DWORD)itemnum > (DWORD)numitems)
            itemnum = numitems;
        if (itemnum < numitems)
            memmove(&items[itemnum + 1], &items[itemnum], (numitems - itemnum) * sizeof(T));
        items[itemnum] = newitem;
        numitems++;
        return itemnum;
    }
      // Inserts an item
    int Insert(T &newitem, TVirtualIterator<T> i)
    {
        return Insert(newitem, i.ItemNum());
    }
      // Removes item and collapses array
    void Collapse(int itemnum)
    {
        if ((DWORD)itemnum >= (DWORD)numitems)
            return;
            
        for (int c = itemnum; c < size - 1; c++)
            items[c] = items[c + 1];
        numitems--;
        if (numitems <= 0)
            Clear();
    }
      // Removes item and collapses array
    void Collapse(TVirtualIterator<T> i)
    {
        Collapse(i.ItemNum());
    }
      // Number of items
    int NumItems() { return numitems; }
      // Returns current itemnum.
    int MaxItems() { return size; }
      // Returns size of array.
    void Clear() { if (items) delete items; items = NULL; size = numitems = 0; }
      // Clears all pointers in array
    BOOL Used(int itemnum) { return TRUE; }
      // Returns TRUE if array element used or FALSE if not
    T   &Get(int itemnum) { return items[itemnum]; }
      // Returns specific item
    T   &operator [] (int itemnum) { return items[itemnum]; }
      // Returns address of specific item
};

template <class T>
class TSizableIterator
{
    TSizableArray<T> *array;
    T   *item;
    int itemnum;

  public:
    TSizableIterator() { array = NULL; item = NULL; itemnum = 0; }
    TSizableIterator(void *a) { SetArray(a); }
    void SetArray(void *a) { array = (TSizableArray<T> *)a;
        item = &(array->items[0]); itemnum = 0; }
    T    *Item() { return item; }
      // Returns current item.
    BOOL Used() { return TRUE; }
      // Returns if item is used or not
    int  ItemNum() { return itemnum; }
      // Returns current itemnum.
    BOOL operator ++ (int) { item++; itemnum++; return array && itemnum < array->numitems; }
      // Allows moving forwards through array in sequential order
    BOOL operator -- (int) { item--; itemnum--; return itemnum >= 0; }
      // Allows moving backwards through array in sequential order
    operator BOOL () { return array && (DWORD)itemnum < (DWORD)array->numitems; }
      // Returns FALSE if past end of array
};

// *****************************************************
// * TPointerArray - Array of pointers of a given type *
// *****************************************************

template <class T, int defsize = 64, int defgrow = 64>
class TPointerArray
{
    friend class TPointerIterator<T>;

    int numitems;   // Number of items in array
    int size;       // Size of the array
    int grow;       // Size to grow array by when full
    T   **items;    // Array elements

    friend class T;

  public:

    TPointerArray() {
        numitems = 0;
        size = defsize;
        grow = defgrow;
        items = NULL;
        items = new T* [size];
        memset(items, 0, sizeof(T*) * size);
     }
    TPointerArray(int arysize, int grwsize) {
        numitems = 0;
        size = arysize;
        grow = grwsize;
        items = NULL;
        items = new T* [size];
        memset(items, 0, sizeof(T*) * size);
     }
    ~TPointerArray() { delete items; }

    void Grow()
    {
        T **newitems = new T* [size + grow];
        memcpy(newitems, items, sizeof(T*) * size);
        memset(newitems + size, 0, sizeof(T*) * grow);
        delete items;
        items = newitems;
        size += grow;
    }

    int Add(T *newitem)
    {
        if (numitems >= size)
            Grow();
        int i = min(numitems, size - 1);
        T **item = &items[i];
        for (; i >= 0; i--, item--)
        {
            if (i >= numitems || !*item)
            {
                *item = newitem;
                if (i >= numitems)
                    numitems = i + 1;
                return i;
            }
        }

        return -1;
    }

    int Set(T *item, int index)
    {
        while ((DWORD)index >= (DWORD)size)
            Grow();

        items[index] = item;

        if (index >= numitems)
            numitems = index + 1;

        return index;
    }
      // Sets item
    int Set(T *item, TPointerIterator<T> i)
    {
        return Set(item, i.ItemNum());
    }
      // Remove item
    void Remove(int itemnum)
    {
        if ((DWORD)itemnum >= (DWORD)numitems)
            return;
        items[itemnum] = NULL;
        while (numitems > 0 && !items[numitems - 1])
            numitems--;
    }
      // Removes items
    void Remove(TPointerIterator<T> i)
    {
        Remove(i.ItemNum());
    }
      // Delete item
    void Delete(int itemnum)
    {
        if ((DWORD)itemnum >= (DWORD)numitems)
            return;
        if (items[itemnum])
            delete items[itemnum];
        items[itemnum] = NULL;
        while (numitems > 0 && !items[numitems - 1])
            numitems--;
    }
      // Deletes items
    void Delete(TPointerIterator<T> i)
    {
        Delete(i.ItemNum());
    }
      // Delete's all items
    void DeleteAll()
    {
        T **item = items;
        for (int i = 0; i < numitems; i++, item++)
        {
            if (*item)
            {
                delete *item;
                *item = NULL;
            }
        }
        numitems = 0;
    }
      // Inserts an item
    int Insert(T *newitem, int itemnum)
    {
        if (numitems >= size)
            Grow();
        if ((DWORD)itemnum > (DWORD)numitems)
            itemnum = numitems;
        if (itemnum < numitems)
            memmove(&items[itemnum + 1], &items[itemnum], (numitems - itemnum) * sizeof(T*));
        items[itemnum] = newitem;
        numitems++;
        return itemnum;
    }
      // Inserts an item
    int Insert(T *newitem, TVirtualIterator<T> i)
    {
        return Insert(newitem, i.ItemNum());
    }
      // Removes item and collapses array
    void Collapse(int itemnum, BOOL del = TRUE)
    {
        if ((DWORD)itemnum >= (DWORD)numitems)
            return;
            
        if (del && items[itemnum])
            delete items[itemnum];
        for (int c = itemnum; c < size - 1; c++)
        {
            items[c] = items[c + 1];
        }
        items[size - 1] = NULL;
        numitems--;
    }
      // Removes item and collapses array
    void Collapse(TVirtualIterator<T> i, BOOL del = TRUE)
    {
        Collapse(i.ItemNum(), del);
    }
      // Number of items
    int NumItems() { return numitems; }
      // Returns current itemnum.
    int MaxItems() { return size; }
      // Returns size of array.
    void Clear() { numitems = 0; }
      // Clears all pointers in array
    BOOL Used(int itemnum)
        { if ((DWORD)itemnum >= (DWORD)numitems) return FALSE; else return items[itemnum] != NULL; }
      // Returns TRUE if array element used or FALSE if not
    T   *Get(int itemnum) { return items[itemnum]; }
      // Returns specific item
    T   *operator [] (int itemnum) { return items[itemnum]; }
      // Returns address of specific item
};

template <class T>
class TPointerIterator
{
    TPointerArray<T> *array;
    T   **item;
    int itemnum;

  public:
    TPointerIterator() { array = NULL; item = NULL; itemnum = 0; }
    TPointerIterator(void *a) { SetArray(a); }
    void SetArray(void *a) { array = (TPointerArray<T> *)a;
        item = &(array->items[0]); itemnum = 0; }
    T    *Item() { return *item; }
      // Returns current item.
    BOOL Used() { return *item != NULL; }
      // Returns if item is used or not
    int  ItemNum() { return itemnum; }
      // Returns current itemnum.
    BOOL operator ++ (int) { item++; itemnum++; return array && itemnum < array->numitems; }
      // Allows moving forwards through array in sequential order
    BOOL operator -- (int) { item--; itemnum--; return itemnum >= 0; }
      // Allows moving backwards through array in sequential order
    operator BOOL () { return array && (DWORD)itemnum < (DWORD)array->numitems; }
      // Returns FALSE if past end of array
};

// ******************************************************************************
// * TVirtualArray - Dynamically allocated array which acts like a static array *
// ******************************************************************************

template <class T, int defsize = 64, int defgrow = 64>
class TVirtualArray
{
    friend class TVirtualIterator<T>;

    int numitems;   // Number of items in array
    int size;       // Size of the array
    int grow;       // Size to grow array by when full
    T   **items;    // Array elements
    T   *empty;     // An empty item we can return if there is no item in an array slot

    friend class T;

  public:

    TVirtualArray() { 
        numitems = 0;
        size = defsize; 
        grow = defgrow;
        items = NULL;
        items = new T* [size]; 
        memset(items, 0, sizeof(T*) * size);
        empty = new T; 
        memset(empty, 0, sizeof(T));
        }
    TVirtualArray(int arysize, int grwsize) { 
        numitems = 0;
        size = arysize;
        grow = grwsize; 
        items = NULL;
        items = new T* [size]; 
        memset(items, 0, sizeof(T*) * size);
        empty = new T; 
        memset(empty, 0, sizeof(T));
        }
    ~TVirtualArray() { delete items; delete empty; }

    void Grow()
    {
        T **newitems = new T* [size + grow];
        memcpy(newitems, items, sizeof(T*) * size);
        memset(newitems + size, 0, sizeof(T*) * grow);
        delete items;
        items = newitems;
        size += grow;
    }
      // Adds an item to array by using pointer instead of copying data
      // (NOTE: object is deleted by array)
    int AddPtr(T *newitem)
    {
        if (numitems >= size)
            Grow();
        int i = min(numitems, size - 1);
        T **item = &items[i];
        for (; i >= 0; i--, item--)
        {
            if (i >= numitems || !*item)
            {
                *item = newitem;
                if (i >= numitems)
                    numitems = i + 1;
                return i;
            }
        }

        return -1;
    }
      // Creates a new entry in the array at end, or in middle if there are empty spaces
    int New() { return AddPtr(new T); }
      // Adds item to array (copies data)
    int Add(T &newitem) { return AddPtr((T*)memcpy(new T, &newitem, sizeof(T))); }
      // Sets item
    int Set(T &item, int index)
    {
        while ((DWORD)index >= (DWORD)size)
            Grow();

        if (!items[index])
            items[index] = new T;

        memcpy(items[index], &item, sizeof(T));

        if (index >= numitems)
            numitems = index + 1;

        return index;
    }
      // Sets item
    int Set(T &item, TVirtualIterator<T> i)
    {
        return Set(item, i.ItemNum());
    }
      // Remove item
    void Remove(int itemnum)
    {
        if ((DWORD)itemnum >= (DWORD)numitems)
            return;
            
        if (items[itemnum])
            delete items[itemnum];
        items[itemnum] = NULL;

        while (numitems > 0 && !items[numitems - 1])
            numitems--;
    }
      // Removes items
    void Remove(TVirtualIterator<T> i)
    {
        Remove(i.ItemNum());
    }
      // Inserts an item
    int Insert(T &newitem, int itemnum)
    {
        if (numitems >= size)
            Grow();
        if ((DWORD)itemnum > (DWORD)numitems)
            itemnum = numitems;
        if (itemnum < numitems)
            memmove(&items[itemnum + 1], &items[itemnum], (numitems - itemnum) * sizeof(T*));
        items[itemnum] = (T*)memcpy(new T, &newitem, sizeof(T));
        numitems++;
        return itemnum;
    }
      // Inserts an item
    int Insert(T &newitem, TVirtualIterator<T> i)
    {
        return Insert(newitem, i.ItemNum());
    }
      // Removes item and collapses array
    void Collapse(int itemnum)
    {
        if ((DWORD)itemnum >= (DWORD)numitems)
            return;
            
        if (items[itemnum])
            delete items[itemnum];
        for (int c = itemnum; c < size - 1; c++)
        {
            items[c] = items[c + 1];
        }
        items[size - 1] = NULL;
        numitems--;
    }
      // Removes item and collapses array
    void Collapse(TVirtualIterator<T> i)
    {
        Collapse(i.ItemNum());
    }
      // Number of items
    int NumItems() { return numitems; }
      // Returns current itemnum.
    int MaxItems() { return size; }
      // Returns size of array.
    void Clear()
    {
        T **item = items;
        for (int i = 0; i < numitems; i++, item++)
        {
            if (*item)
            {
                delete *item;
                *item = NULL;
            }
        }
        numitems = 0;
    }
      // Clear array
    BOOL Used(int itemnum)
        { if ((DWORD)itemnum >= (DWORD)numitems) return FALSE; else return items[itemnum] != NULL; }
      // Returns TRUE if array element used or FALSE if not
    T   &Get(int itemnum) { if (items[itemnum]) return *(items[itemnum]); else return *empty; }
      // Returns specific item
    T   &operator [] (int itemnum) { if (items[itemnum]) return *(items[itemnum]); else return *empty; }
      // Returns address of specific item
};

template <class T>
class TVirtualIterator
{
    TVirtualArray<T> *array;
    T   **item;
    int itemnum;

  public:
    TVirtualIterator() { array = NULL; item = NULL; itemnum = 0; }
    TVirtualIterator(void *a) { SetArray(a); }
    void SetArray(void *a) { array = (TVirtualArray<T> *)a;
        item = &(array->items[0]); itemnum = 0; }
    T    *Item() { return *item; }
      // Returns current item.
    BOOL Used() { return *item != NULL; }
      // Returns if item is used or not
    int  ItemNum() { return itemnum; }
      // Returns current itemnum.
    BOOL operator ++ (int) { item++; itemnum++; return array && itemnum < array->numitems; }
      // Allows moving forwards through array in sequential order
    BOOL operator -- (int) { item--; itemnum--; return itemnum >= 0; }
      // Allows moving backwards through array in sequential order
    operator BOOL () { return array && (DWORD)itemnum < (DWORD)array->numitems; }
      // Returns FALSE if past end of array
};

template <class T, int size = 64>
class TOffsetArray;

template <class T>
class OTOffsetArray;

template <class T>
class TOffsetIterator
{
    TOffsetArray<T> *array;
    T   *item;
    int itemnum;

  public:
    TOffsetIterator(TOffsetArray<T> &array);
      // Initializer for regular offset array
    TOffsetIterator(OTOffsetArray<T> &array);
      // Initializer for offset to an offset array
  public:
    T    *Item() { return item; }
      // Returns current item.
    int  ItemNum() { return itemnum; }
      // Returns current itemnum.
    BOOL operator ++ (int) { if (array && itemnum < array->numitems - 1)
        { item++; itemnum++; return TRUE; } else 
        { item = NULL; itemnum = array->numitems; return FALSE; } }
      // Allows moving forwards through array in sequential order
    BOOL operator -- (int) { if (array && itemnum > 0) 
         { item--; itemnum--; return TRUE; } else
         { item = NULL; itemnum = -1; return FALSE; } }
      // Allows moving backwards through array in sequential order
    operator T* () { return item; }
      // Returns current item
    T *operator -> () { return item; }
      // Returns member access 
    T &operator * () { return *item; }
      // Returns dereferenced item
};

template <class T, int size = 64>
class TOffsetArray
{
    int numitems;
    TOffset<T> items[size];

    TOffsetArray() { numitems = 0; }

    void Add(int)
    {
        if (numitems == size) return -1;
        TOffset<T> *item = items;
        for (int i = 0; i < size; i++, item++)
        {
            if (!(*item))
            {
                items->set(newitem);
                numitems++;
                return i;
            }
        }
        return -1;
    }
      // Adds items
    void Remove(int itemnum)
    {
        items[itemnum] = NULL;
        numitems--;
    }
      // Removes items
    void Remove(TOffsetIterator<T> *i)
    {
        Remove(i->ItemNum());
    }
      // Removes items
    int NumItems() { return numitems; }
      // Returns number of items
    T   *Get(int itemnum) { return items[itemnum]; }
      // Returns specific item
    T   &operator [] (int itemnum) { return items[itemnum]; }
      // Returns address of specific item
};

// Class type pointer and reference definition macro

#ifdef _CLASSDEF
#undef _PTRDEF
#undef _REFDEF
#undef _REFPTRDEF
#undef _PTRCONSTDEF
#undef _REFCONSTDEF
#undef _CLASSDEF
#endif

#define _PTRDEF(name) typedef name * P##name;
#define _REFDEF(name) typedef name & R##name;
#define _REFPTRDEF(name) typedef name * & RP##name;
#define _PTRCONSTDEF(name) typedef const name * PC##name;
#define _REFCONSTDEF(name) typedef const name & RC##name;
#define _OFFDEF(name) typedef TOffset<name> O##name;

#define _CLASSDEF(name) class name; \
    _PTRDEF(name) \
    _REFDEF(name) \
    _REFPTRDEF(name) \
    _PTRCONSTDEF(name) \
    _REFCONSTDEF(name) \
    _OFFDEF(name)

#define _STRUCTDEF(name) struct name; \
    _PTRDEF(name) \
    _REFDEF(name) \
    _OFFDEF(name)

// Name typedefs
typedef char FileName[13];
typedef char FNAMESTRING[81];

// Scalar type pointer & reference definitions
typedef int &           Rint;
typedef int *           Pint;
typedef void * &        RPvoid;

// Global Structure and class definitions.

_STRUCTDEF(SAnimationFrame)
_STRUCTDEF(SBattleBlock)
_STRUCTDEF(SColor)
_STRUCTDEF(SCharAttackData)
_STRUCTDEF(SCharAttackImpact)
_STRUCTDEF(SCharData)
_STRUCTDEF(SDialogLine)
_STRUCTDEF(SDrawBlock)
_STRUCTDEF(SDrawParam)
_STRUCTDEF(SObjectInfo)
_STRUCTDEF(SObjectState)
_STRUCTDEF(SPalette)
_STRUCTDEF(SStatistic)
_STRUCTDEF(SStatisticDef)

_CLASSDEF(T3DAnimator)
_CLASSDEF(T3DController)
_CLASSDEF(T3DImagery)
_CLASSDEF(T3DScene)
_CLASSDEF(T3dSurface)
_CLASSDEF(TActionBlock)
_CLASSDEF(TAnimation)
_CLASSDEF(TAnimSprite)
_CLASSDEF(TArea)
_CLASSDEF(TAreaManager)
_CLASSDEF(TAutoMap)
_CLASSDEF(TBattle)
_CLASSDEF(TBitmap)
_CLASSDEF(TBitmapData)
_CLASSDEF(TBitmapSurface)
_CLASSDEF(TButtonPane)
_CLASSDEF(TCharacter)
_CLASSDEF(TCharDataManager)
_CLASSDEF(TComplexObject)
_CLASSDEF(TControlMap)
_CLASSDEF(TDDSurface)
_CLASSDEF(TDialogList)
_CLASSDEF(TDialogPane)
_CLASSDEF(TDisplay)
_CLASSDEF(TEffect)
_CLASSDEF(TEquipPane)
_CLASSDEF(TFont)
_CLASSDEF(TGameState)
_CLASSDEF(THealthBar)
_CLASSDEF(TImageryBuilder)
_CLASSDEF(TInputStream)
_CLASSDEF(TInventory)
_CLASSDEF(TLogoScreen)
_CLASSDEF(TMainWindow)
_CLASSDEF(TMapPane)
_CLASSDEF(TSidePane)
_CLASSDEF(TSideTabsPane)
_CLASSDEF(TPlyrStatusBar)
_CLASSDEF(TBottomPane)
_CLASSDEF(TMosaicSurface)
_CLASSDEF(TMultiCtrlPane)
_CLASSDEF(TMultiSurface)
_CLASSDEF(TMulti)
_CLASSDEF(TObjectClass)
_CLASSDEF(SObjectDef)
_CLASSDEF(TObjectImagery)
_CLASSDEF(TObjectImageryData)
_CLASSDEF(TObjectInstance)
_CLASSDEF(TObjectBuilder)
_CLASSDEF(TOutputStream)
_CLASSDEF(TPane)
_CLASSDEF(TPlayer)
_CLASSDEF(TPlayerManager)
_CLASSDEF(TPlayScreen)
_CLASSDEF(TQuickSpellPane)
_CLASSDEF(TResource)
_CLASSDEF(TRules)
_CLASSDEF(TSaveGame)
_CLASSDEF(TScreen)
_CLASSDEF(TScript)
_CLASSDEF(TScriptManager)
_CLASSDEF(TSector)
_CLASSDEF(TSpellList)
_CLASSDEF(TSpellManager)
_CLASSDEF(TSpellPane)
_CLASSDEF(TSound)
_CLASSDEF(TSoundPlayer)
_CLASSDEF(TSprite)
_CLASSDEF(TSpriteList)
_CLASSDEF(TStatPane)
_CLASSDEF(TStaminaBar)
_CLASSDEF(TStream)
_CLASSDEF(TSurface)
_CLASSDEF(TChunkCache)
_CLASSDEF(TTextBar)
_CLASSDEF(TTimer)
_CLASSDEF(TToken)
_CLASSDEF(TVideoCapture)
_CLASSDEF(TWaveData)

#define absval(i)       ((i) > 0 ? (i) : -(i))
#define sqr(i)          ((i) * (i))
#define SQRDIST(i, j)   (sqr(absval((i).x - (j).x)) + sqr(absval((i).y - (j).y)) + \
                         sqr(absval((i).z - (j).z)))

_STRUCTDEF(SPoint)
struct SPoint
{
    int x, y;

    SPoint() {}
    SPoint(int newx, int newy)
        { x = newx; y = newy; }
    SPoint(RSPoint p)
        { x = p.x; y = p.y; }
    int operator==(RSPoint p)
        { return ((x == p.x) && (y == p.y));}
    SPoint &operator=(RSPoint p)
        { x = p.x; y = p.y; return *this; }
};

_STRUCTDEF(SRect)
struct SRect
{
    int left, top, right, bottom;

    SRect() {}
    SRect(int x1, int y1, int x2, int y2)
        { left = x1; top = y1; right = x2; bottom = y2; }
    SRect(SPoint p1, SPoint p2)
        { left = p1.x; top = p1.y; right = p2.x; bottom = p2.y; }
    SRect &operator=(RSRect r1)
        { left = r1.left; top = r1.top; right = r1.right; bottom = r1.bottom; return *this; }
    int In(SPoint p)
        { return p.x >= left && p.y >= top && p.x <= right && p.y <= bottom; }
    int In(int x, int y)
        { return x >= left && y >= top && x <= right && y <= bottom; }
    int In(RSRect r)
        { return left >= r.left && top >= r.top && right <= r.right && bottom <= r.bottom; }
    int Intersects(RSRect r)
        { return !(r.right < left || r.left > right || r.bottom < top || r.top > bottom); }
    void Fix() {
        int temp;
        if (right < left) { temp = left; left = right; right = temp; }
        if (bottom < top) { temp = top; top = bottom; bottom = temp; }
        }

    int x() { return left; }
    int y() { return top; } 
    int w() { return right - left + 1; }
    int h() { return bottom - top + 1; }
};

_STRUCTDEF(S3DPoint)
struct S3DPoint
{
    int x, y, z;

    S3DPoint() {}
    S3DPoint(int newx, int newy, int newz)
        { x = newx; y = newy; z = newz;}
    S3DPoint(RS3DPoint p)
        { x = p.x; y = p.y; z = p.z; }

    // assignment and arithmatic operators
    RS3DPoint operator=(RS3DPoint p)
        { x = p.x; y = p.y; z = p.z; return *this; }
    RS3DPoint operator+(RS3DPoint p)
        { return S3DPoint(x + p.x, y + p.y, z + p.z); }
    RS3DPoint operator+=(RS3DPoint p)
        { return S3DPoint(x += p.x, y += p.y, z += p.z); }
    RS3DPoint operator-(RS3DPoint p)
        { return S3DPoint(x - p.x, y - p.y, z - p.z); }
    RS3DPoint operator-=(RS3DPoint p)
        { return S3DPoint(x -= p.x, y -= p.y, z -= p.z); }
    RS3DPoint operator*(int m)
        { return S3DPoint(x * m, y * m, z * m); }
    RS3DPoint operator*=(int m)
        { return S3DPoint(x *= m, y *= m, z *= m); }
    RS3DPoint operator/(int m)
        { return S3DPoint(x / m, y / m, z / m); }
    RS3DPoint operator/=(int m)
        { return S3DPoint(x /= m, y /= m, z /= m); }
    RS3DPoint operator&=(DWORD m)
        { return S3DPoint(x &= m, y &= m, z &= m); }
    BOOL InRange(S3DPoint &pos, int dist)  // In range of other point 2D (x,y)
        { return absval(y - pos.y) <= dist && absval(x - pos.x) <= dist &&
          sqr(y - pos.y) + sqr(x - pos.x) <= sqr(dist) + sqr(dist); } 
    BOOL InRange3D(S3DPoint &pos, int dist) // In range of other point 3D
        { return absval(y - pos.y) <= dist && absval(x - pos.x) <= dist && absval(z - pos.z) <= dist &&
          sqr(y - pos.y) + sqr(x - pos.x) + sqr(z - pos.z) <= sqr(dist) + sqr(dist) + sqr(dist); } 

    // comparison operators
    int operator==(RS3DPoint p)
        { return ((x == p.x) && (y == p.y) && (z == p.z)); }
    int operator!=(RS3DPoint p)
        { return ((x != p.x) || (y != p.y) || (z != p.z)); }
    int operator>=(RS3DPoint p)
        { return x >= p.x && y >= p.y && z >= p.z; }
    int operator<=(RS3DPoint p)
        { return x <= p.x && y <= p.y && z <= p.z; }

};

_STRUCTDEF(S3DRect)
struct S3DRect
{
    S3DPoint beg, end;

    S3DRect() {}
    S3DRect(int x1, int y1, int z1, int x2, int y2, int z2)
        { beg.x = x1; beg.y = y1; beg.z = z1; end.x = x2; end.y = y2; end.z = z2; }
    S3DRect(S3DPoint p1, S3DPoint p2)
        { beg = p1; end = p2; }
    S3DRect &operator=(RS3DRect r1)
        { beg.x = r1.beg.x; beg.y = r1.beg.y; beg.z = r1.beg.z;
          end.x = r1.end.x; end.y = r1.end.y; end.z = r1.end.z; return *this; }
    int In(S3DPoint p)
        { return (p >= beg) && (p <= end); }
    int In(int x, int y, int z)
        { return x >= beg.x && y >= beg.y && z >= beg.z &&
                 x <= end.x && y <= end.y && z <= end.z; }
    int Intersects(RS3DRect r)
        { return !(end.x < r.beg.x || end.y < r.beg.y || end.z < r.beg.z ||
                   beg.x > r.end.x || beg.y > r.end.y || beg.z > r.end.z); }
    void Fix() {
      int temp;
      if (end.x < beg.x) { temp = end.x; end.x = beg.x; beg.x = temp; }
      if (end.y < beg.y) { temp = end.y; end.y = beg.y; beg.y = temp; }
      if (end.z < beg.z) { temp = end.z; end.z = beg.z; beg.z = temp; }
      }
};

// Super deluxe size of static array macro
#define sizearray(a) (sizeof(a) / sizeof(a[0]))

// Super tricky distance function
inline int dist(int x1, int y1, int x2, int y2)
  { int dx = abs(x1 - x2); int dy = abs(y1 - y2); return dx + dy - (min(dx, dy) >> 1); }

inline int dist(int x1, int y1, int z1, int x2, int y2, int z2)
  { return dist(0, z1, dist(x1, y1, x2, y2), z2); }

// Class Definitions
#define OFFARRAY(c) inline c &TOFFSET<c>::operator [] (int i)\
  { return ((c *)(((char *)this) + offset))[i]; }

#endif

