// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             effectcomp.cpp - Effect components module                 *
// *************************************************************************

#include <ddraw.h>
#include <d3d.h>
#include <d3drmwin.h>
#include <math.h>
#include "d3dmacs.h"
#include "d3dmath.h"

#include "revenant.h"
#include "effect.h"
#include "mappane.h"	   
#include "character.h"
#include "statusbar.h"
#include "MissileEffect.H"
#include "StripEffect.H"
#include "effectcomp.h"

// ******************
// * Storm Animator *
// ******************

void TStormAnimator::Init(PT3DAnimator anim, PS3DAnimObj o)
{
	// animation vars
	animator = anim;
	obj = o;
	animator->GetVerts(o, D3DVT_LVERTEX);

	for(int i = 0; i < size; ++i)
		storm_instance[i].used = FALSE;

	// init it!
	memset(&params, 0, sizeof(params));
}

// count up the existing particles
int TStormAnimator::GetCount()
{
	int count = 0;
	for(int i = 0; i < size; ++i)
	{
		if(storm_instance[i].used)
			++count;
	}
	return count;
}

// set the parameters
void TStormAnimator::Set(PSStormParams nparams)
{
	if(nparams->particles > size)
		nparams->particles = size;
	// do the copy!, kinna like the hustle but better
	memcpy(&params, nparams, sizeof(params));
}

// get the parameters
void TStormAnimator::Get(PSStormParams nparams)
{
	memcpy(nparams, &params, sizeof(params));
}

// create new instances
void TStormAnimator::Create()
{
	int i=0;
	// create all necessary particles
	while(GetCount() < params.particles)
	{
		if(storm_instance[i].used)
		{
			i++;
			continue;
		}
		// found a instance to be created
		storm_instance[i].used = TRUE;
		storm_instance[i].pos.x = params.pos.x + random(-(int)params.pos_spread.x, (int)params.pos_spread.x);
		storm_instance[i].pos.y = params.pos.y + random(-(int)params.pos_spread.y, (int)params.pos_spread.y);
		storm_instance[i].pos.z = params.pos.z + random(-(int)params.pos_spread.z, (int)params.pos_spread.z);
		storm_instance[i].is_particle = TRUE;
		storm_instance[i].velocity.x = params.velocity.x;
		storm_instance[i].velocity.y = params.velocity.y;
		storm_instance[i].velocity.z = params.velocity.z * ((float)random(100,150) / 100.0f);
		storm_instance[i].gravity = params.gravity;
		storm_instance[i].frame = (float)params.particle_begin;
		storm_instance[i].particle_frame_inc = params.particle_frame_inc;
		storm_instance[i].impact_frame_inc = params.impact_frame_inc;
		storm_instance[i].part_scl.x = params.particle_scale.x * ((float)random(50,150) / 100.0f);
		storm_instance[i].part_scl.y = params.particle_scale.y;
		storm_instance[i].part_scl.z = params.particle_scale.z;
		storm_instance[i].expl_scl.x = params.impact_scale.x * ((float)random(50,150) / 100.0f);
		storm_instance[i].expl_scl.y = params.impact_scale.y;
		storm_instance[i].expl_scl.z = params.impact_scale.z;
		storm_instance[i].explosion_sounded = FALSE;
		//PLAY("meteor fall");
		i++;
	}
}

void TStormAnimator::Animate()
{	
	D3DVECTOR new_pos;
	S3DPoint point;

	// run through existing particles
	for(int i = 0; i < size; ++i)
	{
		if(!storm_instance[i].used)
			continue;

		if(storm_instance[i].is_particle)
		{
			new_pos.x = storm_instance[i].pos.x + storm_instance[i].velocity.x;
			new_pos.y = storm_instance[i].pos.y + storm_instance[i].velocity.y;
			new_pos.z = storm_instance[i].pos.z + storm_instance[i].velocity.z;

			point.x = (int)new_pos.x;
			point.y = (int)new_pos.y;
			point.z = (int)new_pos.z; 
			int height = MapPane.GetWalkHeight(point);

			if(height >= (int)new_pos.z)
			{
				storm_instance[i].frame = (float)params.impact_begin;
				storm_instance[i].is_particle = FALSE;
			}
			else
			{
				storm_instance[i].pos.x = new_pos.x;
				storm_instance[i].pos.y = new_pos.y;
				storm_instance[i].pos.z = new_pos.z;
				storm_instance[i].velocity.z -= storm_instance[i].gravity;
				storm_instance[i].frame += storm_instance[i].particle_frame_inc;
				if(storm_instance[i].frame >= params.particle_end)
					storm_instance[i].frame -= (params.particle_end - params.particle_begin);
			}
		}
		else
		{
			if((int)storm_instance[i].frame >= params.impact_end)
				storm_instance[i].used = FALSE;
			else
				storm_instance[i].frame += storm_instance[i].impact_frame_inc;
		}
	}
	// create new particles
	Create();
}

