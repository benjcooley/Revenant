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

