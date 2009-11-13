/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
This source file is contained as part of CleanCode Quake2, a project maintained
by Paril, to 'clean up' and make Quake2 an easier source base to read and work with.

You may use any part of this code to help create your own bases and own mods off
this code if you wish. It is under the same license as Quake 2 source (as above),
therefore you are free to have to fun with it. All I ask is you email me so I can
list the mod on my page for CleanCode Quake2 to help get the word around. Thanks.
*/

//
// cc_junk_entities.cpp
// Entities that share a "Junk" class
//

#include "cc_local.h"

#define MAX_JUNK 35

class CJunkList
{
public:
	sint32				NumAllocatedJunk;

	// OpenList = Junk that is ready to use
	// ClosedList = Junk that is already in use
	std::list<sint32, std::level_allocator<sint32> >	OpenList, ClosedList;

	CJunkList () :
	NumAllocatedJunk (0),
	OpenList(),
	ClosedList()
	{
	};

	template <class JunkClassType>
	// Re-sets the entity structure if the
	// entity is not already freed
	JunkClassType *ReallocateEntity (sint32 number)
	{
		JunkClassType *Junk;
		if (g_edicts[number].Entity)
		{
			g_edicts[number].Entity->Free();
			QDelete g_edicts[number].Entity;
			Junk = QNew (com_levelPool, 0) JunkClassType(number);

_CC_DISABLE_DEPRECATION
			G_InitEdict (&g_edicts[number]);
_CC_ENABLE_DEPRECATION

			g_edicts[number].Entity = Junk;
		}
		else
		{
			Junk = QNew (com_levelPool, 0) JunkClassType(number);

_CC_DISABLE_DEPRECATION
			G_InitEdict (&g_edicts[number]);
_CC_ENABLE_DEPRECATION

			g_edicts[number].Entity = Junk;
		}
		return Junk;
	};

	template <class JunkClassType>
	JunkClassType *AllocateEntity ()
	{
		return QNew (com_levelPool, 0) JunkClassType;
	};

	template <class JunkClassType>
	JunkClassType *GetFreeJunk ()
	{
		// Check the open list
		if (OpenList.size())
		{
			// Pop it off the front
			sint32 number = OpenList.front();
			OpenList.pop_front();

			// Throw it in the closed list
			ClosedList.push_back (number);
			return ReallocateEntity<JunkClassType>(number);
		}
		else if (NumAllocatedJunk < MAX_JUNK)
		{
			// Create it
			JunkClassType *Junk = AllocateEntity<JunkClassType>();

			// Throw into closed list
			ClosedList.push_back (Junk->State.GetNumber());

			NumAllocatedJunk++;
			return Junk;
		}
		else
		{
			if (ClosedList.size())
			{
				// Has to be something in closed list
				// Pop the first one off and return that.
				// This should, effectively, remove the last body.
				sint32 number = ClosedList.front();
				ClosedList.pop_front();

				// Revision
				// Push this body to the end of the closed list so we get recycled last
				ClosedList.push_back (number);

				JunkClassType *Junk = ReallocateEntity<JunkClassType>(number);
				Junk->State.GetEvent() = EV_OTHER_TELEPORT;

				return Junk;
			}
		}
		return NULL;
	};
};

CJunkList *JunkList;

void Init_Junk()
{
	JunkList = QNew (com_levelPool, 0) CJunkList;
}

void Shutdown_Junk ()
{
	if (JunkList)
		QDelete JunkList;
	JunkList = NULL;
}

CJunkEntity::CJunkEntity () :
CBaseEntity()
{
	EntityFlags |= ENT_JUNK;
};
CJunkEntity::CJunkEntity (sint32 Index) :
CBaseEntity(Index)
{
	EntityFlags |= ENT_JUNK;
};

void CJunkEntity::Die ()
{
	// Take us out of the closed list
	JunkList->ClosedList.remove (State.GetNumber());

	// Add us to the open list
	JunkList->OpenList.push_back (State.GetNumber());

	// Disappear us
	State.GetModelIndex() = 0;
	State.GetEffects() = 0;
	GetSvFlags() = SVF_NOCLIENT;
}

CGibEntity::CGibEntity () :
CBaseEntity(),
CTossProjectile(),
CJunkEntity()
{
};
CGibEntity::CGibEntity (sint32 Index) :
CBaseEntity(Index),
CTossProjectile(Index),
CJunkEntity(Index)
{
};

/*
=================
Misc functions
=================
*/
vec3f VelocityForDamage (sint32 damage)
{
	return vec3f(100.0f * crand(), 100.0f * crand(), 200 + 100 * frand()) * (damage < 50) ? 0.7f : 1.2f;
}

void ClipGibVelocity (CPhysicsEntity *ent)
{
	if (ent->Velocity.X < -300)
		ent->Velocity.X = -300;
	else if (ent->Velocity.X > 300)
		ent->Velocity.X = 300;
	if (ent->Velocity.Y < -300)
		ent->Velocity.Y = -300;
	else if (ent->Velocity.Y > 300)
		ent->Velocity.Y = 300;
	if (ent->Velocity.Z < 200)
		ent->Velocity.Z = 200;	// always some upwards
	else if (ent->Velocity.Z > 500)
		ent->Velocity.Z = 500;
}

bool CGibEntity::Run ()
{
	return CBounceProjectile::Run();
}

void CGibEntity::Think ()
{
	Die ();
}

void CGibEntity::Spawn (CBaseEntity *Owner, MediaIndex gibIndex, sint32 damage, sint32 type)
{
	CGibEntity *Junk = JunkList->GetFreeJunk<CGibEntity>();

	vec3f size = Owner->GetSize() * 0.5f;

	vec3f origin = Owner->GetAbsMin() + size;

	Junk->State.GetOrigin().Set (origin.X + crand() * size.X,
							origin.Y + crand() * size.Y,
							origin.Z + crand() * size.Z);

	Junk->State.GetModelIndex() = gibIndex;
	Junk->GetSvFlags() = SVF_DEADMONSTER;
	Junk->GetMins().Clear ();
	Junk->GetMaxs().Clear ();
	Junk->GetSolid() = SOLID_NOT;
	Junk->State.GetEffects() = EF_GIB;

	Junk->backOff = (type == GIB_ORGANIC) ? 1.0f : 1.5f;
	float vscale = (type == GIB_ORGANIC) ? 0.5f : 1.0f;

	vec3f vd = VelocityForDamage (damage);

	vec3f velocity = ((Owner->EntityFlags & ENT_PHYSICS) ? (entity_cast<CPhysicsEntity>(Owner)->Velocity) : vec3fOrigin);
	velocity.MultiplyAngles (vscale, vd);
	Junk->Velocity = velocity;
	ClipGibVelocity (Junk);

	Junk->AngularVelocity.Set (crand()*600, crand()*600, crand()*600);
	Junk->NextThink = level.Frame + 100 + frand()*100;

	Junk->Link ();
}