void TStormAnimator::Render()
{
	for(int i = 0; i < size; ++i)
	{
		if(!storm_instance[i].used)
			continue;

		animator->ResetExtents();

		// start it out!
		obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS | OBJ3D_VERTS;
		D3DMATRIXClear(&obj->matrix);

		// facing offset rotation
		if(storm_instance[i].is_particle)
		{
			
			D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-45 * TORADIAN));		//-45
			D3DMATRIXRotateX(&obj->matrix, D3DVAL(0 * TORADIAN));		//0
			D3DMATRIXRotateY(&obj->matrix, D3DVAL(-90 * TORADIAN));		//-45
			
		}
		else
		{
			D3DMATRIXRotateZ(&obj->matrix, D3DVAL(0 * TORADIAN));		//0
			D3DMATRIXRotateX(&obj->matrix, D3DVAL(90 * TORADIAN));		//120
			D3DMATRIXRotateY(&obj->matrix, D3DVAL(0 * TORADIAN));		//0
		}

		// scaling
		if(storm_instance[i].is_particle)
		{
			obj->scl.x = storm_instance[i].part_scl.x;
			obj->scl.y = storm_instance[i].part_scl.y;
			obj->scl.z = storm_instance[i].part_scl.z;
		}
		else
		{
			obj->scl.x = storm_instance[i].expl_scl.x;
			obj->scl.y = storm_instance[i].expl_scl.x;
			obj->scl.z = storm_instance[i].expl_scl.x;
		}
		D3DMATRIXScale(&obj->matrix, &obj->scl);

		// position
		if(storm_instance[i].is_particle)
		{
			obj->pos.x = storm_instance[i].pos.x;
			obj->pos.y = storm_instance[i].pos.y;
			obj->pos.z = FIX_Z_VALUE(storm_instance[i].pos.z);
			D3DMATRIXTranslate(&obj->matrix, &obj->pos);
		}
		else
		{
			obj->pos.x = storm_instance[i].pos.x + 25;
			obj->pos.y = storm_instance[i].pos.y + 35;
			obj->pos.z = FIX_Z_VALUE(storm_instance[i].pos.z);
			D3DMATRIXTranslate(&obj->matrix, &obj->pos);
		}


		if(!(storm_instance[i].is_particle) && (storm_instance[i].explosion_sounded == FALSE))
		{
			S3DPoint pos;
			pos.x = (int)storm_instance[i].pos.x + 25;
			pos.y = (int)storm_instance[i].pos.y + 35;
			pos.z = MapPane.GetWalkHeight(pos);
		    DamageCharactersInRange(((PTEffect)animator->GetObjInst())->GetSpell()->GetInvoker(), pos, 50, METEOR_DAMAGE_MIN, METEOR_DAMAGE_MAX, DAMAGE_FIRE);
			PLAY("meteor explode");
			storm_instance[i].explosion_sounded = TRUE;
		}


		// all the following code is to set the frame
		int frame = (int)storm_instance[i].frame;
		float u;
		float u_size;
		float v=0;
		float v_size;
		if(storm_instance[i].is_particle)
		{
			u_size = (1.0f / (float)params.particle_u);
			u = u_size * (float)(frame % params.particle_u);
			
			v_size = (1.0f / (float)params.particle_v);
			v = 0;
		}
		else
		{
			u_size = (1.0f / (float)params.impact_u);
			u = u_size * (float)(frame % params.impact_u);
			
			v_size = (1.0f / (float)params.impact_v);
			v = v_size * (float)(frame / params.impact_v);
		}

		
		obj->lverts[0].tu = u; 
		obj->lverts[0].tv = v;

		obj->lverts[1].tu = u; 
		obj->lverts[1].tv = v + v_size;

		obj->lverts[2].tu = u + u_size;
		obj->lverts[2].tv = v;

		obj->lverts[3].tu = u + u_size;
		obj->lverts[3].tv = v + v_size;
		
