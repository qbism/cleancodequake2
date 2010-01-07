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
// cc_func_entities.cpp
// Func_ entities that aren't brush models
//

#include "cc_local.h"
#include "cc_brushmodels.h"
#include "cc_func_entities.h"

/*QUAKED func_timer (0.3 0.1 0.6) (-8 -8 -8) (8 8 8) START_ON
"wait"			base time between triggering all targets, default is 1
"random"		wait variance, default is 0

so, the basic time between firing is a random time between
(wait - random) and (wait + random)

"delay"			delay before first firing when turned on, default is 0

"pausetime"		additional delay used only the very first time
				and only if spawned with START_ON

These can used but not touched.
*/

#define TIMER_START_ON		1

CFuncTimer::CFuncTimer () :
	CBaseEntity (),
	CMapEntity (),
	CThinkableEntity (),
	CUsableEntity (),
	Wait (0),
	Random (0),
	PauseTime (0)
	{
	};

CFuncTimer::CFuncTimer (sint32 Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CThinkableEntity (Index),
	CUsableEntity (Index),
	Wait (0),
	Random (0),
	PauseTime (0)
	{
	};

ENTITYFIELDS_BEGIN(CFuncTimer)
{
	CEntityField ("random", EntityMemberOffset(CFuncTimer,Random), FT_FRAMENUMBER | FT_SAVABLE),
	CEntityField ("pausetime", EntityMemberOffset(CFuncTimer,PauseTime), FT_FRAMENUMBER | FT_SAVABLE),
	CEntityField ("wait", EntityMemberOffset(CFuncTimer,Wait), FT_FRAMENUMBER | FT_SAVABLE),
};
ENTITYFIELDS_END(CFuncTimer)

bool			CFuncTimer::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CFuncTimer> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void			CFuncTimer::SaveFields (CFile &File)
{
	SaveEntityFields <CFuncTimer> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
	CThinkableEntity::SaveFields (File);
}

void			CFuncTimer::LoadFields (CFile &File)
{
	LoadEntityFields <CFuncTimer> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
	CThinkableEntity::LoadFields (File);
}

void CFuncTimer::Think ()
{
	UseTargets (Activator, Message);
	NextThink = level.Frame + (Wait + (irandom(Random)));
}

bool CFuncTimer::Run ()
{
	return CBaseEntity::Run ();
};

void CFuncTimer::Use (CBaseEntity *other, CBaseEntity *activator)
{
	Activator = activator;

	// if on, turn it off
	if (NextThink)
	{
		NextThink = 0;
		return;
	}

	// turn it on
	if (Delay)
		NextThink = level.Frame + Delay;
	else
		Think ();
}

void CFuncTimer::Spawn ()
{
	if (!Wait)
		Wait = 10;

	if (Random >= Wait)
	{
		Random = Wait - FRAMETIME;
		// Paril FIXME
		// This to me seems like a very silly warning.
		MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Random is greater than or equal to wait\n");
		//gi.dprintf("func_timer at (%f %f %f) has random >= wait\n", self->state.origin[0], self->state.origin[1], self->state.origin[2]);
	}

	if (SpawnFlags & TIMER_START_ON)
	{
		// lots of backwards compatibility
		NextThink = level.Frame + 10 + (PauseTime + Delay + Wait + irandom(Random));
		Activator = this;
	}

	GetSvFlags() = SVF_NOCLIENT;
}

LINK_CLASSNAME_TO_CLASS ("func_timer", CFuncTimer);

// These two are required for func_clock, so they're here.

/*QUAKED target_character (0 0 1) ?
used with target_string (must be on same "team")
"count" is position in the string (starts at 1)
*/

CTargetCharacter::CTargetCharacter () :
	CBaseEntity (),
	CMapEntity (),
	CBrushModel ()
	{
	};

CTargetCharacter::CTargetCharacter (sint32 Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CBrushModel (Index)
	{
	};

bool CTargetCharacter::Run ()
{
	return CBrushModel::Run ();
};

void CTargetCharacter::Spawn ()
{
	PhysicsType = PHYSICS_PUSH;
	SetBrushModel ();
	GetSolid() = SOLID_BSP;
	State.GetFrame() = 12;
	Link ();
};

ENTITYFIELDS_BEGIN(CTargetCharacter)
{
	CEntityField ("count", EntityMemberOffset(CTargetCharacter,Character), FT_BYTE | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetCharacter)

bool CTargetCharacter::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetCharacter> (this, Key, Value))
		return true;

	return (CBrushModel::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
}

void			CTargetCharacter::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetCharacter> (this, File);
	CMapEntity::SaveFields (File);
	CBrushModel::SaveFields (File);
}

void			CTargetCharacter::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetCharacter> (this, File);
	CMapEntity::LoadFields (File);
	CBrushModel::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_character", CTargetCharacter);

/*QUAKED target_string (0 0 1) (-8 -8 -8) (8 8 8)
*/

CTargetString::CTargetString () :
	CBaseEntity (),
	CMapEntity ()
	{
	};

CTargetString::CTargetString (sint32 Index) :
	CBaseEntity (Index),
	CMapEntity (Index)
	{
	};

class CTargetStringForEachCallback : public CForEachTeamChainCallback
{
public:
	std::cc_string Message;
	CBaseEntity *Me;

	CTargetStringForEachCallback (CBaseEntity *Me, std::cc_string &Message) :
	Me(Me),
	Message(Message)
	{
	};

	void Callback (CBaseEntity *e)
	{
		if (e == Me)
			return;

		CTargetCharacter *Entity = entity_cast<CTargetCharacter>(e);

		if (!Entity->Character)
			return;
		size_t n = Entity->Character - 1;
		if (n > Message.length())
		{
			e->State.GetFrame() = 12;
			return;
		}

		switch (Message[n])
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			e->State.GetFrame() = (Message[n] - '0');
			break;
		case '-':
			e->State.GetFrame() = 10;
			break;
		case ':':
			e->State.GetFrame() = 11;
			break;
		default:
			e->State.GetFrame() = 12;
			break;
		}
	};
};

