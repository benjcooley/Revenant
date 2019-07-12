// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           chunkcache.h - Decompression Cache Include File             *
// *************************************************************************

#ifndef _CHUNKCACHE_H
#define _CHUNKCACHE_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

_CLASSDEF(TChunkCache)

class TChunkCache
{
  protected:
    int numchunks;

  // 8 bit
    void   **chunks;                // Pointer to chunks in buffer
    int    *id;                     // ID number of chunk
    int    *used;                   // Indicates if spot is taken and how many times accessed
    void   *chunkbuffer;            // Pointer to chunk buffer
    int    currentcycle;            // Current cycle count. Used to remove unused chunks

  // 16 bit
    void   **chunks16;              // Pointer to chunks in buffer
    int    *id16;                   // ID number of chunk
    int    *used16;                 // Indicates if spot is taken and how many times accessed
    void   *chunkbuffer16;          // Pointer to chunk buffer
    int    currentcycle16;          // Current cycle count. Used to remove unused chunks

  public:
    TChunkCache();
      // Constructor

    void AllocCache(int megabytes);
        // Allocates space for chunk cache in MB

    DWORD MemUsed();
        // Returns memory used by cache (for editor)

    void *AddChunk(void *chunk, DWORD type);
      // Adds chunk to 8 bit list, unless already present

    void *AddChunkZ(void *chunk, DWORD type);
      // Adds ZBuffer chunk to 16 bit list, unless already present

    void *AddChunk16(void *chunk, DWORD type);
      // Adds chunk to 16 bit list, unless already present

    BOOL RemoveChunk(int number);
      // Removes chunk from buffer

    BOOL RemoveChunk16(int number);
      // Removes chunk from buffer

    ~TChunkCache();
      // Destructor

//  protected:
    int ChunkDecompress(void *source, void *dest, DWORD type);
      // Decompresses chunks of a bitmap. Adds chunks to cache

    int ChunkDecompressZ(void *source, void *dest, DWORD type);
      // Decompresses ZBuffer chunks of a bitmap. Adds chunks to 16 bit cache

    int ChunkDecompress16(void *source, void *dest, DWORD type);
      // Decompresses chunks of a bitmap. Adds chunks to 16 bit cache
};

#endif