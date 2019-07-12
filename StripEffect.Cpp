// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                stripeffect.cpp - TStripEffect module                  *
// *************************************************************************

#include <ddraw.h>
#include <d3d.h>
#include <d3drmwin.h>
#include <math.h>
#include "d3dmacs.h"
#include "d3dmath.h"

#include "effect.h"
#include "revenant.h"
#include "stripeffect.h"
#include "effect.h"
#include "character.h"
#include "3dscene.h"

// ******************
// * TStripEffect   *
// ******************

DEFINE_BUILDER("LightStrip", TStripEffect)
REGISTER_BUILDER(TStripEffect)

void TStripEffect::Initialize()
{
    my_state = STRIP_LAUNCH;
	duration = 20;
	targetrange = -1;
	S3DPoint temp_point;
	GetPos(temp_point);
	temp_point.z += 50;
	SetPos(temp_point);
}

void TStripEffect::Pulse()
{
	if (!spell)
	{
		KillThisEffect();
		return;
	}

    switch (my_state)
    {
        case STRIP_LAUNCH:
        { 
			PTObjectInstance invoker = spell->GetInvoker();
			S3DPoint newpos, vel;
			int pnt, block;

			invoker->GetPos( newpos);
			ConvertToVector( facing, 10, vel);

			for (pnt = 0; pnt < 16; pnt++)
			{
				newpos.x += vel.x;
				newpos.y += vel.y;
				block = MapPane.GetWalkHeight( newpos);
				if ((block == 0) || ((PTCharacter)Player)->CharBlocking( ((PTObjectInstance)Player), newpos))
				{
					// Either a character or a wall
					targetrange = (pnt + 2);
					break;
				}
			}
		
			my_state = STRIP_FLY;
            break;
        }
        case STRIP_FLY:
        {
			duration--;
			if (!duration)
			{
				my_state = STRIP_EXPLODE;
				duration = 5;
			}
            break;
        }
		case STRIP_EXPLODE:
			if (duration) duration--;
			break;
	}

	TEffect::Pulse();
}

void TStripEffect::OffScreen()
{
	if (animator && (my_state == STRIP_EXPLODE) && commanddone)
		TObjectInstance::OffScreen();
}


// **********************
// * Lightning Animator *
// **********************

REGISTER_3DANIMATOR("LightStrip", TLightningAnimator)

void TLightningAnimator::Initialize()
{
	D3DMATRIX pos;
	D3DVECTOR hand_pos;

	T3DAnimator::Initialize();

	PLAY("lightning sound");

	// get effect position
	S3DPoint effect;
	((PTEffect)inst)->GetPos(effect);

	// get the position of the hand so that the lightning is in the correct place..
	GetObjectMatrix(GetObjectNum("lhand"), &pos);

	// extract the translation info from the hand object's matrix
	hand_pos.x = pos._41;
	hand_pos.y = pos._42;
	hand_pos.z = pos._43;

	firsttime = TRUE;

	// init the sparks
	PS3DAnimObj o = GetObject(2);
	spark.Init(this, o);
	impact_spark.Init(this, o);

	// set the parameters
	SSubParticleParams params;

	params.particles = 20;
	params.chance = 25;
	params.velocity_dir.x = 0;
	params.velocity_dir.y = 0;
	params.velocity_dir.z = 0;
	params.pos.x = (float)effect.x + hand_pos.x;
	params.pos.y = (float)effect.y + hand_pos.y;
	params.pos.z = (float)effect.z + hand_pos.z;
	params.pos_spread.x = params.pos_spread.y = params.pos_spread.z = 0.0f;
	params.scale.x = params.scale.y = params.scale.z = 2.0f;
	params.scale_dec.x = params.scale_dec.y = params.scale_dec.z = .90f;
	params.scale_spread.x = params.scale_spread.y = params.scale_spread.z = 0.0f;
	params.velocity.x = params.velocity.y = params.velocity.z = 0.0f;
	params.velocity_spread.x = params.velocity_spread.y = 2.0f;
	params.velocity_spread.z = 1.0f;
	params.gravity = .18f;
	params.min_life = 25;
	params.max_life = 35;
	params.flicker = TRUE;
	params.flicker_size = 2.5f;

	// set them!
	spark.Set(&params);
	params.particles = 0;
	params.pos = end_p;
	impact_spark.Set(&params);

	glow_scale = 3.4f;
	total_offset = 0;
}

