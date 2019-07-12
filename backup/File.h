// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       file.h - Common File IO                         *
// *************************************************************************

#if !defined(FILE_DEFINED)
#define FILE_DEFINED

#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>


//==============================================================================
//                           File.CPP Function Prototypes
//==============================================================================

void *LoadFile(const char *filepath, void *filebuf = NULL, long *filesize = NULL);
BOOL FileExists(const char *filepath);
long FileSize(const char *filepath);
FILE *TryOpen(const char *name, const char *mode);
BOOL TryDelete(const char *name);
BOOL TryRename(const char *oldname, const char *newname);

#endif
