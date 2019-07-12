// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 chunkcache.cpp - Chunk Cache Object                   *
// *************************************************************************

#include "revenant.h"
#include "bitmap.h"
#include "decompdata.h"
#include "chunkcache.h"

int TChunkCache::ChunkDecompress(void *source, void *dest, DWORD clear);
int TChunkCache::ChunkDecompressZ(void *source, void *dest, DWORD clear);

TChunkCache::TChunkCache()
{
    numchunks = 0;
    chunks = chunks16 = NULL;
    id = used = id16 = used16 = NULL;
    chunkbuffer = chunkbuffer16 = NULL;
}

void TChunkCache::AllocCache(int megabytes)
{
    numchunks = (megabytes * 1024 * 1024) / (CHUNKWIDTH * CHUNKHEIGHT * 3);

    chunks = (void **)malloc(numchunks * sizeof(void *));
    id = (int *)malloc(numchunks * sizeof(int));
    used = (int *)malloc(numchunks * sizeof(int));

    chunks16 = (void **)malloc(numchunks * sizeof(void *));
    id16 = (int *)malloc(numchunks * sizeof(int));
    used16 = (int *)malloc(numchunks * sizeof(int));

    for (int loop = 0; loop < numchunks; loop++)
    {
        chunks[loop] = NULL;
        id[loop]     = 0;
        used[loop]   = 0;
    
        chunks16[loop] = NULL;
        id16[loop]     = 0;
        used16[loop]   = 0;
    }   

    currentcycle   = 0;
    currentcycle16 = 0;
    chunkbuffer    = (char *)malloc(CHUNKWIDTH * CHUNKHEIGHT * numchunks);
//  if (!VirtualLock(chunkbuffer, CHUNKWIDTH * CHUNKHEIGHT * numchunks))
//  {
//      Error("Error");
//  }
    chunkbuffer16  = (char *)malloc(CHUNKWIDTH * CHUNKHEIGHT * numchunks * 2);
//  if (!VirtualLock(chunkbuffer16, CHUNKWIDTH * CHUNKHEIGHT * numchunks * 2))
//  {
//      Error("Error");
//  }
}

DWORD TChunkCache::MemUsed()
{
    return numchunks * CHUNKWIDTH * CHUNKHEIGHT * 3;
}

void *TChunkCache::AddChunk(void *chunk, DWORD type)
{
    if (chunk == NULL || chunks == NULL)
        return NULL;

    BEGIN_CRITICAL(); // Don't allow two threads at once

    int oldest    = 0x7fffffff;
    int oldestptr = 0;

    currentcycle++;

    int *value = (int *)chunk;
    int number = *value;

    int *idptr = id;
    int *usedptr = used;
    for (int loop = 0; loop < numchunks; loop++, idptr++, usedptr++)
    {
        if (*idptr == number)
        {
            used[loop] = currentcycle;
            END_CRITICAL();
            return chunks[loop];
        }
            
        if (*usedptr < oldest)
        {
            oldest     = used[loop];
            oldestptr  = loop;
        }
    }

    used[oldestptr]   = currentcycle;                                         
    chunks[oldestptr] = (void *)((char *)chunkbuffer + (oldestptr * CHUNKWIDTH * CHUNKHEIGHT));

    END_CRITICAL(); // 

    number = ChunkDecompress(chunk, chunks[oldestptr], type);
    id[oldestptr] = number;

    return chunks[oldestptr];
}

void *TChunkCache::AddChunkZ(void *chunk, DWORD type)
{
    if (chunk == NULL || chunks == NULL)
        return NULL;

    BEGIN_CRITICAL(); // Don't allow two threads at once

    int oldest    = 0x7fffffff;
    int oldestptr = 0;

    currentcycle16++;

    int *value = (int *)chunk;
    int number = *value;

    for (int loop = 0; loop < numchunks; loop++)
    {
        if (id16[loop] == number)
        {
            used16[loop] = currentcycle16;
            END_CRITICAL(); 
            return chunks16[loop];
        }
            
        if (used16[loop] < oldest)
        {
            oldest     = used16[loop];
            oldestptr  = loop;
        }
    }

    used16[oldestptr]   = currentcycle16;
    chunks16[oldestptr] = (void *)((char *)chunkbuffer16 + (oldestptr * CHUNKWIDTH * CHUNKHEIGHT * 2));

    END_CRITICAL(); // 

    number = ChunkDecompressZ(chunk, chunks16[oldestptr], type);
    id16[oldestptr] = number;

    return chunks16[oldestptr];
}