void TLightningAnimator::SetupObjects()
{
	D3DVECTOR p;
	int	i;

	// LightStrip
	PS3DAnimObj o = NewObject( 0);

	showangle = 0;
	rotdegree = 0;
	morrotdegree = 0;
	maxsize = FALSE;
	down = TRUE;
	damage_flag = FALSE;

	numpoints = 2;
	//maxpoints = (((PTStripEffect)inst)->targetrange == -1) ? 40 : ((PTStripEffect)inst)->targetrange;

	// code for figure the maxpoints
	float r = 10.0f;
	float dx = r * (float)sin((float)TORADIAN * (((float)inst->GetFace() * 360.0f) / 256.0f));
	float dy = -r * (float)cos((float)TORADIAN * (((float)inst->GetFace() * 360.0f) / 256.0f));
	S3DPoint effect;
	((PTEffect)inst)->GetPos(effect);
	D3DVECTOR point;
	point.x = (float)effect.x;
	point.y = (float)effect.y;
	point.z = (float)effect.z;
	maxpoints = 1;
	float zheight;
//	int num;
	BOOL flag = FALSE;
	S3DPoint new_pos;
	do
	{
		point.x += dx;
		point.y += dy;

		effect.x = (int)point.x;
		effect.y = (int)point.y;
		effect.z = (int)point.z;

		zheight = (float)MapPane.GetWalkHeight(effect);

//		int targets[MAXFOUNDOBJS];

		new_pos.x = (int)point.x;
		new_pos.y = (int)point.y;
		new_pos.z = (int)point.z - 100;


		S3DPoint temp_point1, temp_point2;
		temp_point1.x = new_pos.x;
		temp_point1.y = new_pos.y;
		temp_point1.z = new_pos.z;
		PTCharacter invoker;

		PTSpell spell = ((PTStripEffect)inst)->GetSpell();
		
		if(spell)
		{
			invoker = (PTCharacter)spell->GetInvoker();
			for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
			{
				((PTStripEffect)inst)->chr = (PTCharacter)i.Item();

				if (((PTStripEffect)inst)->chr == invoker)
					continue;

				if (((PTStripEffect)inst)->chr->IsDead())
					continue;

				((PTStripEffect)inst)->chr->GetPos(temp_point2);
				if (Distance(temp_point1,temp_point2) > 16)
					continue;

				if (invoker && !invoker->IsEnemy(((PTStripEffect)inst)->chr))
					continue;	// We can't hurt our friends
				
	//			if(spell)
	//			{
	//				spell->Damage(chr);
	//			}
				flag = TRUE;
				break;
			}
			if (((PTStripEffect)inst)->chr == invoker)
				((PTStripEffect)inst)->chr = NULL;
		}
		else
			((PTStripEffect)inst)->chr = NULL;
        // See if we've hit a character
/*        num = MapPane.FindObjectsInRange(new_pos, targets, 1, 0, OBJCLASS_CHARACTER);

		for(int i = 0; i < num; ++i) 
		{
			PTObjectInstance inst = MapPane.GetInstance(targets[i]);
            if (inst && inst != ((PTEffect)inst)->GetSpell()->GetInvoker())
				flag = TRUE;
		}
*/
		++maxpoints;
	}
	while(point.z > zheight && flag == FALSE && maxpoints < MAX_MAXPOINTS);

	end_p.x = point.x;
	end_p.y = point.y;
	end_p.z = point.z - 20.0f;

	maxpoints = min(100, maxpoints);

	// if true = used for doing demos
	if(!TARGET_POS)
		maxpoints *= SMOOTH_SIZE;

	mystrip = new TStripAnimator( o, maxpoints);
//	mystrip->SetTextureRange( D3DVALUE( 0.0), D3DVALUE( 1.0));

	mystrip->SetWidth((LIGHTNING_SCALE_N * 16) / LIGHTNING_SCALE_D, (LIGHTNING_SCALE_N * 10) / LIGHTNING_SCALE_D);

//	mystrip->startsize = 0;
//	mystrip->endsize = SMOOTH_SIZE;

	// Set our first segment
    for (i = 0; i < numpoints; i++)
    {
		p.x = 0;
		p.y = (D3DVALUE)(i * -10);
		p.z = 0;
		mystrip->AddPoint( &p);
	}

	AddObject( o);

	// Glow
	o = NewObject( 1);
	AddObject( o);

	// Spark
	o = NewObject( 2);
	AddObject( o);
}