/*		obj->lverts[3].tu = u; 
		obj->lverts[3].tv = v;

		obj->lverts[2].tu = u; 
		obj->lverts[2].tv = v + v_size;

		obj->lverts[1].tu = u + u_size;
		obj->lverts[1].tv = v;

		obj->lverts[0].tu = u + u_size;
		obj->lverts[0].tv = v + v_size;*/
		// yep it sure is alot just to do a simple thing

		animator->RenderObject(obj);
		animator->UpdateExtents();
	}
}

void TStormAnimator::RefreshZBuffer()
{
	S3DPoint map_pos, screen_pos;
	D3DVECTOR scale;

	if(params.particle_scale.x > params.impact_scale.x)
		scale.x = params.particle_scale.x;
	else
		scale.x = params.impact_scale.x;
	if(params.particle_scale.y > params.impact_scale.y)
		scale.y = params.particle_scale.y;
	else
		scale.y = params.impact_scale.y;

	// figure out the size
	int rect_u = (int)((float)params.tex_u * scale.x * 1.5f);
	int rect_v = (int)((float)params.tex_v * scale.y * 1.5f);

	for(int i = 0; i < size; ++i)
	{
		// skip unused particles
		if(!storm_instance[i].used)
			continue;

		// get the map position
		map_pos.x = (int)storm_instance[i].pos.x;
		map_pos.y = (int)storm_instance[i].pos.y;
		map_pos.z = (int)storm_instance[i].pos.z;

		// convert it to screen position
		WorldToScreen(map_pos, screen_pos);
		RestoreZ(screen_pos.x - (rect_u / 2), screen_pos.y - (rect_v / 2), rect_u, rect_v);
		//Display->Box(screen_pos.x - (rect_u / 2), screen_pos.y - (rect_v / 2), rect_u, rect_v);
	}
}

// ****************************
// * The SubParticle Animator *
// ****************************

void TSubParticleAnimator::Set(PSSubParticleParams nparams)
{
	// copy the new parameters in
	memcpy(&params, nparams, sizeof(SSubParticleParams));
}

void TSubParticleAnimator::Get(PSSubParticleParams nparams)
{
	// copy the new parameters in
	memcpy(nparams, &params, sizeof(SSubParticleParams));
}

int TSubParticleAnimator::GetCount()
{
	// get the total number of active particles
	int count = 0;

	for(int i = 0; i < max_particles; ++i)
	{
		if(particle[i].used)
			++count;
	}

	return count;
}

void TSubParticleAnimator::Animate()
{
	// add new particles if necessary
	int p;
	if(params.particles > max_particles)
		p = max_particles;
	else
		p = params.particles;

	int size = p - GetCount();
	for(int i = 0; i < size; ++i)
		Create();

	for(i = 0; i < max_particles; ++i)
	{
		// skip unused particles
		if(!particle[i].used)
			continue;

		// do animation!

		// change flicker status
		particle[i].flicker_status = random(0, 1);

		// change the life
		--particle[i].life;
		if(particle[i].life < 0)
			particle[i].used = FALSE;

		// change the scale
		particle[i].scale.x *= particle[i].scale_dec.x;
		particle[i].scale.y *= particle[i].scale_dec.y;
		particle[i].scale.z *= particle[i].scale_dec.z;

		// change the position
		particle[i].pos.x += particle[i].velocity.x;
		particle[i].pos.y += particle[i].velocity.y;
		particle[i].pos.z += particle[i].velocity.z;

		// change the velocity
		particle[i].velocity.z -= particle[i].gravity;
	}
}

void TSubParticleAnimator::Init(PT3DAnimator anim, PS3DAnimObj o)
{
	// initialize the particle parameters

	animator = anim;
	obj = o;

	params.particles = SUBPARTICLE_MAX_PARTICLE;

	params.pos.x = 0.0f;
	params.pos.y = 0.0f;
	params.pos.z = 0.0f;

	params.pos_spread.x = 0.0f;
	params.pos_spread.y = 0.0f;
	params.pos_spread.z = 0.0f;

	params.scale.x = 1.0f;
	params.scale.y = 1.0f;
	params.scale.z = 1.0f;

	params.scale_dec.x = 0.0f;
	params.scale_dec.y = 0.0f;
	params.scale_dec.z = 0.0f;

	params.scale_spread.x = 0.0f;
	params.scale_spread.y = 0.0f;
	params.scale_spread.z = 0.0f;

	params.velocity.x = 0.0f;
	params.velocity.y = 0.0f;
	params.velocity.z = 0.0f;

	params.velocity_dir.x = 0.0f;
	params.velocity_dir.y = 0.0f;
	params.velocity_dir.z = 0.0f;

	params.velocity_spread.x = 1.0f;
	params.velocity_spread.y = 1.0f;
	params.velocity_spread.z = 1.0f;

	params.gravity = 1.0f;

	params.min_life = 10;
	params.max_life = 20;

	params.flicker = FALSE;
	params.flicker_size = 1.0f;
}

