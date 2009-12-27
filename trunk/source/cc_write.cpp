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

void _WriteString (const char *val)
{
	gi.WriteString ((char*)val);
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
	EWriteType	Type;

	CWriteIndex (EWriteType Type) :
	Type(Type)
	{
	};

	virtual ~CWriteIndex ()
	{
	};

	virtual void Write ()
	{
		_CC_ASSERT_EXPR (0, "WriteIndex has invalid write ID");
	};
};

template <typename TType>
class CWritePrimIndex : public CWriteIndex
{
public:
	TType	Val;

	CWritePrimIndex (TType Val, EWriteType Type) :
	Val(Val),
	CWriteIndex(Type)
	{
	};

	void Write ()
	{
		switch (Type)
		{
		case WT_CHAR:
			_WriteChar (Val);
			break;
		case WT_BYTE:
			_WriteByte (Val);
			break;
		case WT_SHORT:
			_WriteShort (Val);
			break;
		case WT_LONG:
			_WriteByte (Val);
			break;
		case WT_FLOAT:
			_WriteFloat (Val);
			break;
		};
	};
};

class CWriteString : public CWriteIndex
{
public:
	std::cc_string	Val;

	CWriteString (std::cc_string Val) :
	Val(Val),
	CWriteIndex(WT_STRING)
	{
	};

	void Write ()
	{
		_WriteString (Val.c_str());
	};
};

std::vector <CWriteIndex*, std::write_allocator<CWriteIndex*> > WriteQueue;

void SendQueue (edict_t *To, bool Reliable)
{
	for (size_t i = 0; i < WriteQueue.size(); i++)
		WriteQueue[i]->Write ();

	gi.unicast (To, (Reliable) ? 1 : 0);
}

void Clear ()
{
	for (size_t i = 0; i < WriteQueue.size(); i++)
		QDelete WriteQueue[i];
	WriteQueue.clear();
}

// vec3f overloads
void Cast (ECastType castType, ECastFlags castFlags, vec3f &Origin, CBaseEntity *Ent, bool SuppliedOrigin)
{
	// Sanity checks
	if (castType == CAST_MULTI && Ent)
		DebugPrintf ( "Multicast with an associated Ent\n");
	else if (castType == CAST_MULTI && !SuppliedOrigin)
	{
		DebugPrintf ( "Multicast with no associated Origin! Can't do!\n");
		Clear ();
		return;
	}
	else if (castType == CAST_UNI && !Ent)
	{
		DebugPrintf ( "Unicast with no associated Ent! Can't do!\n");
		Clear ();
		return;
	}
	else if (castType == CAST_UNI && SuppliedOrigin)
		DebugPrintf ( "Multicast with an associated Origin\n");

	CPlayerEntity *Entity = NULL;
	if (Ent)
		Entity = entity_cast<CPlayerEntity>(Ent);

	// Sends to all entities
	switch (castType)
	{
	case CAST_MULTI:
		for (sint32 i = 1; i <= game.MaxClients; i++)
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(g_edicts[i].Entity);

			if (!Player || !Player->GetInUse() || (Player->Client.Persistent.State != SVCS_SPAWNED))
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
		DebugPrintf ( "Malformed char written!\n");
		val = Clamp<char> (val, CHAR_MIN, CHAR_MAX);
	}

	//gi.WriteChar (val);
	WriteQueue.push_back (QNew (com_writePool, 0) CWritePrimIndex<sint8> (val, WT_CHAR));
}

void WriteByte (uint8 val)
{
	if (val < 0 || val > UCHAR_MAX)
	{
		DebugPrintf ( "Malformed uint8 written!\n");
		val = Clamp<uint8> (val, 0, UCHAR_MAX);
	}

	//gi.WriteByte (val);
	WriteQueue.push_back (QNew (com_writePool, 0) CWritePrimIndex<uint8> (val, WT_BYTE));
}

void WriteShort (sint16 val)
{
	if (val < SHRT_MIN || val > SHRT_MAX)
	{
		DebugPrintf ( "Malformed sint16 written!\n");
		val = Clamp<sint16> (val, SHRT_MIN, SHRT_MAX);
	}

	//gi.WriteShort (val);
	WriteQueue.push_back (QNew (com_writePool, 0) CWritePrimIndex<sint16> (val, WT_SHORT));
}

void WriteLong (long val)
{
	if (val < LONG_MIN || val > LONG_MAX)
	{
		DebugPrintf ( "Malformed long written!\n");
		val = Clamp<long> (val, LONG_MIN, LONG_MAX);
	}

	//gi.WriteLong (val);
	WriteQueue.push_back (QNew (com_writePool, 0) CWritePrimIndex<long> (val, WT_LONG));
}

void WriteFloat (float val)
{
	if (val < FLT_MIN || val > FLT_MAX)
	{
		DebugPrintf ( "Malformed float written!\n");
		val = Clamp<float> (val, FLT_MIN, FLT_MAX);
	}

	//gi.WriteFloat (val);
	WriteQueue.push_back (QNew (com_writePool, 0) CWritePrimIndex<float> (val, WT_FLOAT));
}

void WriteAngle (float val)
{
	if (val < 0 || val > 360)
		DebugPrintf ( "Malformed angle may have been written!\n");

	WriteByte (ANGLE2BYTE (val));
}

void WriteAngle16 (float val)
{
	if (val < 0 || val > 360)
		DebugPrintf ( "Malformed angle may have been written!\n");

	WriteShort (ANGLE2SHORT (val));
}

void WriteString (const char *val)
{
	if (!val || val == NULL || !val[0] || strlen(val) > 1400)
	{
		DebugPrintf ( "Malformed string written!\n");
		// FIXME: Clamp the string??
	}

	//gi.WriteString (val);
	WriteQueue.push_back (QNew (com_writePool, 0) CWriteString (val));
}

void WriteCoord (float f)
{
	//WriteShort ((sint32)(f * 8));
	WriteQueue.push_back (QNew (com_writePool, 0) CWritePrimIndex<sint16> ((f * 8), WT_SHORT));
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
				DebugPrintf ( "Malformed position may have been written!\n");
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
				DebugPrintf ( "Malformed position may have been written!\n");
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