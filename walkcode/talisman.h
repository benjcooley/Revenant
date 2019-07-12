// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   talisman.h - TTalisman module                       *
// *************************************************************************

#ifndef _TALISMAN_H
#define _TALISMAN_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

_CLASSDEF(TTalisman)
class TTalisman : public TObjectInstance
{
  public:
	TTalisman(PTObjectImagery newim) : TObjectInstance(newim) {}
	TTalisman(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}

	virtual void AddToInventory(PTObjectInstance oi);
		// Add this object to oi's inventory

	// Talisman statistics
	STATFUNC(Value)
	STATFUNC(Order)
	STATFUNC(Power)
	STATFUNC(Earth)
	STATFUNC(Fire)
	STATFUNC(Lightning)
	STATFUNC(Ice)
	STATFUNC(Destruction)
	STATFUNC(Healing)

};

DEFINE_BUILDER("TALISMAN", TTalisman)

#endif