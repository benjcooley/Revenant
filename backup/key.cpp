// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        key.cpp - TKey module                          *
// *************************************************************************

#include "revenant.h"
#include "key.h"
#include "object.h"
#include "mappane.h"
#include "player.h"

REGISTER_BUILDER(TKey)
TObjectClass KeyClass("KEY", OBJCLASS_KEY, 0);

// Hard coded class stats
DEFSTAT(Key, Value, VAL, 0, 0, 0, 1000000)

// Hard codes object stats
DEFOBJSTAT(Key, KeyId, KEY, 0, 0, 0, 1000000)