void *TChunkCache::AddChunk16(void *chunk, DWORD type)
{
    if (chunk == NULL || chunks == NULL)
        return NULL;

    BEGIN_CRITICAL(); // Don't allow two threads at once

    int oldest    = 0x7fffffff;
    int oldestptr = 0;

    currentcycle16++;

    int *value = (int *)chunk;
    int number = *value;

    for (int loop = 0; loop < numchunks; loop++)
    {
        if (id16[loop] == number)
        {
            used16[loop] = currentcycle16;
            END_CRITICAL(); 
            return chunks16[loop];
        }
            
        if (used16[loop] < oldest)
        {
            oldest     = used16[loop];
            oldestptr  = loop;
        }
    }

    used16[oldestptr]   = currentcycle16;
    chunks16[oldestptr] = (void *)((char *)chunkbuffer16 + (oldestptr * CHUNKWIDTH * CHUNKHEIGHT * 2));

    END_CRITICAL(); // 

    number = ChunkDecompress16(chunk, chunks16[oldestptr], type);
    id16[oldestptr] = number;

    return chunks16[oldestptr];
}

BOOL TChunkCache::RemoveChunk(int number)
{
    if (chunks == NULL)
        return FALSE;

    BEGIN_CRITICAL(); // Don't allow two threads at once

    for (int loop = 0; loop < numchunks; loop++)
    {
        int cycle = currentcycle - 10;

        if (id[loop] == number)
        {
            used[loop] = cycle;
            id[loop]   = 0;

            break;
        }
    }

    END_CRITICAL(); // 

    if (loop == numchunks)
        return FALSE;

    return TRUE;
}

BOOL TChunkCache::RemoveChunk16(int number)
{
    if (chunks == NULL)
        return FALSE;

    BEGIN_CRITICAL(); // Don't allow two threads at once

    for (int loop = 0; loop < numchunks; loop++)
    {
        int cycle = currentcycle16 - 10;

        if (id16[loop] == number)
        {
            used16[loop] = cycle;
            id16[loop]   = 0;

            break;
        }
    }

    END_CRITICAL(); // 

    if (loop == numchunks)
        return FALSE;

    return TRUE;
}