void TSubParticleAnimator::Create()
{
	// check for creation
	int roll = random(1, 100);
	if(roll > params.chance)
		return;
		
	for(int i = 0; i < max_particles; ++i)
	{
		// skip existing particles
		if(particle[i].used)
			continue;

		// mark it as used
		particle[i].used = TRUE;

		// get the position
		particle[i].pos.x = params.pos.x + random((int)-params.pos_spread.x, (int)params.pos_spread.x);
		particle[i].pos.y = params.pos.y + random((int)-params.pos_spread.y, (int)params.pos_spread.y);
		particle[i].pos.z = params.pos.z + random((int)-params.pos_spread.z, (int)params.pos_spread.z);

		// get the scale
		particle[i].scale_dec = params.scale_dec;
		particle[i].scale.x = params.scale.x + random((int)-params.scale_spread.x, (int)params.scale_spread.x);
		particle[i].scale.y = params.scale.y + random((int)-params.scale_spread.y, (int)params.scale_spread.y);
		particle[i].scale.z = params.scale.z + random((int)-params.scale_spread.z, (int)params.scale_spread.z);

		// get the velocity
		float vx, vy, vz;

		if(params.velocity_dir.x < 0)
			vx = (float)random((int)-params.velocity_spread.x, 0);
		else if(params.velocity_dir.x == 0)
			vx = (float)random((int)-params.velocity_spread.x, (int)params.velocity_spread.x);
		else
			vx = (float)random(0, (int)params.velocity_spread.x);

		if(params.velocity_dir.y < 0)
			vy = (float)random((int)-params.velocity_spread.y, 0);
		else if(params.velocity_dir.y == 0)
			vy = (float)random((int)-params.velocity_spread.y, (int)params.velocity_spread.y);
		else
			vy = (float)random(0, (int)params.velocity_spread.y);

		if(params.velocity_dir.z < 0)
			vz = (float)random((int)-params.velocity_spread.z, 0);
		else if(params.velocity_dir.x == 0)
			vz = (float)random((int)-params.velocity_spread.z, (int)params.velocity_spread.z);
		else
			vz = (float)random(0, (int)params.velocity_spread.z);

		particle[i].velocity.x = params.velocity.x + vx;
		particle[i].velocity.y = params.velocity.y + vy;
		particle[i].velocity.z = params.velocity.z + vz;

		// get the gravity
		particle[i].gravity = params.gravity;

		// get the life
		particle[i].life = random(params.min_life, params.max_life);

		// get the flicker
		particle[i].flicker = params.flicker;
		particle[i].flicker_status = random(0, 1);
		particle[i].flicker_size = params.flicker_size;

		// done!
		break;
	}
}

void TSubParticleAnimator::Render()
{
	for(int i = 0; i < max_particles; ++i)
	{
		if(!particle[i].used)
			continue;

		animator->ResetExtents();
		
		// rotate, scale, position, and use absolute position
		obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS;

		D3DMATRIXClear(&obj->matrix);

		// rotate the to face user
		D3DMATRIXRotateZ(&obj->matrix, (float)(-M_PI / 3.0f));
		D3DMATRIXRotateX(&obj->matrix, (float)(-M_PI / 4.0f));
		D3DMATRIXRotateY(&obj->matrix, 0);

		// scale the thing
		if(particle[i].flicker && particle[i].flicker_status)
		{
			obj->scl.x = particle[i].scale.x * particle[i].flicker_size;
			obj->scl.y = particle[i].scale.y * particle[i].flicker_size;
			obj->scl.z = particle[i].scale.z * particle[i].flicker_size;
		}
		else
		{
			obj->scl.x = particle[i].scale.x;
			obj->scl.y = particle[i].scale.y;
			obj->scl.z = particle[i].scale.z;
		}
		D3DMATRIXScale(&obj->matrix, &obj->scl);

		// position the thing
		obj->pos.x = particle[i].pos.x;
		obj->pos.y = particle[i].pos.y;
		obj->pos.z = FIX_Z_VALUE(particle[i].pos.z);
		D3DMATRIXTranslate(&obj->matrix, &obj->pos);

		animator->RenderObject( obj);

		animator->UpdateExtents();
	}
}

