// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   container.h - TContainer module                     *
// *************************************************************************

#ifndef _CONTAINER_H
#define _CONTAINER_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

_CLASSDEF(TContainer)
class TContainer : public TObjectInstance
{
  public:
    TContainer(PTObjectImagery newim) : TObjectInstance(newim) { }
    TContainer(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) { }

    virtual BOOL Use(PTObjectInstance user, int with = -1);
    virtual int CursorType(PTObjectInstance inst = NULL);

    virtual void Load(RTInputStream is, int version, int objversion);
    virtual void Save(RTOutputStream os);

    int NumObjects();
        // Count the number of objects in the container

    BOOL CheckKeyUse(PTObjectInstance user, PTObjectInstance inst);
        // Try to use inst to unlock this object
    
    // Container stats
    STATFUNC(Openable)
    STATFUNC(Value)
    OBJSTATFUNC(Locked)
    OBJSTATFUNC(KeyId)
    OBJSTATFUNC(PickDifficulty)

    virtual BOOL AddToInventory(PTObjectInstance inst, int slot = -1);
        // Add inst to this object's inventory, in the given slot (first free slot if none specified)
};

DEFINE_BUILDER("CONTAINER", TContainer)

#endif