int TChunkCache::ChunkDecompress(void *source, void *dest, DWORD clear)
{
    if (chunks == NULL)
        return -1;
    
    int *buffer = (int *)source;
    int number  = *buffer;
    buffer++;
    
    DWORD tmpedx;
    DWORD tmpesi;

    __asm
    {
        mov  ecx, CHUNKWIDTH * CHUNKHEIGHT
        shr  ecx, 2

        mov  edi, [dest]

        mov  eax, [clear]
        cmp  eax, 1
        jne  clearzloop

    clearloop:
        mov  DWORD PTR [edi], DWORD PTR 0
        add  edi, 4
        dec  ecx
        jne  clearloop
        jmp  normy

    clearzloop:
        mov  DWORD PTR [edi], DWORD PTR 0xffffffff
        add  edi, 4
        dec  ecx
        jne  clearzloop

    normy:
        mov  esi, [buffer]          ; Point ESI to source
        mov  edi, [dest]            ; Point EDI to destination

        cld                         ; Forward direction

        mov  dx, [esi]              ; Get DLE 1/2
        add  esi, 2                 ; Advance past header
        
        mov  ecx, CHUNKHEIGHT

    DecompLoop:
        mov  ax, [esi]              ; Get one byte of data
        inc  esi

        cmp  al, dl                 ; Is RLE ?
        je   unrle  

        cmp  al, dh                 ; Is LZ ?
        je   unlz   

        mov  [edi], al              ; Store raw data

        inc  edi
        jmp  DecompLoop

    EOL:
        dec  ecx
        jne  DecompLoop

        jmp  Exit

    unrle:
        mov  al, ah             ; Get count
        inc  esi

        or   al, al
        je   EOL

        cmp  al, 80h
        jb   NormalRLE
        
        and  eax, 7fh
        add  edi, eax

        jmp  DecompLoop             ; Loop for more data

    NormalRLE:
        and  eax, 7fh
        mov  [tmpedx], edx          

        mov  edx, eax               ; Use local counter
        mov  al, [esi]              ; Get byte to repeat cx times

        inc  esi                    ; Advance
        mov  ah, al                 ; Extend to ax

        mov  ebx, eax               ; Store in bx
        shl  eax, 16                ; Put it into high 16 bits

        mov  al, bl                 ; Include it in low 16 bits
        mov  ah, bh                 ; Include it in low 16 bits

        shr  edx, 1                 ; Make ready for word
        setc bl                     ; Store carry-flag in bl

        shr  edx, 1                 ; Make ready for dwords
        setc bh                     ; Store carry-flag in bh

        or   edx, edx
        je   CheckSingle
    
    RLEDecompLoop:
        mov  [edi], eax
        add  edi, 4

        dec  edx
        jne  RLEDecompLoop

    CheckSingle:
        or   bh, bh
        jz   skip1

        mov  [edi], ax              ; Store extra-word
        add  edi, 2                 ; Advance

    skip1:
        or   bl, bl                 ; Extra byte ?
        jz   skip2  

        mov  [edi], al              ; Store extra byte
        inc  edi                    ; Advance

    skip2:
        mov  edx, [tmpedx]
        jmp  DecompLoop             ; Loop for more data

    unlz:
        xor  ebx, ebx
        mov  [tmpedx], edx

        mov  dl, ah                 ; Get count
        mov  bx, [esi + 1]          ; Get distance to look back

        and  edx, 000000ffh
        add  esi, 3                 ; Advance

        mov  [tmpesi], esi 
        sub  esi, ebx               ; Go back to start of data

        sub  esi, 4

        cmp  ebx, 4                 ; Less than 4 bytes back ?
        jae  Ok386

    LZDecompLoop:
        mov  al, [esi]
        inc  esi

        mov  [edi], al
        inc  edi

        dec  edx
        jne  LZDecompLoop

        jmp  GoOn

    Ok386:
        shr  edx, 1                 ; Make ready for words
        setc bl                     ; Save carry flag

        shr  edx, 1                 ; Make ready for dwords
        setc bh                     ; Save carry flag

        or   edx, edx
        je   Skip3

    LZDeLoop2:
        mov  eax, [esi]
        add  esi, 4

        mov  [edi], eax
        add  edi, 4
    
        dec  edx
        jne  LZDeLoop2

        or   bh, bh
        jz   Skip3                  ; Skip if no extra word

        mov  ax, [esi]
        add  esi, 2

        mov  [edi], ax
        add  edi, 2
    
    Skip3:
        or   bl, bl                 ; Extra-byte ?
        jz   GoOn   

        mov  al, [esi]
        inc  esi

        mov  [edi], al
        inc  edi

    GoOn:
        mov  edx, [tmpedx]
        mov  esi, [tmpesi]

        jmp  DecompLoop

    Exit:
    }

    return number;
}

