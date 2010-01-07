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
// cc_bodyqueue.cpp
// Bodyqueue class
//

#include "cc_local.h"

class CBody : public CHurtableEntity, public CTossProjectile, public CThinkableEntity
{
public:
	class CBodyQueue *BodyQueueList; // Queue the body belongs to

	CBody ();
	CBody (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CBody)

	bool			ParseField (const char *Key, const char *Value)
	{
		return true;
	};

	void			SaveFields (CFile &File)
	{
		CHurtableEntity::SaveFields (File);
		CThinkableEntity::SaveFields (File);
		CTossProjectile::SaveFields (File);
	};
	void			LoadFields (CFile &File);

	void TossHead (sint32 damage);

	void Pain (CBaseEntity *other, float kick, sint32 damage);
	void Die (CBaseEntity *inflictor, CBaseEntity *attacker, sint32 damage, vec3f &point);

	void Think	(); // Only done if we're a head

	bool Run ();
};

CBody::CBody () :
CBaseEntity(),
CHurtableEntity(),
BodyQueueList(NULL)
{
};

CBody::CBody (sint32 Index) :
CBaseEntity(Index),
CHurtableEntity(Index),
BodyQueueList(NULL)
{
};

IMPLEMENT_SAVE_SOURCE (CBody)

bool CBody::Run ()
{
	return (GetSvFlags() & SVF_NOCLIENT) ? false : CTossProjectile::Run();
}

void CBody::Pain (CBaseEntity *other, float kick, sint32 damage)
{
}

vec3f VelocityForDamage (sint32 damage);
void CBody::TossHead (sint32 damage)
{
	if (irandom(2))
	{
		State.GetModelIndex() = GameMedia.Gib_Head[1];
		State.GetSkinNum() = 1;		// second skin is player
	}
	else
	{
		State.GetModelIndex() = GameMedia.Gib_Skull;
		State.GetSkinNum() = 0;
	}

	State.GetFrame() = 0;
	GetMins().Set (-16, -16, 0);
	GetMaxs().Set (16);

	CanTakeDamage = false;
	GetSolid() = SOLID_NOT;
	State.GetEffects() = EF_GIB;
	State.GetSound() = 0;
	Flags |= FL_NO_KNOCKBACK;

	backOff = 1.5f;	
	Velocity += VelocityForDamage (damage);

	NextThink = level.Frame + 100 + frand()*100;

	Link ();
}

typedef std::list<CBody*, std::level_allocator<CBody*> > TBodyQueueList;
class CBodyQueue
{
public:
	TBodyQueueList OpenList; // A list of entity numbers that are currently waiting a body
	TBodyQueueList ClosedList; // A list of entity numbers that are currently being used, in order of accessed.
	// If OpenList is empty, pop the first one off of ClosedList.

	// Creates the body queue
	CBodyQueue (sint32 MaxSize);

	CBody *GrabFreeBody ();

	// Grabs a free body and uses it
	void CopyBodyToQueue (CPlayerEntity *Player);
};

CBodyQueue	*BodyQueue;

void	CBody::LoadFields (CFile &File)
{
	BodyQueueList = BodyQueue;
	
	CHurtableEntity::LoadFields (File);
	CThinkableEntity::LoadFields (File);
	CTossProjectile::LoadFields (File);
};

void CBody::Think ()
{
	// Take us out of the closed list
	BodyQueueList->ClosedList.remove (this);

	// Add us to the open list
	BodyQueueList->OpenList.push_back (this);

	// Disappear us
	State.GetModelIndex() = State.GetEffects() = 0;
	GetSvFlags() = SVF_NOCLIENT;
}

