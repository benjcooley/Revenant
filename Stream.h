// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      stream.h - TStream object                        *
// *************************************************************************

#ifndef _STREAM_H
#define _STREAM_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

// *******************************************************
// * TInputStream - Stream object for loading and saving *
// *******************************************************

// The TInputStream object represents an INPUT file or buffer
// stream from which a sector or other streamed data is
// loaded.  The stream object is similar to a C++ stream
// object.

_CLASSDEF(TInputStream)
class TInputStream
{
  public:
    TInputStream(BYTE *buffer, int bufferlen) { buf = ptr = buffer; buflen = bufferlen; }

    RTInputStream operator >> (int &d) { d = *(int *)ptr; ptr += 4;  return *this; }
    RTInputStream operator >> (DWORD &d) { d = *(DWORD *)ptr; ptr += 4;  return *this; }
    RTInputStream operator >> (short &d) { d = *(short *)ptr; ptr += 2;  return *this; }
    RTInputStream operator >> (WORD &d) { d = *(WORD *)ptr; ptr += 2;  return *this; }
    RTInputStream operator >> (char &d) { d = *(char *)ptr; ptr += 1;  return *this; }
    RTInputStream operator >> (BYTE &d) { d = *(BYTE *)ptr; ptr += 1;  return *this; }
    RTInputStream operator >> (char *d);

    void *Buffer() { return buf; }
        // Returns pointer to buf
    int BufferSize() { return buflen; }
        // Returns size of buffer
    int DataSize() { return buflen; }
        // Returns size of data
    void Reset()
      { ptr = buf; }
        // Resets read positon
    int GetPos()
      { return (int)((DWORD)ptr - (DWORD)buf); }
        // Gets read position
    BOOL SetPos(int newpos)
      { if ((DWORD)newpos < (DWORD)buflen) { ptr = buf + newpos; return TRUE; } else return FALSE; }
        // Sets read position (if not past end of buffer)
    BOOL MovePos(int newpos)
      { return SetPos(GetPos() + newpos); }
        // Sets read position (if not past end of buffer)

  private:
    BYTE *buf, *ptr;                    // Pointers
    int buflen;                         // Sizes
};

_CLASSDEF(TOutputStream)
class TOutputStream
{
  public:
    TOutputStream(int startsize, int growsize);
    virtual ~TOutputStream();

    RTOutputStream operator << (int d) { *(int *)ptr = d; ptr += 4;  return *this; }
    RTOutputStream operator << (DWORD d) { *(DWORD *)ptr = d; ptr += 4;  return *this; }
    RTOutputStream operator << (short d) { *(short *)ptr = d; ptr += 2;  return *this; }
    RTOutputStream operator << (WORD d) { *(WORD *)ptr = d; ptr += 2;  return *this; }
    RTOutputStream operator << (char d) { *(char *)ptr = d; ptr += 1;  return *this; }
    RTOutputStream operator << (BYTE d) { *(BYTE *)ptr = d; ptr += 1;  return *this; }
    RTOutputStream operator << (char *d);

    void *Buffer() { return buf; }
        // Returns pointer to buf
    int BufferSize() { return bufsize; }
        // Returns size of buffer
    int DataSize() { return (int)ptr - (int)buf; }
        // Returns size of data
    void MakeFreeSpace(int freespace);
        // Makes sure there is at least this amount of free space at end of buffer
    void Reset()
      { ptr = buf; }
        // Resets write positon
    int GetPos()
      { return (int)((DWORD)ptr - (DWORD)buf); }
        // Gets read position
    BOOL SetPos(int newpos)
      { if ((DWORD)newpos < (DWORD)bufsize) { ptr = buf + newpos; return TRUE; } else return FALSE; }
        // Sets read position (if not past end of buffer)
    BOOL MovePos(int newpos)
      { return SetPos(GetPos() + newpos); }
        // Sets read position (if not past end of buffer)

  private:
    BYTE *buf, *ptr;          // Pointers
    int bufsize, growsize;    // Sizes
};

#endif