void TLightningAnimator::Animate( BOOL draw) 
{
	int	i;
	D3DVECTOR *p;

	T3DAnimator::Animate( draw);
	inst->SetCommandDone( FALSE);

	SSubParticleParams params;
	if(mystrip->curpoints == mystrip->maxpoints)
	{
		impact_spark.Get(&params);
		params.particles = 20;
		impact_spark.Set(&params);
		if(!damage_flag)
		{
			damage_flag = TRUE;
			S3DPoint new_pos;
			new_pos.x = (int)end_p.x;
			new_pos.y = (int)end_p.y;
			new_pos.z = (int)end_p.z - 80;
//			DamageCharactersInRange(((PTEffect)inst)->GetSpell()->GetInvoker(), new_pos, 1, 15, 25, DAMAGE_FIRE);
		}
	}
	else
	{
		impact_spark.Get(&params);
		params.particles = 0;
		impact_spark.Set(&params);
	}
	
	if(((PTStripEffect)inst)->my_state == STRIP_EXPLODE)
	{		

		params.particles = 0;
		spark.Set(&params);
		impact_spark.Set(&params);

		glow_scale *= .8f;

		for(i = 0; i < ADD_FACTOR; ++i)
			mystrip->DelEndPoint();
		total_offset += (ADD_FACTOR * -10);

		if(firsttime)
		{
			if(((PTStripEffect)inst)->chr)
			{
				if(((PTStripEffect)inst)->GetSpell())
				{
					((PTStripEffect)inst)->GetSpell()->Damage(((PTStripEffect)inst)->chr);
					firsttime=FALSE;
				}
			}
		}
	}

	if(numpoints < maxpoints)
	{
		for(i = 0; i < ADD_FACTOR && numpoints < maxpoints; ++i)
		{
			D3DVECTOR p;

			p.x = 0;
			p.y = D3DVAL( numpoints * -10);
			p.z = 0;

			numpoints++;
			if (numpoints == maxpoints)	// The last point is left on target
				maxsize = TRUE;

			mystrip->AddPoint( &p);
		}
	}
	spark.Animate();
	impact_spark.Animate();

	// smoothing calculations
	float *px = new float[numpoints + SMOOTH_SIZE];
	float *pz = new float[numpoints + SMOOTH_SIZE];
	
	// init beginnings and endings
	for(i = 0; i < numpoints + SMOOTH_SIZE; ++i)
		px[i] = pz[i] = 0;

	// make the standard points
	for (i = 1; i < numpoints; i++)
	{
		if(!(i % SMOOTH_SIZE))
		{
			px[i] = (float)random(-13, 13);
			pz[i] = (float)random(-13, 13);
		}
	}

	int last_point = 0;
	int next_point = SMOOTH_SIZE;
	
	// make the smoothing points
	for (i = 1; i < numpoints; i++)
	{
		if(i == next_point)
		{
			last_point = next_point;
			next_point += SMOOTH_SIZE;
		}

		int mod = i % SMOOTH_SIZE;
		if(mod)
		{
			if(mod == 1)
			{
				px[i] = ((px[next_point] - px[last_point]) * .5f) / (float)SMOOTH_SIZE;
				pz[i] = ((pz[next_point] - pz[last_point]) * .5f) / (float)SMOOTH_SIZE;
			}
			else if(mod == 2)
			{
				px[i] = ((px[next_point] - px[last_point]) * 2.0f) / (float)SMOOTH_SIZE;
				pz[i] = ((pz[next_point] - pz[last_point]) * 2.0f) / (float)SMOOTH_SIZE;
			}
			else if(mod == 3)
			{
				px[i] = ((px[next_point] - px[last_point]) * 3.5f) / (float)SMOOTH_SIZE;
				pz[i] = ((pz[next_point] - pz[last_point]) * 3.5f) / (float)SMOOTH_SIZE;
			}

			px[i] += px[last_point];
			pz[i] += pz[last_point];
		}
	}


	// Randomize points - Should probably be relative to the strip size
	// skip every other point to average the stuff out
    for (i = 1; i < numpoints; i++)
	{
		p = &mystrip->operator[]( i);
		p->x = 0;
		p->z = 0;
		if (down)
			p->z = -D3DVAL( ((i - 1) * 20) / (maxpoints - 1));
		if (i < (maxpoints - 1))
		{
			p->x = px[i];
			p->z += pz[i];
		}
	}

	delete [] px;
	delete [] pz;

//	showangle = ((showangle + 1) % 256);	// Demo Mode
	rotdegree = ((rotdegree + 12) % 360);
	morrotdegree = ((morrotdegree + 16) % 360);

//* Test for reducing the strip
	if (maxsize)
	{
//		((PTCharacter)Player)->EffectBurst( "sparks", 50);
//		if (random( 0, 10) > 7)
//		{
//			mystrip->SetTexture( random( 0, 1));
//			mystrip->DelStartPoint();
//			mystrip->DelEndPoint();
//			if (mystrip->curpoints == 2)
//			{
//				maxsize = FALSE;
//				numpoints = 2;
//			}
//		}
	}

//* Tests for scrolling the texture or advancing the strip
	mystrip->ScrollTexture( D3DVALUE( -0.1));
//	mystrip->AdvanceStrip();
//	mystrip->ReverseStrip();
	if(((PTStripEffect)inst)->my_state == STRIP_EXPLODE && !spark.GetCount() && !impact_spark.GetCount())
		((PTEffect)inst)->KillThisEffect();
}