void TSubParticleAnimator::RefreshZBuffer(int tex_u, int tex_v)
{
	S3DPoint map_pos, screen_pos;
	int rect_u, rect_v;

	for(int i = 0; i < max_particles; ++i)
	{
		// skip unused particles
		if(!particle[i].used)
			continue;

		// figure the size out
		rect_u = (int)((float)tex_u * particle[i].scale.x * 1.5f);
		rect_v = (int)((float)tex_v * particle[i].scale.y * 1.5f);
		if(particle[i].flicker && particle[i].flicker_status)
		{
			rect_u = (int)((float)rect_u * particle[i].flicker_size);
			rect_v = (int)((float)rect_v * particle[i].flicker_size);
		}

		// get the map position
		map_pos.x = (int)particle[i].pos.x;
		map_pos.y = (int)particle[i].pos.y;
		map_pos.z = (int)particle[i].pos.z;

		// convert it to screen position
		WorldToScreen(map_pos, screen_pos);

		// refresh the zbuffer
		RestoreZ(screen_pos.x - (rect_u / 2), screen_pos.y - (rect_v / 2), rect_u, rect_v);
	}
}

// **********************
// * Shockwave Animator *
// **********************

void TShockAnimator::Set(PSShockParam nparams)
{
	// copy the new parameters in
	memcpy(&params, nparams, sizeof(SShockParam));

	init_scale = params.scale;

	if(!(params.flags & SHOCKWAVE_FLAG_SHRINK))
		grow = SHOCKWAVE_GROW;
}

void TShockAnimator::Init(PT3DAnimator anim, PS3DAnimObj o, int rings, int vertices)
{
	// get the animator stuff
	animator = anim;
	obj = o;
	animator->GetVerts(o, D3DVT_LVERTEX);

	// init done to false - duh!
	done = FALSE;

	// ring info
	ring_count = rings;
	vertex_count = vertices;
	ring = new D3DCOLOR[ring_count];
}

void TShockAnimator::Animate()
{
	if(done == TRUE)
		return;

	// scaling
	params.scale.x *= params.scale_factor.x;
	params.scale.y *= params.scale_factor.y;
	params.scale.z *= params.scale_factor.z;

	// check growth, and shrinking
	if((params.scale.x > params.max_size.x || params.scale.y > params.max_size.y || params.scale.z > params.max_size.z) && grow == SHOCKWAVE_GROW)
	{
		if(params.flags & SHOCKWAVE_FLAG_SHRINK)
		{
			grow = SHOCKWAVE_SHRINK;
			params.scale_factor.x = params.shrink_factor.x;
			params.scale_factor.y = params.shrink_factor.y;
			params.scale_factor.z = params.shrink_factor.z;
		}
		else
			done = TRUE;
	}
	else if((params.scale.x < params.min_size.x || params.scale.y < params.min_size.y || params.scale.z < params.min_size.z) && grow == SHOCKWAVE_SHRINK)
	{
		done = TRUE;
	}
}

void TShockAnimator::RefreshZBuffer(int tex_u, int tex_v)
{
	// z buffer refreshing
	int rect_u, rect_v;
	S3DPoint map, screen;

	float max = params.scale.x;
	if(params.scale.y > max)
		max = params.scale.y;
	if(params.scale.z > max)
		max = params.scale.z;

	rect_u = (int)(tex_u * max); 
	rect_v = (int)(tex_v * max);

	map.x = (int)params.pos.x;
	map.y = (int)params.pos.y;
	map.z = (int)params.pos.z;

	WorldToScreen(map, screen);

	RestoreZ(screen.x - (rect_u / 2), screen.y - (rect_v / 2), rect_u, rect_v);
}