int TChunkCache::ChunkDecompressZ(void *source, void *dest, DWORD clear)
{
    if (chunks == NULL)
        return -1;

    int *buffer = (int *)source;
    int number  = *buffer;
    buffer++;
    
    DWORD tmpecx;
    DWORD tmpedx;
    DWORD tmpesi;
    BYTE  highbyte = 0;

    __asm
    {
        mov  ecx, CHUNKWIDTH * CHUNKHEIGHT
        shr  ecx, 1

        mov  edi, [dest]

    clearzloop:
        mov  DWORD PTR [edi], DWORD PTR 0x7f7f7f7f
        add  edi, 4
        dec  ecx
        jne  clearzloop

        mov  esi, [buffer]          ; Point ESI to source
        mov  edi, [dest]            ; Point EDI to destination

        cld                         ; Forward direction

        mov  dx, [esi]              ; Get DLE 1/2
        add  esi, 2                 ; Advance past header
        
        mov  [tmpecx], CHUNKHEIGHT

    DecompLoop:
        mov  ax, [esi]              ; Get one byte of data
        inc  esi

        cmp  al, dl                 ; Is RLE ?
        je   unrle  

        cmp  al, dh                 ; Is LZ ?
        je   unlz   

        mov  ah, [highbyte]
        mov  [edi], ax              ; Store raw data

        add  edi, 2
        jmp  DecompLoop

    EOL:
        dec  [tmpecx]
        jne  DecompLoop

        jmp  Exit

    unrle:
        mov  al, ah             ; Get count
        inc  esi

        or   al, al
        je   EOL
                 
        cmp  al, 80h
        jb   NormalRLE
        
        and  eax, 7fh
        
        shl  eax, 1
        add  edi, eax

        jmp  DecompLoop             ; Loop for more data

    NormalRLE:
        and  eax, 7fh
        mov  [tmpedx], eax          

        mov  al, [esi]              ; Get byte to repeat cx times
        inc  esi                    ; Advance

        mov  ah, [highbyte]
        mov  ebx, eax               ; Store in bx
        
        shl  eax,  16               ; Put it into high 16 bits
        mov  al, bl                 ; Include it in low 16 bits
        
        mov  ah, bh
        shr  [tmpedx], 1            ; Make ready for word

        setc bl                     ; Store carry-flag in bl
        
        cmp  [tmpedx], 0
        je   CheckSingle
    
    RLEDecompLoop:
        mov  [edi], eax
        add  edi, 4

        dec  [tmpedx]
        jne  RLEDecompLoop

    CheckSingle:
        or   bl, bl                 ; Extra byte ?
        je   skip2  

        mov  [edi], ax              ; Store extra byte
        add  edi, 2                 ; Advance

    skip2:
        jmp  DecompLoop             ; Loop for more data

    unlz:
        or   ah, ah
        jne  normallz

        mov  ah, [esi + 1]
        add  esi, 2

        mov  [highbyte], ah
        jmp  DecompLoop

    normallz:
        mov  al, ah                 ; Get count

        mov  bx, [esi + 1]          ; Get distance to look back
        and  ebx, 0000ffffh

        and  eax, 000000ffh
        mov  [tmpedx], eax
    
        add  esi, 3                 ; Advance
        mov  [tmpesi], esi 

        sub  esi, ebx               ; Go back to start of data
        sub  esi, 4

        mov  ah, [highbyte]

    LZDecompLoop:
        mov  al, [esi]
        inc  esi

        mov  [edi], ax
        add  edi, 2

        dec  [tmpedx]
        jne  LZDecompLoop

        mov  esi, [tmpesi]
        jmp  DecompLoop

    Exit:
    }

    return number;
}

int TChunkCache::ChunkDecompress16(void *source, void *dest, DWORD clear)
{
    if (chunks == NULL)
        return -1;

    int *buffer = (int *)source;
    int number  = *buffer;
    buffer++;

    memcpy(dest, buffer, CHUNKWIDTH * CHUNKHEIGHT * 2);

    return number;
}

TChunkCache::~TChunkCache()
{
//  VirtualUnlock(chunkbuffer, CHUNKWIDTH * CHUNKHEIGHT * numchunks);
//  VirtualUnlock(chunkbuffer16, CHUNKWIDTH * CHUNKHEIGHT * numchunks * 2);

    if (chunks)
        free(chunks);
    if (id)
        free(id);
    if (used)
        free(used);
    if (chunks16)
        free(chunks16);
    if (id16)
        free(id16);
    if (used16)
        free(used16);
    if (chunkbuffer)
        free(chunkbuffer);
    if (chunkbuffer16)
        free(chunkbuffer16);
}