BOOL TLightningAnimator::Render()
{
	DWORD savedcull;

    SaveBlendState();
    SetBlendState();

	// So I can render any face
	TRY_D3D(Device2->GetRenderState( D3DRENDERSTATE_CULLMODE, &savedcull));
	TRY_D3D(Device2->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_NONE));

	PS3DAnimObj obj;

// LightStrip
	if ((((PTStripEffect)inst)->my_state != STRIP_EXPLODE) || (((PTStripEffect)inst)->duration > 0))
	{
		// Reset render extents
		ResetExtents();

		// Face strip to our absolute angle	
		mystrip->GenerateStrip( inst->GetFace() + showangle);

		obj = mystrip->GetStripObj();

		for(int i=0;i<obj->numverts;i++)
		{
			obj->lverts[i].color = D3DRGBA(1.0f,1.0f,1.0f,1.0f);
		}
		// But rotate strip to its own angle
		if (showangle)
			D3DMATRIXRotateZ( &obj->matrix, D3DVAL( ((showangle * 360) / 256) * TORADIAN));

		//RenderObject( obj);
		RenderObject( obj);

		// I don't know how to get the right rectangles
		//	GetExtents( &extents);
		//	mystrip->AddUpdateRects( &extents);

		// Update bounding rect and screen rect
		UpdateExtents();
	}
// End LightStrip

	if(glow_scale > .005f)
	{
// Glow
		obj = GetObject( 2);

		// Reset render extents
		ResetExtents();

		// Set the scale and position of the object
		obj->flags = OBJ3D_MATRIX;
		obj->scl.x = obj->scl.y = obj->scl.z = D3DVAL( glow_scale + (random( 0, 4) / 10.0));

		D3DMATRIXClear( &obj->matrix);

		D3DMATRIXRotateZ( &obj->matrix, D3DVAL( -rotdegree * TORADIAN));
		D3DMATRIXRotateX( &obj->matrix, D3DVAL( -30 * TORADIAN));
		D3DMATRIXRotateY( &obj->matrix, D3DVAL( 60 * TORADIAN));

		// Offset for our base rotation
		D3DMATRIXRotateZ( &obj->matrix, D3DVAL( -((inst->GetFace() * 360) / 256) * TORADIAN));

		D3DMATRIXTranslate( &obj->matrix, &obj->pos);
		D3DMATRIXScale( &obj->matrix, &obj->scl);

		RenderObject( obj);

		D3DMATRIXClear( &obj->matrix);

		D3DMATRIXRotateZ( &obj->matrix, D3DVAL( morrotdegree * TORADIAN));
		D3DMATRIXRotateX( &obj->matrix, D3DVAL( -30 * TORADIAN));
		D3DMATRIXRotateY( &obj->matrix, D3DVAL( 60 * TORADIAN));

		// Offset for our base rotation
		D3DMATRIXRotateZ( &obj->matrix, D3DVAL( -((inst->GetFace() * 360) / 256) * TORADIAN));

		D3DMATRIXTranslate( &obj->matrix, &obj->pos);
		D3DMATRIXScale( &obj->matrix, &obj->scl);

		RenderObject( obj);

		UpdateExtents();

	}
// End Glow

/*
	{
// Glow
		obj = GetObject( 2);

		// Reset render extents
		ResetExtents();

		// Set the scale and position of the object
		obj->flags = OBJ3D_MATRIX;
		obj->scl.x = obj->scl.y = obj->scl.z = D3DVAL( 0.8 + (random( 0, 4) / 10.0));
		obj->pos.x = pos.x;
		obj->pos.y = pos.y;
		obj->pos.z = pos.z;

		D3DMATRIXClear( &obj->matrix);

		D3DMATRIXRotateZ( &obj->matrix, D3DVAL( -(rotdegree + rotdegree) * TORADIAN));
		D3DMATRIXRotateX( &obj->matrix, D3DVAL( -30 * TORADIAN));
		D3DMATRIXRotateY( &obj->matrix, D3DVAL( 60 * TORADIAN));

		D3DMATRIXTranslate( &obj->matrix, &obj->pos);
		D3DMATRIXScale( &obj->matrix, &obj->scl);

		RenderObject( obj);
		UpdateExtents();

	}
// End Glow
*/