void CBody::Die (CBaseEntity *inflictor, CBaseEntity *attacker, sint32 damage, vec3f &point)
{
	if (Health < -40)
	{
		PlaySound(CHAN_BODY, SoundIndex ("misc/udeath.wav"));
		for (sint32 n = 0; n < 4; n++)
			CGibEntity::Spawn (this, GameMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
			
		State.GetOrigin().Z -= 16;
		TossHead (damage);
	}
}

CBody *CBodyQueue::GrabFreeBody ()
{
	// If there's anything in the open List..
	if (OpenList.size())
	{
		// Pop it off the front
		CBody *Body = OpenList.front();
		OpenList.pop_front();

		// Throw it in the closed list
		ClosedList.push_back (Body);
		return Body;
	}
	// Has to be something in closed list
	// Pop the first one off and return that.
	// This should, effectively, remove the last body.
	CBody *Body = ClosedList.front();
	ClosedList.pop_front();

	// Revision
	// Push this body to the end of the closed list so we get recycled last
	ClosedList.push_back (Body);
	return Body;
}

CBodyQueue::CBodyQueue (sint32 MaxSize)
{
	// Add MaxSize entities to the body queue (reserved entities)
	for (sint32 i = 0; i < MaxSize; i++)
	{
		CBody *Body = QNewEntityOf CBody ();
		Body->ClassName = "bodyqueue";
		Body->BodyQueueList = this;
		Body->GetSvFlags() = SVF_NOCLIENT;
		Body->Link ();
		Body->GetInUse() = true;

		OpenList.push_back(Body);
	}
};

void BodyQueue_Init (sint32 reserve)
{
	BodyQueue = QNew (com_levelPool, 0) CBodyQueue (reserve);
}

// Saves currently allocated body numbers
void SaveBodyQueue (CFile &File)
{
	if (!BodyQueue)
		return; // ????

	File.Write<size_t> (BodyQueue->ClosedList.size());
	for (TBodyQueueList::iterator it = BodyQueue->ClosedList.begin(); it != BodyQueue->ClosedList.end(); ++it)
		File.Write<sint32> ((*it)->gameEntity->state.number);

	File.Write<size_t> (BodyQueue->OpenList.size());
	for (TBodyQueueList::iterator it = BodyQueue->OpenList.begin(); it != BodyQueue->OpenList.end(); ++it)
		File.Write<sint32> ((*it)->gameEntity->state.number);
}

// Loads the bodyqueue numbers into allocationzzz
void LoadBodyQueue (CFile &File)
{
	if (!BodyQueue)
		return; // ????

	size_t num = File.Read <size_t> ();
	for (size_t i = 0; i < num; i++)
		BodyQueue->ClosedList.push_back (entity_cast<CBody>(g_edicts[File.Read <sint32> ()].Entity));

	num = File.Read <size_t> ();
	for (size_t i = 0; i < num; i++)
		BodyQueue->OpenList.push_back (entity_cast<CBody>(g_edicts[File.Read <sint32> ()].Entity));
}

void ShutdownBodyQueue ()
{
	if (BodyQueue)
		QDelete BodyQueue;
	BodyQueue = NULL;
}

void CopyToBodyQueue (CPlayerEntity *Player)
{
	BodyQueue->CopyBodyToQueue (Player);
}

void CBodyQueue::CopyBodyToQueue (CPlayerEntity *Player)
{
	// Grab a free body
	CBody *Body = GrabFreeBody();

	// Make sure it doesn't interpolate
	Body->State.GetEvent() = EV_OTHER_TELEPORT;

	Player->Unlink();
	Body->Unlink();

	Body->State.GetAngles() = Player->State.GetAngles();
	Body->State.GetAngles().X = 0;
	Body->State.GetEffects() = Player->State.GetEffects();
	Body->State.GetFrame() = Player->State.GetFrame();
	Body->State.GetModelIndex() = Player->State.GetModelIndex();
	Body->State.GetOldOrigin() = Player->State.GetOldOrigin();
	Body->State.GetOrigin() = Player->State.GetOrigin();
	Body->State.GetRenderEffects() = Player->State.GetRenderEffects();
	Body->State.GetSkinNum() = Player->State.GetSkinNum();

	Body->GetSvFlags() = Player->GetSvFlags();
	Body->GetMins() = Player->GetMins();
	Body->GetMaxs() = Player->GetMaxs();
	Body->GetAbsMin() = Player->GetAbsMin();
	Body->GetAbsMax() = Player->GetAbsMax();
	Body->GetSize() = Player->GetSize();
	Body->GetSolid() = Player->GetSolid();
	Body->GetClipmask() = Player->GetClipmask();
	Body->Velocity.Clear ();
	CBaseEntity *owner;
	if ((owner = Player->GetOwner()) != 0)
		Body->SetOwner (owner);

	Body->backOff = 1.0f;
	Body->CanTakeDamage = true;

	// Implied that Player is a dead-head (lol)
	if (!Player->CanTakeDamage)
		Body->TossHead (0);

	Body->Link();
}
