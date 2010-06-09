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
// cc_insane.cpp
// Insane marine
//

#include "cc_local.h"
#include "m_insane.h"
#include "cc_insane.h"

/**
\enum	

\brief	Values that represent spawnflags pertaining to CInsane. 
**/
enum
{
	INSANE_CRAWL		= BIT(2),
	INSANE_CRUCIFIED	= BIT(3),
	INSANE_STAND_GROUND = BIT(4),
	INSANE_ALWAYS_STAND = BIT(5)
};

CInsane::CInsane (uint32 ID) :
CMonster (ID)
{
	Scale = MODEL_SCALE;
	MonsterName = "Insane Dude";
}

void CInsane::Fist ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_FIST], 255, ATTN_IDLE);
}

void CInsane::Shake ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SHAKE], 255, ATTN_IDLE);
}

void CInsane::Moan ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_MOAN], 255, ATTN_IDLE);
}

void CInsane::Scream ()
{
	Entity->PlaySound (CHAN_VOICE, SoundScream[irandom(8)], 255, ATTN_IDLE);
}

CFrame InsaneFramesStandNormal [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CInsane::CheckDown))
};
CAnim InsaneMoveStandNormal (FRAME_stand60, FRAME_stand65, InsaneFramesStandNormal, &CMonster::Stand);

CFrame InsaneFramesStandInsane [] =
{
	CFrame (&CMonster::AI_Stand,	0,	ConvertDerivedFunction(&CInsane::Shake)),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0,	ConvertDerivedFunction(&CInsane::CheckDown))
};
CAnim InsaneMoveStandInsane (FRAME_stand65, FRAME_stand94, InsaneFramesStandInsane, &CMonster::Stand);

CFrame InsaneFramesUpToDown [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CInsane::Moan)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),

	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),

	CFrame (&CMonster::AI_Move,	2.7f),
	CFrame (&CMonster::AI_Move,	4.1f),
	CFrame (&CMonster::AI_Move,	6),
	CFrame (&CMonster::AI_Move,	7.6f),
	CFrame (&CMonster::AI_Move,	3.6f),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CInsane::Fist)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),

	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CInsane::Fist)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveUpToDown (FRAME_stand1, FRAME_stand40, InsaneFramesUpToDown, ConvertDerivedFunction(&CInsane::OnGround));

CFrame InsaneFramesDownToUp [] =
{
	CFrame (&CMonster::AI_Move,	-0.7f),			// 41
	CFrame (&CMonster::AI_Move,	-1.2f),			// 42
	CFrame (&CMonster::AI_Move,	-1.5f),		// 43
	CFrame (&CMonster::AI_Move,	-4.5f),		// 44
	CFrame (&CMonster::AI_Move,	-3.5f),			// 45
	CFrame (&CMonster::AI_Move,	-0.2f),			// 46
	CFrame (&CMonster::AI_Move,	0),			// 47
	CFrame (&CMonster::AI_Move,	-1.3f),			// 48
	CFrame (&CMonster::AI_Move,	-3),				// 49
	CFrame (&CMonster::AI_Move,	-2),			// 50
	CFrame (&CMonster::AI_Move,	0),				// 51
	CFrame (&CMonster::AI_Move,	0),				// 52
	CFrame (&CMonster::AI_Move,	0),				// 53
	CFrame (&CMonster::AI_Move,	-3.3f),			// 54
	CFrame (&CMonster::AI_Move,	-1.6f),			// 55
	CFrame (&CMonster::AI_Move,	-0.3f),			// 56
	CFrame (&CMonster::AI_Move,	0),				// 57
	CFrame (&CMonster::AI_Move,	0),				// 58
	CFrame (&CMonster::AI_Move,	0)				// 59
};
CAnim InsaneMoveDownToUp (FRAME_stand41, FRAME_stand59, InsaneFramesDownToUp, &CMonster::Stand);