// Sparks	
	spark.Render();
	impact_spark.Render();
// End Sparks

	TRY_D3D(Device2->SetRenderState(D3DRENDERSTATE_CULLMODE, savedcull));

    RestoreBlendState();

	return TRUE;
}

void TLightningAnimator::RefreshZBuffer()
{
	S3DPoint screen_pos;
	S3DPoint map_pos;

	S3DPoint effect_pos;

	((PTEffect)inst)->GetPos(effect_pos);

	WorldToScreen(effect_pos, screen_pos);

	S3DPoint size;
	size.x = 80;
	size.y = 40;

	RestoreZ(screen_pos.x - (size.x / 2), screen_pos.y - (size.y / 2), size.x, size.y);

//	RestoreZ(320, 200, size.x, size.y);

	
	// SPARKS
	//spark.RefreshZBuffer(4, 4);

	// LIGHTNING STRIP

	size.x = 20;
	size.y = 20;
	
	for(int i = 0; i <= numpoints; ++i)
	{
		D3DVECTOR *p;

		p = &mystrip->operator[](i);
		
		map_pos.x = (int)p->x + effect_pos.x;
		map_pos.y = (int)p->y + effect_pos.y;
		// change this to be good stuff, like the rotation use ((PTEffect)inst)->GetAngle() =:)
		map_pos.z = (int)(((((float)p->z * 360) / 256) * TORADIAN) + effect_pos.z);

		WorldToScreen(map_pos, screen_pos);

		RestoreZ(screen_pos.x - (size.x / 2), screen_pos.y - (size.y / 2), size.x, size.y);

//		MapPane.DrawRestoreRect((int)screen_pos.x - 15, (int)screen_pos.y - 15, 30, 30,
//			DM_WRAPCLIPSRC | DM_NORESTORE | DM_ZBUFFER | DM_NODRAW);
	}
	

	// GLOW
	//map_pos.x = effect_pos.x;
	//map_pos.y = effect_pos.y;
	//map_pos.z = effect_pos.z;

	//WorldToScreen(map_pos, screen_pos);
	//MapPane.DrawRestoreRect((int)screen_pos.x - 25, (int)screen_pos.y - 18, 50, 50,
	//	DM_WRAPCLIPSRC | DM_NORESTORE | DM_ZBUFFER | DM_NODRAW);
}

// ********************
// * WindStrip Effect *
// ********************

DEFINE_BUILDER("WindStrip", TWindStripEffect)
REGISTER_BUILDER(TWindStripEffect)

void TWindStripEffect::Initialize()
{
	count = 0;
}

void TWindStripEffect::Pulse()
{
	count++;
	if(count >= 5)
	{
		count = 0;
		if(spell)
		{
			PTCharacter invoker = (PTCharacter)spell->GetInvoker();
			if(invoker)
			{
				int j;
				S3DPoint temp_point1, temp_point2;
				GetPos(temp_point1);
				temp_point1.x += (int)((PTWindStripAnimator)animator)->realpos.x;
				temp_point1.y += (int)((PTWindStripAnimator)animator)->realpos.y;
				temp_point1.z += (int)((PTWindStripAnimator)animator)->realpos.z;
				for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
				{
					PTCharacter chr = (PTCharacter)i.Item();

					if (chr == invoker)
						continue;

					if (chr->IsDead())
						continue;

					if (invoker && !invoker->IsEnemy(chr))
						continue;	// We can't hurt our friends

					chr->GetPos(temp_point2);
					if (::Distance(temp_point1,temp_point2) > 50)
						continue;
					
					for(j=0;j<end_of_targets;j++)
					{
						if(chr == targets[j])
							break;
					}
					if(j == end_of_targets && end_of_targets < 10)
					{
						spell->Damage(chr);
						chr->KnockBack(temp_point1);
						targets[end_of_targets] = chr;
						end_of_targets++;
					}
					break;
				}
			}
		}
	}
	TEffect::Pulse();
}

// **********************
// * WindStrip Animator *
// **********************

REGISTER_3DANIMATOR("WindStrip", TWindStripAnimator)

