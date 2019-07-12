// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   charanimator.h - TCharAnimator module               *
// *************************************************************************

#ifndef _CHARANIMATOR_H
#define _CHARANIMATOR_H

#ifndef _WINDOWS_
#error WINDOWS.H Must be included at the top of your .CPP file
#endif

#include "3dimage.h"

// *****************
// * TCharAnimator *
// *****************

_CLASSDEF(TCharAnimator)
class TCharAnimator : public T3DAnimator
{
  public:
	TCharAnimator(PTObjectInstance oi);
	virtual ~TCharAnimator();

	virtual void Animate(BOOL draw);
	virtual BOOL Render();

  protected:
	float *origmatred, *origmatgreen;		// saved material values

	int oldpoison;			// update only when needed
};

_CLASSDEF(TPlayerAnimator)
class TPlayerAnimator : public TCharAnimator
{
  public:
	TPlayerAnimator(PTObjectInstance oi) : TCharAnimator(oi) {}
	virtual ~TPlayerAnimator()				{}
};

#endif
