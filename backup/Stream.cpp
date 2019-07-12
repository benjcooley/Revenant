// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     stream.cpp - TStream object                       *
// *************************************************************************

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "revenant.h"
#include "stream.h"
#pragma hdrstop

// ******************************
// * Constructor and Destructor *
// ******************************

TOutputStream::TOutputStream(int startsize, int grwsize)
{
    buf = (BYTE *)malloc(startsize);
    bufsize = startsize;
    growsize = grwsize;
    Reset();
}

TOutputStream::~TOutputStream()
{
    delete buf;
}

void TOutputStream::MakeFreeSpace(int freespace)
{
    int datasize = DataSize();
    if (bufsize - datasize < freespace)
    {
        buf = (BYTE *)realloc(buf, bufsize + growsize);
        ptr = buf + datasize;
        bufsize = bufsize + growsize;
    }
}

// ****************
// * IO Functions *
// ****************

RTInputStream TInputStream::operator >> (char *d)
{
    strncpy(d, (char *)(ptr + 1), *ptr);
    d[*ptr] = NULL;
    ptr += *ptr + 1;
    return *this;
}

RTOutputStream TOutputStream::operator << (char *d)
{
    *ptr = strlen(d);
    strncpy((char *)(ptr + 1), d, *ptr);
    ptr += *ptr + 1;
    return *this;
}