#define WINDSTRIP_POINTS		20
#define WINDSTRIP_WIDTH			15
#define WINDSTRIP_WIDTH2		15
#define WINDSTRIP_DURATION		250
#define WINDSTRIP_FADEOUT		100
#define WINDSTRIP_MODIFIER		5
#define WINDSTRIP_MODIFIERV		15
#define WINDSTRIP_RADIUS		2
#define WINDSTRIP_NUMSPARKS		(20 * 20)
#define WINDSTRIP_SPARKSCALE	0.4
#define WINDSTRIP_SPARKDSCALE	0.005
#define WINDSTRIP_SPARKGRAV		0.2
#define WINDSTRIP_SPARKFALL		0.5
#define WINDSTRIP_HALOSTEP		0.04

void TWindStripAnimator::Initialize()
{
	T3DAnimator::Initialize();

	// get effect position
	S3DPoint effect;
	((PTEffect)inst)->GetPos(effect);
	subspell = ((PTEffect)inst)->GetSubSpell();

	center.x = (float)0;
	center.y = (float)0;
	center.z = (float)0;

	realpos.x = (float)0;
	realpos.y = (float)0;
	realpos.z = (float)0;

	centerang = 0;
	haloscale = (float)0;
	frameon = 0;

	//PS3DAnimObj o = GetObject(1);
	for (int i = 0; i < WINDSTRIP_MAX_STRIPS; i++)
	{
		r[i] = (float)random(0, 6);
		th[i] = (float)random(0, 6);
		h[i] = (float)random(0, 6);
		dr[i] = (float)(random(1, 20) / 100.0);
		dth[i] = (float)(random(15, 20) / 100.0);
		dh[i] = (float)(random(1, 20) / 100.0);
		ac[i] = (float)(0.0);
		dac[i] = (float)(random(5, 10) / 100.0);
	
		//mystrip[i] = new TStripAnimator(o, WINDSTRIP_POINTS);
		//mystrip[i]->SetWidth(WINDSTRIP_WIDTH, WINDSTRIP_WIDTH2);
		//mystrip[i]->SetTextureRange((float)-1, (float)1);
	}
	InitDrops();
}

void TWindStripAnimator::SetupObjects()
{
	//D3DVECTOR p;
	//int	i;

	PS3DAnimObj o;
	// sparks
	o = NewObject(0, OBJ3D_COPYVERTS | OBJ3D_COPYFACES);
	AddObject(o);

	// halo
	o = NewObject(1, OBJ3D_COPYVERTS | OBJ3D_COPYFACES);
	AddObject(o);

	// strips
	o = NewObject(0);
	AddObject(o);

	for (int i = 0; i < WINDSTRIP_MAX_STRIPS; i++)
	{
		mystrip[i] = new TStripAnimator(o, WINDSTRIP_POINTS);
		mystrip[i]->SetWidth(WINDSTRIP_WIDTH, WINDSTRIP_WIDTH2);
		mystrip[i]->SetTextureRange((float)-1, (float)1);
	}
	//PS3DAnimObj me = GetObject(0);
	//PS3DAnimObj metoo = GetObject(1);
	//int ok = 2;
}