void TShockAnimator::Render()
{
	if(done == TRUE)
		return;

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

	animator->ResetExtents();

	obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS | OBJ3D_VERTS;

	D3DMATRIXClear(&obj->matrix);

	// rotation
	D3DMATRIXRotateX(&obj->matrix, params.rot.x);
	D3DMATRIXRotateY(&obj->matrix, params.rot.y);
	D3DMATRIXRotateZ(&obj->matrix, params.rot.z);

	// scaling
	D3DVECTOR scl;
	scl.x = params.scale.x;
	scl.y = params.scale.y;
	scl.z = params.scale.z;
	D3DMATRIXScale(&obj->matrix, &scl);
	
	// translation
	D3DVECTOR pos;
	pos.z = FIX_Z_VALUE(params.pos.z);
	pos.x = params.pos.x;
	pos.y = params.pos.y;
	D3DMATRIXTranslate(&obj->matrix, &pos);

	float alpha_blend_factor;
	if(params.flags & SHOCKWAVE_FLAG_FADE)
		alpha_blend_factor =  (params.max_size.x - params.scale.x) / (params.max_size.x - init_scale.x);
	
	for(int i = 0; i < ring_count; ++i)
	{
		float r = (RGBA_GETRED(ring[i])   / 255.0f);
		float g = (RGBA_GETGREEN(ring[i]) / 255.0f);
		float b = (RGBA_GETBLUE(ring[i])  / 255.0f);
		float a = (RGBA_GETALPHA(ring[i]) / 255.0f);

		if(params.flags & SHOCKWAVE_FLAG_FADE)
		{
			//r *= alpha_blend_factor;
			//g *= alpha_blend_factor;
			//b *= alpha_blend_factor;
			a *= alpha_blend_factor;
		}
		for(int j = 0; j < vertex_count; ++j)
		{
			obj->lverts[(i * vertex_count) + j].color = D3DRGBA(r, g, b, a);
		}
	}
	
	// set the new render flags
	Device2->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
	Device2->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	Device2->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	Device2->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);

	animator->RenderObject(obj);

	animator->UpdateExtents();

	// restore the old render flags
	Device2->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, oldblendstate);
	Device2->SetRenderState(D3DRENDERSTATE_CULLMODE, oldcullmode);
	Device2->SetRenderState(D3DRENDERSTATE_SRCBLEND, oldsrcmode);
	Device2->SetRenderState(D3DRENDERSTATE_DESTBLEND, olddestmode);
}

// **********************
// * The Strip Animator *
// **********************

TStripAnimator::TStripAnimator( PS3DAnimObj myobj, int maxp)
{
	int v, foff;
	float rangle;

	pbeg = pend = curpoints = 0;

	float zrot[8] = { -60, -60, 0, 60, 60, 60, 0, -60 };
	for (v = 0; v < 8; v++)
	{
		rangle = D3DVAL( zrot[v] * TORADIAN);
		myx[v] = D3DVAL( cos( rangle));
		myz[v] = D3DVAL( sin( rangle));
	}

	obj = myobj;

	maxpoints = maxp;
	SetWidth( 10, 5);

	uoff = 0;
	SetTextureRange( 0, 1);

	points = new D3DVECTOR[maxpoints];

	// Paraphrased GetVerts
	obj->numverts = (maxpoints * 2);	// Updated in AddPoint
	obj->lverts = new D3DLVERTEX[obj->numverts];
	obj->verttype = D3DVT_LVERTEX;

	// Paraphrased GetFaces
	obj->numfaces = ((maxpoints - 1) * 2);
	if (!obj->faces)
		obj->faces = new S3DFace[obj->numfaces];
	obj->texfaces[0] = 0;
	obj->texfaces[1] = 0;
	obj->numtexfaces[0] = 0;
	obj->numtexfaces[1] = obj->numfaces;

	// Build Vertex index for each Face
	for (v = 0; v < obj->numfaces; v++)
	{
		switch (v % 4)
		{
			case 0:
				obj->faces[v].v1 = 2;
				obj->faces[v].v2 = 0;
				obj->faces[v].v3 = 3;
				break;
			case 1:
				obj->faces[v].v1 = 1;
				obj->faces[v].v2 = 3;
				obj->faces[v].v3 = 0;
				break;
			case 2:
				obj->faces[v].v1 = 2;
				obj->faces[v].v2 = 4;
				obj->faces[v].v3 = 3;
				break;
			case 3:
				obj->faces[v].v1 = 5;
				obj->faces[v].v2 = 3;
				obj->faces[v].v3 = 4;
				break;
		}

		// Offset index every two Faces
		if (foff = (4 * (v / 4)))
		{
			obj->faces[v].v1 += foff;
			obj->faces[v].v2 += foff;
			obj->faces[v].v3 += foff;
		}
	}
};

void TStripAnimator::SetTextureRange( D3DVALUE startu, D3DVALUE endu)
{
	ubeg = startu;
	uend = endu;
	ufrequency = (float)((uend - ubeg) / maxpoints);
	if (uoff < ubeg)
		uoff = ubeg;
	if (uoff > uend)
		uoff = uend;
}

