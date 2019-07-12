// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                lightsource.cpp - TLightSource module                  *
// *************************************************************************

#include "revenant.h"
#include "lightsource.h"

REGISTER_BUILDER(TLightSource)
TObjectClass LightSourceClass("LIGHTSOURCE", OBJCLASS_LIGHTSOURCE, 0);

// Hard coded class stats
DEFSTAT(LightSource, EqSlot, EQSL, 0, 4, 0, 6)
DEFSTAT(LightSource, Value,  VAL,  1, 0, 0, 1000000)