void TWindStripAnimator::Animate(BOOL draw) 
{
	int	j, i;
	D3DVECTOR p;
	
	T3DAnimator::Animate(draw);
	inst->SetCommandDone(FALSE);

	/*if(inst->GetState() == STRIP_EXPLODE)
	{		
		for(i = 0; i < ADD_FACTOR; ++i)
			mystrip->DelEndPoint();
	}*/

	if (drops == NULL)
		return;

	for (j = 0; j < WINDSTRIP_MAX_STRIPS; j++)
	{
		if (frameon == 0)
		{
			for (i = 0; i < WINDSTRIP_POINTS / 2; i++)
			{
				p.x = (float)((cos(r[j]) + 2) * cos(th[j]) * WINDSTRIP_MODIFIER * ac[j] * (sin(h[j]) + 1));
				p.y = (float)((cos(r[j]) + 2) * sin(th[j]) * WINDSTRIP_MODIFIER * ac[j] * (sin(h[j]) + 1));
				p.z = (float)(sin(h[j]) * WINDSTRIP_MODIFIERV * ac[j]);
				mystrip[j]->AddPoint(&p);
				th[j] += (float)dth[j];
				r[j] += (float)dr[j];
				h[j] += (float)dh[j];
				ac[j] += (float)dac[j];
				if (ac[j] > 5.0)
					ac[j] = (float)5.0;
			}
		}
		else
		{
			mystrip[j]->DelStartPoint();
			p.x = (float)((cos(r[j]) + 2) * cos(th[j]) * WINDSTRIP_MODIFIER * ac[j] * (sin(h[j]) + 1));
			p.y = (float)((cos(r[j]) + 2) * sin(th[j]) * WINDSTRIP_MODIFIER * ac[j] * (sin(h[j]) + 1));
			p.z = (float)(sin(h[j]) * WINDSTRIP_MODIFIERV * ac[j]);
			mystrip[j]->AddPoint(&p);
			th[j] += (float)dth[j];
			r[j] += (float)dr[j];
			h[j] += (float)dh[j];
			p.x += center.x;
			p.y += center.y;
			p.z += center.z;
			AddDrop(p);
			if (frameon < WINDSTRIP_FADEOUT)
			{
				ac[j] += (float)dac[j];
				if (ac[j] > 5.0)
					ac[j] = (float)5.0;
			}
			else
			{
				ac[j] -= (float)dac[j];
				if (ac[j] < 0.0)
					ac[j] = (float)0.0;
			}
			if (frameon < WINDSTRIP_FADEOUT / 2)
				haloscale += (float)WINDSTRIP_HALOSTEP;
			if (frameon > (WINDSTRIP_FADEOUT * 3) / 2)
				haloscale -= (float)WINDSTRIP_HALOSTEP;
		}
	}
	for (i = 0; i < WINDSTRIP_NUMSPARKS; i++)
	{
		if (!drops[i].used)
			continue;
		drops[i].pos.x += drops[i].vel.x;
		drops[i].pos.y += drops[i].vel.y;
		drops[i].pos.z += drops[i].vel.z;
		drops[i].vel.z -= (float)WINDSTRIP_SPARKGRAV;
		drops[i].scl -= (float)WINDSTRIP_SPARKDSCALE;
		if (drops[i].scl <= 0 || drops[i].pos.z < 20)
			drops[i].used = FALSE;
	}
	if(((PTStripEffect)inst)->my_state == STRIP_EXPLODE || frameon > WINDSTRIP_DURATION)
	{
		delete drops;
		drops = NULL;
		((PTEffect)inst)->KillThisEffect();
	}
	frameon++;
	if(((PTEffect)inst)->GetSpell())
	{
		PTObjectInstance invoker = ((PTEffect)inst)->GetSpell()->GetInvoker();
		S3DPoint vel;
		ConvertToVector(centerang, WINDSTRIP_RADIUS, vel);
		center.x += (float)vel.x;
		center.y += (float)vel.y;
		ConvertToVector(centerang + invoker->GetFace(), WINDSTRIP_RADIUS, vel);
		realpos.x += vel.x;
		realpos.y += vel.y;
		centerang = (centerang + random(-2, 2)) & 255;

/*		if (!(frameon % 15))
		{					
			S3DPoint pos, thepos;
			((PTEffect)inst)->GetPos(thepos);
			pos.x = (int)(thepos.x + realpos.x);
			pos.y = (int)(thepos.y + realpos.y);
			pos.z = (int)(thepos.z + realpos.z - 70);
			DamageCharactersInRange(invoker, pos, 100, ((PTEffect)inst)->GetSpell()->VariantData()->mindamage, ((PTEffect)inst)->GetSpell()->VariantData()->maxdamage, ((PTEffect)inst)->GetSpell()->SpellData()->damagetype);
		}
*/
	}

}

BOOL TWindStripAnimator::Render()
{

	if (drops == NULL)
		return TRUE;
	
	int i, j;
	PS3DAnimObj obj, o;

    SaveBlendState();
    SetBlendState();

	// So I can render any face
	// vars for rendering
	DWORD oldcullmode;
	DWORD oldsrcmode;
	DWORD olddestmode;
	DWORD oldblendstate;

	// save the render state
	Device2->GetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, &oldblendstate);
	Device2->GetRenderState(D3DRENDERSTATE_CULLMODE, &oldcullmode);
	Device2->GetRenderState(D3DRENDERSTATE_SRCBLEND, &oldsrcmode);
	Device2->GetRenderState(D3DRENDERSTATE_DESTBLEND, &olddestmode);

	// set the new render flags
	Device2->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
	Device2->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	Device2->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	Device2->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);