void TStripAnimator::ScrollTexture( float deltau)
{
	uoff += deltau;

	if (uoff < ubeg)
		uoff = uend - (ubeg - uoff);
	if (uoff > uend)
		uoff = ubeg + (uoff - uend);
}

void TStripAnimator::GenerateStrip( int angle)
{
	int pnt, v1, v2, w, z;
	float rx, rz;

	angle = (angle + 224) % 256; // 16

	int ang = (angle / 32);
	float sz, tfreq;

	if (startsize != endsize)
		sz = (float)(((float)(startsize - endsize) / maxpoints) * curpoints) / (maxpoints - 1);

	for (pnt = 0; pnt < curpoints; pnt++)
	{
		z = (startsize + (int)((curpoints - pnt) * sz)) / 2;
		if (pnt)
			w = (pnt + pbeg) % curpoints;
		else
			w = 0;

		v1 = (pnt * 2);
		v2 = v1 + 1;

		rx = (z * myx[ang]); 
		rz = (z * myz[ang]);

		obj->verts[v1].y = points[pnt].y;
		obj->verts[v1].x = points[w].x - rx;
		obj->verts[v1].z = points[w].z - rz;

		obj->verts[v2].y = points[pnt].y;
		obj->verts[v2].x = points[w].x + rx;
		obj->verts[v2].z = points[w].z + rz;

		tfreq = D3DVALUE( (pnt * ufrequency) + uoff);
		if (tfreq < ubeg)
			tfreq = uend - (ubeg - tfreq);
		if (tfreq > uend)
			tfreq = ubeg + (tfreq - uend);
			
		obj->verts[v1].tu = tfreq;
		obj->verts[v2].tu = tfreq;
	}

	obj->flags = OBJ3D_MATRIX | OBJ3D_ROT1 | OBJ3D_VERTS | OBJ3D_FACES | OBJ3D_OWNSVERTS | OBJ3D_OWNSFACES;
    D3DMATRIXClear( &obj->matrix);
}

void TStripAnimator::AddPoint( D3DVECTOR *p)
{
	if (curpoints >= maxpoints)
		return;

    points[curpoints].x = p->x;
    points[curpoints].y = p->y;
    points[curpoints].z = p->z;

	obj->verts[(curpoints * 2)].z = D3DVAL( startsize / 2);
	obj->verts[(curpoints * 2)].tv = 0;	
	obj->verts[(curpoints * 2)].tu = 1;

	obj->verts[(curpoints * 2) + 1].z = D3DVAL( startsize / -2);
	obj->verts[(curpoints * 2) + 1].tv = 1;	
	obj->verts[(curpoints * 2) + 1].tu = 1;

	curpoints++;
	if (pend >= pbeg)
		pend++;

	// Update our verts and faces
	obj->numverts = (curpoints * 2);
	obj->numfaces = ((curpoints - 1) * 2);
	obj->numtexfaces[1] = obj->numfaces;
}

// Deletes point from the end of the point array
void TStripAnimator::DelEndPoint()
{
	if (curpoints < 3)	// Don't kill last segment
		return;

	curpoints--;
	if (pbeg == curpoints)
		pbeg--;
	if (pend == curpoints)
		pend--;

	// Update our verts and faces
	obj->numverts = (curpoints * 2);	// Updated in AddPoint
	obj->numfaces = ((curpoints - 1) * 2);
	obj->numtexfaces[1] = obj->numfaces;
}

// Deletes point from the start of the point array
void TStripAnimator::DelStartPoint()
{
	int i;

	if (curpoints < 1)	// Don't kill last segment
		return;

	curpoints--;
	if (pbeg == curpoints)
		pbeg--;
	if (pend == curpoints)
		pend--;

	obj->numverts = (curpoints * 2);	// Updated in AddPoint
	obj->numfaces = ((curpoints - 1) * 2);
	obj->numtexfaces[1] = obj->numfaces;

	for (i = 0; i < curpoints; i++)
	    points[i] = points[(i + 1)];
}

void TStripAnimator::AdvanceStrip()
{
	if (!pbeg)
		pbeg = curpoints;
	pbeg--;
	if (!pend)
		pend = curpoints;
	pend--;
}

void TStripAnimator::ReverseStrip()
{
	pbeg++;
	if (pbeg == curpoints)
		pbeg = 0;
	pend++;
	if (pend == curpoints)
		pend = 0;
}