CFrame InsaneFramesJumpDown [] =
{
	CFrame (&CMonster::AI_Move,	0.2f),
	CFrame (&CMonster::AI_Move,	11.5f),
	CFrame (&CMonster::AI_Move,	5.1f),
	CFrame (&CMonster::AI_Move,	7.1f),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveJumpDown (FRAME_stand96, FRAME_stand100, InsaneFramesJumpDown, ConvertDerivedFunction(&CInsane::OnGround));


CFrame InsaneFramesDown [] =
{
	CFrame (&CMonster::AI_Move,	0),		// 100
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		// 110
	CFrame (&CMonster::AI_Move,	-1.7f),
	CFrame (&CMonster::AI_Move,	-1.6f),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0, ConvertDerivedFunction(&CInsane::Fist)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		// 120
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		// 130
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0, ConvertDerivedFunction(&CInsane::Moan)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		// 140
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		// 150
	CFrame (&CMonster::AI_Move,	0.5f),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	-0.2f, ConvertDerivedFunction(&CInsane::Scream)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0.2f),
	CFrame (&CMonster::AI_Move,	0.4f),
	CFrame (&CMonster::AI_Move,	0.6f),
	CFrame (&CMonster::AI_Move,	0.8f),
	CFrame (&CMonster::AI_Move,	0.7f),
	CFrame (&CMonster::AI_Move,	0, ConvertDerivedFunction(&CInsane::CheckUp))		// 160
};
CAnim InsaneMoveDown (FRAME_stand100, FRAME_stand160, InsaneFramesDown, ConvertDerivedFunction(&CInsane::OnGround));

CFrame InsaneFramesWalkNormal [] =
{
	CFrame (&CMonster::AI_Walk,	0, ConvertDerivedFunction(&CInsane::Scream)),
	CFrame (&CMonster::AI_Walk,	2.5f),
	CFrame (&CMonster::AI_Walk,	3.5f),
	CFrame (&CMonster::AI_Walk,	1.7f),
	CFrame (&CMonster::AI_Walk,	2.3f),
	CFrame (&CMonster::AI_Walk,	2.4f),
	CFrame (&CMonster::AI_Walk,	2.2f),
	CFrame (&CMonster::AI_Walk,	4.2f),
	CFrame (&CMonster::AI_Walk,	5.6f),
	CFrame (&CMonster::AI_Walk,	3.3f),
	CFrame (&CMonster::AI_Walk,	2.4f),
	CFrame (&CMonster::AI_Walk,	0.9f),
	CFrame (&CMonster::AI_Walk,	0)
};
CAnim InsaneMoveWalkNormal (FRAME_walk27, FRAME_walk39, InsaneFramesWalkNormal, &CMonster::Walk);
CAnim InsaneMoveRunNormal (FRAME_walk27, FRAME_walk39, InsaneFramesWalkNormal, &CMonster::Run);

CFrame InsaneFramesWalkInsane [] =
{
	CFrame (&CMonster::AI_Walk,	0, ConvertDerivedFunction(&CInsane::Scream)),		// walk 1
	CFrame (&CMonster::AI_Walk,	3.4f),		// walk 2
	CFrame (&CMonster::AI_Walk,	3.6f),		// 3
	CFrame (&CMonster::AI_Walk,	2.9f),		// 4
	CFrame (&CMonster::AI_Walk,	2.2f),		// 5
	CFrame (&CMonster::AI_Walk,	2.6f),		// 6
	CFrame (&CMonster::AI_Walk,	0),		// 7
	CFrame (&CMonster::AI_Walk,	0.7f),		// 8
	CFrame (&CMonster::AI_Walk,	4.8f),		// 9
	CFrame (&CMonster::AI_Walk,	5.3f),		// 10
	CFrame (&CMonster::AI_Walk,	1.1f),		// 11
	CFrame (&CMonster::AI_Walk,	2),		// 12
	CFrame (&CMonster::AI_Walk,	0.5f),		// 13
	CFrame (&CMonster::AI_Walk,	0),		// 14
	CFrame (&CMonster::AI_Walk,	0),		// 15
	CFrame (&CMonster::AI_Walk,	4.9f),		// 16
	CFrame (&CMonster::AI_Walk,	6.7f),		// 17
	CFrame (&CMonster::AI_Walk,	3.8f),		// 18
	CFrame (&CMonster::AI_Walk,	2),		// 19
	CFrame (&CMonster::AI_Walk,	0.2f),		// 20
	CFrame (&CMonster::AI_Walk,	0),		// 21
	CFrame (&CMonster::AI_Walk,	3.4f),		// 22
	CFrame (&CMonster::AI_Walk,	6.4f),		// 23
	CFrame (&CMonster::AI_Walk,	5),		// 24
	CFrame (&CMonster::AI_Walk,	1.8f),		// 25
	CFrame (&CMonster::AI_Walk,	0)		// 26
};
CAnim InsaneMoveWalkInsane (FRAME_walk1, FRAME_walk26, InsaneFramesWalkInsane, &CMonster::Walk);
CAnim InsaneMoveRunInsane (FRAME_walk1, FRAME_walk26, InsaneFramesWalkInsane, &CMonster::Run);