void CTargetString::Use (CBaseEntity *other, CBaseEntity *activator)
{
	CTargetStringForEachCallback (this, Message).Query (this);
}

void CTargetString::Spawn ()
{
}

LINK_CLASSNAME_TO_CLASS ("target_string", CTargetString);

/*QUAKED func_clock (0 0 1) (-8 -8 -8) (8 8 8) TIMER_UP TIMER_DOWN START_OFF MULTI_USE
target a target_string with this

The default is to be a time of day clock

TIMER_UP and TIMER_DOWN run for "count" seconds and the fire "pathtarget"
If START_OFF, this entity must be used before it starts

"style"		0 "xx"
			1 "xx:xx"
			2 "xx:xx:xx"
*/

#define CLOCK_TIMER_UP		1
#define CLOCK_TIMER_DOWN	2
#define CLOCK_START_OFF		4
#define CLOCK_MULTI_USE		8

#define CLOCK_MESSAGE_SIZE	16

CFuncClock::CFuncClock () :
	CBaseEntity (),
	CMapEntity (),
	CThinkableEntity (),
	CUsableEntity (),
	Seconds(0),
	String(NULL)
	{
	};

CFuncClock::CFuncClock (sint32 Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CThinkableEntity (Index),
	CUsableEntity (Index),
	Seconds(0),
	String(NULL)
	{
	};

ENTITYFIELDS_BEGIN(CFuncClock)
{
	CEntityField ("style", EntityMemberOffset(CFuncClock,Style), FT_BYTE | FT_SAVABLE),
	CEntityField ("count", EntityMemberOffset(CFuncClock,Count), FT_INT | FT_SAVABLE),
	CEntityField ("pathtarget", EntityMemberOffset(CFuncClock,CountTarget), FT_LEVEL_STRING | FT_SAVABLE),

	CEntityField ("Wait", EntityMemberOffset(CFuncClock,Wait), FT_FRAMENUMBER | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("Seconds", EntityMemberOffset(CFuncClock,Seconds), FT_INT | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END (CFuncClock);

bool CFuncClock::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CFuncClock> (this, Key, Value))
		return true;

	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
}

bool CFuncClock::Run ()
{
	return CBaseEntity::Run ();
};

void		CFuncClock::SaveFields (CFile &File)
{
	File.Write<sint32> ((String) ? String->State.GetNumber() : -1);

	SaveEntityFields <CFuncClock> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
	CThinkableEntity::SaveFields (File);
}

void		CFuncClock::LoadFields (CFile &File)
{
	sint32 Index = File.Read<sint32> ();

	if (Index != -1)
		String = entity_cast<CTargetString>(g_edicts[Index].Entity);

	LoadEntityFields <CFuncClock> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
	CThinkableEntity::LoadFields (File);
}

// don't let field width of any clock messages change, or it
// could cause an overwrite after a game load

void CFuncClock::Reset ()
{
	Activator = NULL;
	if (SpawnFlags & CLOCK_TIMER_UP)
	{
		Seconds = 0;
		Wait = Count;
	}
	else if (SpawnFlags & CLOCK_TIMER_DOWN)
	{
		Seconds = Count;
		Wait = 0;
	}
}

void CFuncClock::FormatCountdown ()
{
	char tempBuffer[CLOCK_MESSAGE_SIZE];
	switch (Style)
	{
	case 0:
	default:
		Q_snprintfz (tempBuffer, CLOCK_MESSAGE_SIZE, "%2i", Seconds);
		Message = tempBuffer;
		break;
	case 1:
		Q_snprintfz(tempBuffer, CLOCK_MESSAGE_SIZE, "%2i:%2i", Seconds / 60, Seconds % 60);
		if (tempBuffer[3] == ' ')
			tempBuffer[3] = '0';
		Message = tempBuffer;
		break;
	case 2:
		Q_snprintfz(tempBuffer, CLOCK_MESSAGE_SIZE, "%2i:%2i:%2i", Seconds / 3600, (Seconds - (Seconds / 3600) * 3600) / 60, Seconds % 60);
		if (tempBuffer[3] == ' ')
			tempBuffer[3] = '0';
		if (tempBuffer[6] == ' ')
			tempBuffer[6] = '0';
		Message = tempBuffer;
		break;
	};
}

void CFuncClock::Think ()
{
	if (!String)
	{
		String = entity_cast<CTargetString>(CC_Find<CMapEntity, ENT_MAP, EntityMemberOffset(CMapEntity,TargetName)> (NULL, Target));
		if (!String)
			return;
	}

	if (SpawnFlags & CLOCK_TIMER_UP)
	{
		FormatCountdown ();
		Seconds++;
	}
	else if (SpawnFlags & CLOCK_TIMER_DOWN)
	{
		FormatCountdown ();
		Seconds--;
	}
	else
	{
		struct tm	ltime;
		time_t		gmtime;
		char tempBuffer[CLOCK_MESSAGE_SIZE];

		time(&gmtime);
		localtime_s (&ltime, &gmtime);
		Q_snprintfz (tempBuffer, CLOCK_MESSAGE_SIZE, "%2i:%2i:%2i", ltime.tm_hour, ltime.tm_min, ltime.tm_sec);
		if (tempBuffer[3] == ' ')
			tempBuffer[3] = '0';
		if (tempBuffer[6] == ' ')
			tempBuffer[6] = '0';
		Message = tempBuffer;
	}

	String->Message = Message;
	String->Use (this, this);

	if (((SpawnFlags & CLOCK_TIMER_UP) && (Seconds > Wait)) ||
		((SpawnFlags & CLOCK_TIMER_DOWN) && (Seconds < Wait)))
	{
		if (CountTarget)
		{
			char *savetarget = Target;
			std::cc_string savemessage = Message;
			Target = CountTarget;
			Message.clear();
			UseTargets (Activator, Message);
			Target = savetarget;
			Message = savemessage;
		}

		if (!(SpawnFlags & CLOCK_MULTI_USE))
			return;

		Reset ();

		if (SpawnFlags & CLOCK_START_OFF)
			return;
	}

	NextThink = level.Frame + 10;
}

void CFuncClock::Use (CBaseEntity *other, CBaseEntity *activator)
{
	if (!Usable)
		return;

	if (!(SpawnFlags & CLOCK_MULTI_USE))
		Usable = false;
	
	if (Activator)
		return;

	Activator = activator;
	Think ();
}

void CFuncClock::Spawn ()
{
	if (!Target)
	{
		MapPrint (MAPPRINT_ERROR, this, GetAbsMin(), "No target\n");
		Free ();
		return;
	}

	if ((SpawnFlags & CLOCK_TIMER_DOWN) && (!Count))
	{
		MapPrint (MAPPRINT_ERROR, this, GetAbsMin(), "No count\n");
		Free ();
		return;
	}

	if ((SpawnFlags & CLOCK_TIMER_UP) && (!Count))
		Count = 3600;

	Reset ();

	if (SpawnFlags & CLOCK_START_OFF)
		Usable = true;
	else
		NextThink = level.Frame + 10;
}

LINK_CLASSNAME_TO_CLASS ("func_clock", CFuncClock);

