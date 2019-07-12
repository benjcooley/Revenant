// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       tool.cpp - TTool module                         *
// *************************************************************************

#include "revenant.h"
#include "tool.h"
#include "exit.h"
#include "container.h"
#include "mappane.h"

REGISTER_BUILDER(TTool)
TObjectClass ToolClass("TOOL", OBJCLASS_TOOL, 0);

// Hard coded class stats
DEFSTAT(Tool, Value, VAL, 0, 0, 0, 1000000)
DEFSTAT(Tool, Pick, PICK, 1, 0, 0, 100)

BOOL TTool::Use(PTObjectInstance user, int with)
{
    return FALSE;
}

int TTool::CursorType(PTObjectInstance with)
{
    return CURSOR_NONE;
}

