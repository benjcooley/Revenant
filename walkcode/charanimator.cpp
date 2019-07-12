// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 charanimator.cpp - TCharAnimator module               *
// *************************************************************************

#include <windows.h>

#include "character.h"
#include "charanimator.h"

TCharAnimator::TCharAnimator(PTObjectInstance oi) : T3DAnimator(oi)
{
	origmatred = new float[MAXMATERIALS];
	origmatgreen = new float[MAXMATERIALS];
	oldpoison = 0;

	S3DMat mat;
	for (int i = 0; i < Get3DImagery()->NumMaterials(); i++)
	{
		memset(&mat, 0, sizeof(S3DMat));
		Get3DImagery()->GetMaterial(i, &mat);

		D3DMATERIAL &m = mat.matdesc;
		origmatred[i] = m.ambient.r;
		origmatgreen[i] = m.ambient.g;
	}
}

TCharAnimator::~TCharAnimator()
{
	S3DMat mat;
	for (int i = 0; i < Get3DImagery()->NumMaterials(); i++)
	{
		memset(&mat, 0, sizeof(S3DMat));
		Get3DImagery()->GetMaterial(i, &mat);

		D3DMATERIAL &m = mat.matdesc;
		m.ambient.r = D3DVAL(origmatred[i]);
		m.ambient.b = D3DVAL(origmatgreen[i]);

		Get3DImagery()->SetMaterial(i, &mat);
	}

	delete [] origmatred;
	delete [] origmatgreen;

	Close();
}

void TCharAnimator::Animate(BOOL draw)
{
	T3DAnimator::Animate(draw);
}

BOOL TCharAnimator::Render()
{
	int poison = 9 * ((PTCharacter)inst)->Poisoned();

	if (poison != oldpoison)
	{
		S3DMat mat;

		float amount = ((float)10.0 - (float)poison) / (float)10.0;

		if (poison > 10)
			amount = 0.0;
		else if (poison < 0)
			amount = 1.0;

		for (int i = 0; i < Get3DImagery()->NumMaterials(); i++)
		{
			memset(&mat, 0, sizeof(S3DMat));
			Get3DImagery()->GetMaterial(i, &mat);

			D3DMATERIAL &m = mat.matdesc;

			if (amount < 0.1)
			{
				m.ambient.r = D3DVAL(origmatred[i]);
				m.ambient.b = D3DVAL(origmatgreen[i]);
			}
			else
			{
				m.ambient.r = D3DVAL(origmatred[i] * amount);
				m.ambient.b = D3DVAL(origmatgreen[i] * amount);
			}

			Get3DImagery()->SetMaterial(i, &mat);
		}
	}

	T3DAnimator::Render();

	return TRUE;
}

REGISTER_3DANIMATOR("CHARACTER", TCharAnimator)
REGISTER_3DANIMATOR("PLAYER", TPlayerAnimator)

