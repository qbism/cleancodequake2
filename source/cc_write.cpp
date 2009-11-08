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
// cc_write.cpp
// "Write" functions to write and check for malformed writes.
//

#include "cc_local.h"

#include <limits.h>
#include <float.h>

_CC_DISABLE_DEPRECATION
void _WriteChar (sint8 val)
{
	gi.WriteChar (val);
}

void _WriteByte (uint8 val)
{
	gi.WriteByte (val);
}

void _WriteShort (sint16 val)
{
	gi.WriteShort (val);
}

void _WriteLong (long val)
{
	gi.WriteLong (val);
}

void _WriteFloat (float val)
{
	gi.WriteFloat (val);
}

void _WriteString (char *val)
{
	gi.WriteString (val);
}

CC_ENUM (uint8, EWriteType)
{
	WT_CHAR,
	WT_BYTE,
	WT_SHORT,
	WT_LONG,
	WT_FLOAT,
	WT_STRING,
};

class CWriteIndex
{
public:
	uint8		*Ptr;
	EWriteType	Type;

	CWriteIndex (uint8 *Ptr, EWriteType Type) :
	Ptr(Ptr),
	Type(Type)
	{
	};

	~CWriteIndex ()
	{
		if (Type == WT_STRING)
			QDelete[] Ptr;
		else
			QDelete Ptr;
	};

	void Write ()
	{
		switch (Type)
		{
		case WT_CHAR:
			_WriteChar ((sint8)*Ptr);
			break;
		case WT_BYTE:
			_WriteByte ((uint8)*Ptr);
			break;
		case WT_SHORT:
			_WriteShort (*((sint16*)(Ptr)));
			break;
		case WT_LONG:
			_WriteLong (*((long*)(Ptr)));
			break;
		case WT_FLOAT:
			_WriteFloat (*((float*)(Ptr)));
			break;
		case WT_STRING:
			_WriteString ((char*)Ptr);
			break;
		default:
			_CC_ASSERT_EXPR (0, "WriteIndex has invalid write ID");
			break;
		};
	};
};

std::vector <CWriteIndex*, std::game_allocator<CWriteIndex*> > WriteQueue;

void SendQueue (edict_t *To, bool Reliable)
{
	for (size_t i = 0; i < WriteQueue.size(); i++)
		WriteQueue[i]->Write ();

	gi.unicast (To, (Reliable) ? 1 : 0);
}

void Clear ()
{
	WriteQueue.clear();
}

void PushUp (uint8 *Ptr, EWriteType Type)
{
	WriteQueue.push_back (QNew (com_levelPool, 0) CWriteIndex(Ptr, Type));
}

// vec3f overloads
void Cast (ECastType castType, ECastFlags castFlags, vec3f &Origin, CBaseEntity *Ent, bool SuppliedOrigin)
{
	// Sanity checks
	if (castType == CAST_MULTI && Ent)
		Com_Printf (0, "Multicast with an associated Ent\n");
	else if (castType == CAST_MULTI && !SuppliedOrigin)
	{
		Com_Printf (0, "Multicast with no associated Origin! Can't do!\n");
		Clear ();
		return;
	}
	else if (castType == CAST_UNI && !Ent)
	{
		Com_Printf (0, "Unicast with no associated Ent! Can't do!\n");
		Clear ();
		return;
	}
	else if (castType == CAST_UNI && SuppliedOrigin)
		Com_Printf (0, "Multicast with an associated Origin\n");

	CPlayerEntity *Entity = NULL;
	if (Ent)
		Entity = entity_cast<CPlayerEntity>(Ent);

	// Sends to all entities
	switch (castType)
	{
	case CAST_MULTI:
		for (sint32 i = 1; i <= game.maxclients; i++)
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(g_edicts[i].Entity);

			if (!Player || !Player->GetInUse() || (Player->Client.Persistent.state != SVCS_SPAWNED))
				continue;

			if ((castFlags & CASTFLAG_PVS) && !InVisibleArea(Origin, Player->State.GetOrigin()))
				continue;
			if ((castFlags & CASTFLAG_PHS) && !InHearableArea(Origin, Player->State.GetOrigin()))
				continue;

			//gi.unicast (e, (castFlags & CASTFLAG_RELIABLE) ? true : false);
			SendQueue (Player->gameEntity, (castFlags & CASTFLAG_RELIABLE) ? true : false);
		}
		break;
	// Send to one entity
	case CAST_UNI:
		if ((castFlags & CASTFLAG_PVS) && !InVisibleArea(Origin, Entity->State.GetOrigin()))
			break;
		if ((castFlags & CASTFLAG_PHS) && !InHearableArea(Origin, Entity->State.GetOrigin()))
			break;

		//gi.unicast (Ent, (castFlags & CASTFLAG_RELIABLE) ? true : false);
		SendQueue (Ent->gameEntity, (castFlags & CASTFLAG_RELIABLE) ? true : false);
		break;
	}

	//if (castType == CAST_MULTI)
	//	gi.multicast (Origin, (castFlags & CASTFLAG_PVS) ? MULTICAST_PVS : MULTICAST_PHS);
	//else if (castType == CAST_UNI)
	//	gi.unicast (Ent, (castFlags & CASTFLAG_RELIABLE) ? true : false);
	Clear ();
}
void Cast (ECastFlags castFlags, vec3f &Origin)
{
	Cast (CAST_MULTI, castFlags, Origin, NULL, true);
}
void Cast (ECastFlags castFlags, CBaseEntity *Ent)
{
	Cast (CAST_UNI, castFlags, vec3fOrigin, Ent, false);
}