CFrame InsaneFramesStandPain [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveStandPain (FRAME_st_pain2, FRAME_st_pain12, InsaneFramesStandPain, &CMonster::Run);

CFrame InsaneFramesStandDeath [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveStandDeath (FRAME_st_death2, FRAME_st_death18, InsaneFramesStandDeath, ConvertDerivedFunction(&CInsane::Dead));

CFrame InsaneFramesCrawl [] =
{
	CFrame (&CMonster::AI_Walk,	0,		ConvertDerivedFunction(&CInsane::Scream)),
	CFrame (&CMonster::AI_Walk,	1.5f),
	CFrame (&CMonster::AI_Walk,	2.1f),
	CFrame (&CMonster::AI_Walk,	3.6f),
	CFrame (&CMonster::AI_Walk,	2),
	CFrame (&CMonster::AI_Walk,	0.9f),
	CFrame (&CMonster::AI_Walk,	3),
	CFrame (&CMonster::AI_Walk,	3.4f),
	CFrame (&CMonster::AI_Walk,	2.4f)
};
CAnim InsaneMoveCrawl (FRAME_crawl1, FRAME_crawl9, InsaneFramesCrawl);
CAnim InsaneMoveRunCrawl (FRAME_crawl1, FRAME_crawl9, InsaneFramesCrawl);

CFrame InsaneFramesCrawlPain [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveCrawlPain (FRAME_cr_pain2, FRAME_cr_pain10, InsaneFramesCrawlPain, &CMonster::Run);

CFrame InsaneFramesCrawlDeath [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveCrawlDeath (FRAME_cr_death10, FRAME_cr_death16, InsaneFramesCrawlDeath, ConvertDerivedFunction(&CInsane::Dead));

CFrame InsaneFramesCross [] =
{
	CFrame (&CMonster::AI_Move,	0, ConvertDerivedFunction(&CInsane::Moan)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveCross (FRAME_cross1, FRAME_cross15, InsaneFramesCross, ConvertDerivedFunction(&CInsane::Cross));

CFrame InsaneFramesStruggleCross [] =
{
	CFrame (&CMonster::AI_Move,	0, ConvertDerivedFunction(&CInsane::Scream)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveStruggleCross (FRAME_cross16, FRAME_cross30, InsaneFramesStruggleCross, ConvertDerivedFunction(&CInsane::Cross));

void CInsane::Cross ()
{
	CurrentMove = (frand() < 0.8) ? &InsaneMoveCross : &InsaneMoveStruggleCross;
}

void CInsane::Walk ()
{
	if ( (Entity->SpawnFlags & INSANE_STAND_GROUND) && (Entity->State.GetFrame() == FRAME_cr_pain10) )			// Hold Ground?
	{
		CurrentMove = &InsaneMoveDown;
		return;
	}

	if (Entity->SpawnFlags & INSANE_CRAWL)
		CurrentMove = &InsaneMoveCrawl;
	else
		CurrentMove = (frand() <= 0.5) ? &InsaneMoveWalkNormal : &InsaneMoveWalkInsane;
}

void CInsane::Run ()
{
	if ( (Entity->SpawnFlags & INSANE_STAND_GROUND) && (Entity->State.GetFrame() == FRAME_cr_pain10))			// Hold Ground?
	{
		CurrentMove = &InsaneMoveDown;
		return;
	}
	
	if (Entity->SpawnFlags & INSANE_CRAWL)				// Crawling?
		CurrentMove = &InsaneMoveRunCrawl;
	else // Else, mix it up
		CurrentMove = (frand() <= 0.5) ? &InsaneMoveRunNormal : &InsaneMoveRunInsane;
}


void CInsane::Pain (IBaseEntity *Other, sint32 Damage)
{
//	if (self->health < (self->max_health / 2))
//		self->state.skinnum = 1;

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;

	sint32 l = Clamp<sint32>(((floorf((Max<>(0, Entity->Health-1)) / 25))), 0, 3);
	Entity->PlaySound (CHAN_VOICE, GameMedia.Player.Pain[l][(irandom(2))]);

	// Don't go into pain frames if crucified.
	if (Entity->SpawnFlags & INSANE_CRUCIFIED)
	{
		CurrentMove = &InsaneMoveStruggleCross;			
		return;
	}
	
	if  ( ((Entity->State.GetFrame() >= FRAME_crawl1) && (Entity->State.GetFrame() <= FRAME_crawl9)) || ((Entity->State.GetFrame() >= FRAME_stand99) && (Entity->State.GetFrame() <= FRAME_stand160)) )
		CurrentMove = &InsaneMoveCrawlPain;
	else
		CurrentMove = &InsaneMoveStandPain;
}

void CInsane::OnGround ()
{
	CurrentMove = &InsaneMoveDown;
}

void CInsane::CheckDown ()
{
	if (Entity->SpawnFlags & INSANE_ALWAYS_STAND)				// Always stand
		return;
	if (frand() < 0.3)
		CurrentMove = (frand() < 0.5) ? &InsaneMoveUpToDown : &InsaneMoveJumpDown;
}

void CInsane::CheckUp ()
{
	// If Hold_Ground and Crawl are set
	if ( (Entity->SpawnFlags & INSANE_CRAWL) && (Entity->SpawnFlags & INSANE_STAND_GROUND) )
		return;
	if (frand() < 0.5)
		CurrentMove = &InsaneMoveUpToDown;
}

void CInsane::Stand ()
{
	if (Entity->SpawnFlags & INSANE_CRUCIFIED)			// If crucified
	{
		CurrentMove = &InsaneMoveCross;
		AIFlags |= AI_STAND_GROUND;
	}
	// If Hold_Ground and Crawl are set
	else if ( (Entity->SpawnFlags & INSANE_CRAWL) && (Entity->SpawnFlags & 16) )
		CurrentMove = &InsaneMoveDown;
	else
		CurrentMove = (frand() < 0.5) ? &InsaneMoveStandNormal : &InsaneMoveStandInsane;
}

void CInsane::Dead ()
{
	if (Entity->SpawnFlags & INSANE_CRUCIFIED)
		AIFlags |= AI_FLY;
	else
	{
		Entity->GetMins().Set (-16, -16, -24);
		Entity->GetMaxs().Set (16, 16, -8);
		Entity->PhysicsType = PHYSICS_TOSS;
	}
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}


void CInsane::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"), 255, ATTN_IDLE);
		for (sint32 n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_Bone[0], Damage, GIB_ORGANIC);
		for (sint32 n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_Head[1], Damage, GIB_ORGANIC);
		Entity->DeadFlag = true;
		return;
	}

	if (Entity->DeadFlag == true)
		return;

	Entity->PlaySound (CHAN_VOICE, GameMedia.Player.Death[irandom(4)], 255, ATTN_IDLE);

	Entity->DeadFlag = true;
	Entity->CanTakeDamage = true;

	if (Entity->SpawnFlags & INSANE_CRUCIFIED)
		Dead ();
	else
	{
		if ( ((Entity->State.GetFrame() >= FRAME_crawl1) && (Entity->State.GetFrame() <= FRAME_crawl9)) || ((Entity->State.GetFrame() >= FRAME_stand99) && (Entity->State.GetFrame() <= FRAME_stand160)) )		
			CurrentMove = &InsaneMoveCrawlDeath;
		else
			CurrentMove = &InsaneMoveStandDeath;
	}
}


/*QUAKED misc_insane (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn CRAWL CRUCIFIED STAND_GROUND ALWAYS_STAND
*/

void CInsane::Spawn ()
{
	Sounds[SOUND_FIST] = SoundIndex ("insane/insane11.wav");
	Sounds[SOUND_SHAKE] = SoundIndex ("insane/insane5.wav");
	Sounds[SOUND_MOAN] = SoundIndex ("insane/insane7.wav");
	SoundScream[0] = SoundIndex ("insane/insane1.wav");
	SoundScream[1] = SoundIndex ("insane/insane2.wav");
	SoundScream[2] = SoundIndex ("insane/insane3.wav");
	SoundScream[3] = SoundIndex ("insane/insane4.wav");
	SoundScream[4] = SoundIndex ("insane/insane6.wav");
	SoundScream[5] = SoundIndex ("insane/insane8.wav");
	SoundScream[6] = SoundIndex ("insane/insane9.wav");
	SoundScream[7] = SoundIndex ("insane/insane10.wav");

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex("models/monsters/insane/tris.md2");

	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, 32);

	Entity->Health = 100;
	Entity->GibHealth = -50;
	Entity->Mass = 300;

	AIFlags |= AI_GOOD_GUY;

	Entity->Link ();

	if (Entity->SpawnFlags & INSANE_STAND_GROUND)				// Stand Ground
		AIFlags |= AI_STAND_GROUND;

	CurrentMove = &InsaneMoveStandNormal;

	if (Entity->SpawnFlags & INSANE_CRUCIFIED)					// Crucified ?
	{
		Entity->GetMins().Set (-16, 0, 0);
		Entity->GetMaxs().Set (16, 8, 32);
		Entity->AffectedByKnockback = false;
		FlyMonsterStart ();
	}
	else
	{
		WalkMonsterStart ();
	}
	Entity->State.GetSkinNum() = irandom(3);
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("misc_insane", CInsane);