// WindStrip
	if(inst->GetState() != STRIP_EXPLODE)
	{
		// Reset render extents
		for (j = 0; j < WINDSTRIP_MAX_STRIPS; j++)
		{
			ResetExtents();
			// Face strip to our absolute angle	
			
			mystrip[j]->GenerateStrip(0);//inst->GetFace());
			obj = mystrip[j]->GetStripObj();

			obj->flags |= OBJ3D_MATRIX;
			D3DMATRIXClear(&obj->matrix);		
		
			// translate to place should be
			obj->pos.x = (D3DVALUE)center.x;
			obj->pos.y = (D3DVALUE)center.y;
			obj->pos.z = (D3DVALUE)center.z;
			D3DMATRIXTranslate(&obj->matrix, &obj->pos);

			//for (i = 0; i < obj->numverts; i++)
				//obj->lverts[i].color = D3DRGBA(0.0, 0.0, 1.0, (obj->numverts - i - 1) / obj->numverts);

			RenderObject(obj);
			// Update bounding rect and screen rect
			UpdateExtents();
		}
		
		o = GetObject(0);
		for (i = 0; i < WINDSTRIP_NUMSPARKS; i++)
		{
			if (!drops[i].used || random(0, 1))
				continue;

			o->flags = OBJ3D_MATRIX;
			D3DMATRIXClear(&o->matrix);		
					
			o->scl.x = (D3DVALUE)(drops[i].scl);
			o->scl.y = (D3DVALUE)(drops[i].scl);
			o->scl.z = (D3DVALUE)(drops[i].scl);
			D3DMATRIXScale(&o->matrix, &o->scl);
			// rotate to face screen
			D3DMATRIXRotateX(&o->matrix, -(float)(M_PI / 2.0));
			//D3DMATRIXRotateX(&o->matrix, -(float)(M_PI / 6.0));
			D3DMATRIXRotateZ(&o->matrix, -(float)(M_PI / 4.0));
			D3DMATRIXRotateZ(&o->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));
			// translate to place should be
			o->pos.x = (D3DVALUE)drops[i].pos.x;
			o->pos.y = (D3DVALUE)drops[i].pos.y;
			o->pos.z = (D3DVALUE)drops[i].pos.z;
			D3DMATRIXTranslate(&o->matrix, &o->pos);

			ResetExtents();
			RenderObject(o);
			UpdateExtents();						
		}

		SetAddBlendState();
		obj = GetObject(1);
		if (haloscale > 0)
		{
			ResetExtents();
			// clear matrix and get ready to transform and render
			D3DMATRIXClear(&obj->matrix);
			obj->flags = OBJ3D_MATRIX;
			// scale to size of spark
			obj->scl.x = obj->scl.y = obj->scl.z = (D3DVALUE)haloscale;
			D3DMATRIXScale(&obj->matrix, &obj->scl);
			// rotate to face screen
			D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 2.0));
			//D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 6.0));
			D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
			D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));
			// translate to place should be
			obj->pos.x = (D3DVALUE)center.x;
			obj->pos.y = (D3DVALUE)center.y;
			obj->pos.z = (D3DVALUE)(center.z + haloscale);
			D3DMATRIXTranslate(&obj->matrix, &obj->pos);
			RenderObject(obj);
			UpdateExtents();
		}
	}
// End WindStrip

	// restore the old render flags
	Device2->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, oldblendstate);
	Device2->SetRenderState(D3DRENDERSTATE_CULLMODE, oldcullmode);
	Device2->SetRenderState(D3DRENDERSTATE_SRCBLEND, oldsrcmode);
	Device2->SetRenderState(D3DRENDERSTATE_DESTBLEND, olddestmode);

    RestoreBlendState();

	return TRUE;
}

void TWindStripAnimator::RefreshZBuffer()
{
	S3DPoint screen_pos;
	S3DPoint map_pos;

	S3DPoint effect_pos;

	((PTEffect)inst)->GetPos(effect_pos);

	WorldToScreen(effect_pos, screen_pos);

	S3DPoint size;
	size.x = 620;
	size.y = 400;

	RestoreZ(screen_pos.x - (size.x / 2), screen_pos.y - (size.y / 2), size.x, size.y);
}

void TWindStripAnimator::InitDrops()
{
	drops = new SDropSparkSystem[WINDSTRIP_NUMSPARKS];
	for (int i = 0; i < WINDSTRIP_NUMSPARKS; i++)
	{
		drops[i].used = FALSE;
	}
}

void TWindStripAnimator::AddDrop(D3DVECTOR pos)
{
	int p = -1, i;
	for (i = 0; i < WINDSTRIP_NUMSPARKS; i++)
	{
		if (drops[i].used)
			continue;
		p = i;
	}
	if (p > -1)
	{
		drops[p].pos.x = pos.x;
		drops[p].pos.y = pos.y;
		drops[p].pos.z = pos.z;
		drops[p].vel.x = (float)0;
		drops[p].vel.y = (float)0;
		drops[p].vel.z = (float)-WINDSTRIP_SPARKFALL;
		drops[p].scl = (float)WINDSTRIP_SPARKSCALE;
		drops[p].used = TRUE;
	}
}