void WriteChar (sint8 val)
{
	if (val < CHAR_MIN || val > CHAR_MAX)
	{
		Com_Printf (0, "Malformed char written!\n");
		val = Clamp<char> (val, CHAR_MIN, CHAR_MAX);
	}

	//gi.WriteChar (val);
	PushUp ((uint8*)QNew (com_levelPool, 0) sint8 (val), WT_CHAR);
}

void WriteByte (uint8 val)
{
	if (val < 0 || val > UCHAR_MAX)
	{
		Com_Printf (0, "Malformed uint8 written!\n");
		val = Clamp<uint8> (val, 0, UCHAR_MAX);
	}

	//gi.WriteByte (val);
	PushUp ((uint8*)QNew (com_levelPool, 0) uint8 (val), WT_BYTE);
}

void WriteShort (sint16 val)
{
	if (val < SHRT_MIN || val > SHRT_MAX)
	{
		Com_Printf (0, "Malformed sint16 written!\n");
		val = Clamp<sint16> (val, SHRT_MIN, SHRT_MAX);
	}

	//gi.WriteShort (val);
	PushUp ((uint8*)QNew (com_levelPool, 0) sint16 (val), WT_SHORT);
}

void WriteLong (long val)
{
	if (val < LONG_MIN || val > LONG_MAX)
	{
		Com_Printf (0, "Malformed long written!\n");
		val = Clamp<long> (val, LONG_MIN, LONG_MAX);
	}

	//gi.WriteLong (val);
	PushUp ((uint8*)QNew (com_levelPool, 0) long (val), WT_LONG);
}

void WriteFloat (float val)
{
	if (val < FLT_MIN || val > FLT_MAX)
	{
		Com_Printf (0, "Malformed float written!\n");
		val = Clamp<float> (val, FLT_MIN, FLT_MAX);
	}

	//gi.WriteFloat (val);
	PushUp ((uint8*)QNew (com_levelPool, 0) float (val), WT_FLOAT);
}

void WriteAngle (float val)
{
	if (val < 0 || val > 360)
		Com_Printf (0, "Malformed angle may have been written!\n");

	WriteByte (ANGLE2BYTE (val));
}

void WriteAngle16 (float val)
{
	if (val < 0 || val > 360)
		Com_Printf (0, "Malformed angle may have been written!\n");

	WriteShort (ANGLE2SHORT (val));
}

void WriteString (const char *val)
{
	if (!val || val == NULL || !val[0] || strlen(val) > 1400)
	{
		Com_Printf (0, "Malformed string written!\n");
		// FIXME: Clamp the string??
	}

	//gi.WriteString (val);
	PushUp ((uint8*)Mem_PoolStrDup (val, com_levelPool, 0), WT_STRING);
}

void WriteCoord (float f)
{
	//WriteShort ((sint32)(f * 8));
	PushUp ((uint8*)QNew (com_levelPool, 0) sint16 ((sint16)(f * 8)), WT_SHORT);
}

void WritePosition (vec3_t val)
{
	if (!val)
	{
		for (sint32 i = 0; i < 3; i++)
			WriteCoord(vec3fOrigin[i]);
	}
	else
	{
		bool Printed = false;
		for (sint32 i = 0; i < 3; i++)
		{
			if (!Printed && (val[i] > 4096 || val[i] < -4096))
			{			
				Com_Printf (0, "Malformed position may have been written!\n");
				Printed = true;
			}

			WriteCoord(val[i]);
		}
	}
}

void WritePosition (vec3f &val)
{
	if (!val)
	{
		for (sint32 i = 0; i < 3; i++)
			WriteCoord(vec3fOrigin[i]);
	}
	else
	{
		bool Printed = false;
		for (sint32 i = 0; i < 3; i++)
		{
			if (!Printed && (val[i] > 4096 || val[i] < -4096))
			{			
				Com_Printf (0, "Malformed position may have been written!\n");
				Printed = true;
			}

			WriteCoord(val[i]);
		}
	}
}

void WriteDirection (vec3_t val)
{
	WriteByte (DirToByte (val));
}

void WriteDirection (vec3f &val)
{
	WriteByte (DirToByte (val));
}

_CC_ENABLE_DEPRECATION