void TStripAnimator::SetWidth( int start, int end) {
	startsize = start;
	endsize = end;
}

void TStripAnimator::AddUpdateRects( LPD3DRECT drawextents) {
	SRect r;

	r.left = -5;
	r.top = -10;
	r.right = 15 * 16;
	r.bottom = 10;

//	drawextents->x1 += r.left;
//	drawextents->y1 += r.top;
//	drawextents->x2 += r.right;
//	drawextents->y2 += r.bottom;

	// WorldToScreen( RS3DPoint pos, int &x, int &y);
	// WorldToScreen( RS3DPoint pos, RS3DPoint spos);
}

void TParticleSystem::Init(PT3DAnimator a, PS3DAnimObj o, S3DPoint s, BOOL move, float facing_angle)
{
	// set up the basic info
	animator = a;
	object = o;
	size = s;
	check_move = move;
	facing = facing_angle;
}

void TParticleSystem::Animate()
{
	// cycle through and do life checking, and moving if necessary
	// don't run this if you don't want to use lifespans
	for(int i = 0; i < count; ++i)
	{
		if(!particle[i].used)
			continue;

		if(particle[i].life >= particle[i].life_span)
		{
			particle[i].used = FALSE;
			continue;
		}
		++particle[i].life;

		if(check_move)
		{
			particle[i].pos.x += particle[i].vel.x;
			particle[i].pos.y += particle[i].vel.y;
			particle[i].pos.z += particle[i].vel.z;

			particle[i].vel.x *= particle[i].acc.x;
			particle[i].vel.y *= particle[i].acc.y;
			particle[i].vel.z *= particle[i].acc.z;
		}
	}
}

void TParticleSystem::Render()
{
	// basically cycle through the particles and render them...duh!
	for(int i = 0; i < count; ++i)
	{
		if(!particle[i].used)
			continue;

		animator->ResetExtents();

		object->flags = OBJ3D_MATRIX;
		D3DMATRIXClear(&object->matrix);

		D3DMATRIXRotateX(&object->matrix, (float)(particle[i].rot.x * TORADIAN));
		D3DMATRIXRotateY(&object->matrix, (float)(particle[i].rot.y * TORADIAN));
		D3DMATRIXRotateZ(&object->matrix, (float)(particle[i].rot.z * TORADIAN));
		D3DMATRIXRotateX(&object->matrix, -(float)(M_PI / 2.0));
		D3DMATRIXRotateZ(&object->matrix, -(float)(M_PI / 4.0));
		D3DMATRIXRotateZ(&object->matrix, facing);

		object->scl = particle[i].scl;
		D3DMATRIXScale(&object->matrix, &object->scl);

		object->pos.x = particle[i].pos.x;
		object->pos.y = particle[i].pos.y;
		object->pos.z = FIX_Z_VALUE(particle[i].pos.z);
		D3DMATRIXTranslate(&object->matrix, &object->pos);

		animator->RenderObject(object);
		animator->UpdateExtents();
	}
}

void TParticleSystem::RefreshZBuffer()
{
	S3DPoint effect, pos, screen;

	// get the effect position
	((PTEffect)animator->GetObjInst())->GetPos(effect);

	// again cycle through the particles and update their zbuffers
	for(int i = 0; i < count; ++i)
	{
		if(!particle[i].used)
			continue;
		pos.x = effect.x + (int)particle[i].pos.x;
		pos.y = effect.y + (int)particle[i].pos.y;
		pos.z = effect.z + (int)particle[i].pos.z;

		WorldToScreen(pos, screen);

		RestoreZ(screen.x - (int)(size.x * particle[i].scl.x / 2), screen.y - (int)(size.y * particle[i].scl.y / 2), (int)(size.x * particle[i].scl.x), (int)(size.y * particle[i].scl.y));
	}
}

void TParticleSystem::Add(PSParticleSystemInfo p)
{
	// look for an unused particle
	for(int i = 0; i < count; ++i)
	{
		if(particle[i].used)
			continue;

		// add in a particle where possible
		particle[i].used = TRUE;
		particle[i].life = 0;

		particle[i].life_span = p->life_span;

		particle[i].temp = p->temp;

		particle[i].vel = p->vel;
		particle[i].pos = p->pos;
		particle[i].scl = p->scl;
		particle[i].rot = p->rot;
		particle[i].acc = p->acc;

		return;
	}
}

