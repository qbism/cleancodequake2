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
// cc_tank.h
// Tank
//

#if !defined(CC_GUARD_TANK_H) || !INCLUDE_GUARDS
#define CC_GUARD_TANK_H

class CTank : public CMonster
{
public:
	enum
	{
		FRAME_stand01,
		FRAME_stand02,
		FRAME_stand03,
		FRAME_stand04,
		FRAME_stand05,
		FRAME_stand06,
		FRAME_stand07,
		FRAME_stand08,
		FRAME_stand09,
		FRAME_stand10,
		FRAME_stand11,
		FRAME_stand12,
		FRAME_stand13,
		FRAME_stand14,
		FRAME_stand15,
		FRAME_stand16,
		FRAME_stand17,
		FRAME_stand18,
		FRAME_stand19,
		FRAME_stand20,
		FRAME_stand21,
		FRAME_stand22,
		FRAME_stand23,
		FRAME_stand24,
		FRAME_stand25,
		FRAME_stand26,
		FRAME_stand27,
		FRAME_stand28,
		FRAME_stand29,
		FRAME_stand30,
		FRAME_walk01,
		FRAME_walk02,
		FRAME_walk03,
		FRAME_walk04,
		FRAME_walk05,
		FRAME_walk06,
		FRAME_walk07,
		FRAME_walk08,
		FRAME_walk09,
		FRAME_walk10,
		FRAME_walk11,
		FRAME_walk12,
		FRAME_walk13,
		FRAME_walk14,
		FRAME_walk15,
		FRAME_walk16,
		FRAME_walk17,
		FRAME_walk18,
		FRAME_walk19,
		FRAME_walk20,
		FRAME_walk21,
		FRAME_walk22,
		FRAME_walk23,
		FRAME_walk24,
		FRAME_walk25,
		FRAME_attak101,
		FRAME_attak102,
		FRAME_attak103,
		FRAME_attak104,
		FRAME_attak105,
		FRAME_attak106,
		FRAME_attak107,
		FRAME_attak108,
		FRAME_attak109,
		FRAME_attak110,
		FRAME_attak111,
		FRAME_attak112,
		FRAME_attak113,
		FRAME_attak114,
		FRAME_attak115,
		FRAME_attak116,
		FRAME_attak117,
		FRAME_attak118,
		FRAME_attak119,
		FRAME_attak120,
		FRAME_attak121,
		FRAME_attak122,
		FRAME_attak201,
		FRAME_attak202,
		FRAME_attak203,
		FRAME_attak204,
		FRAME_attak205,
		FRAME_attak206,
		FRAME_attak207,
		FRAME_attak208,
		FRAME_attak209,
		FRAME_attak210,
		FRAME_attak211,
		FRAME_attak212,
		FRAME_attak213,
		FRAME_attak214,
		FRAME_attak215,
		FRAME_attak216,
		FRAME_attak217,
		FRAME_attak218,
		FRAME_attak219,
		FRAME_attak220,
		FRAME_attak221,
		FRAME_attak222,
		FRAME_attak223,
		FRAME_attak224,
		FRAME_attak225,
		FRAME_attak226,
		FRAME_attak227,
		FRAME_attak228,
		FRAME_attak229,
		FRAME_attak230,
		FRAME_attak231,
		FRAME_attak232,
		FRAME_attak233,
		FRAME_attak234,
		FRAME_attak235,
		FRAME_attak236,
		FRAME_attak237,
		FRAME_attak238,
		FRAME_attak301,
		FRAME_attak302,
		FRAME_attak303,
		FRAME_attak304,
		FRAME_attak305,
		FRAME_attak306,
		FRAME_attak307,
		FRAME_attak308,
		FRAME_attak309,
		FRAME_attak310,
		FRAME_attak311,
		FRAME_attak312,
		FRAME_attak313,
		FRAME_attak314,
		FRAME_attak315,
		FRAME_attak316,
		FRAME_attak317,
		FRAME_attak318,
		FRAME_attak319,
		FRAME_attak320,
		FRAME_attak321,
		FRAME_attak322,
		FRAME_attak323,
		FRAME_attak324,
		FRAME_attak325,
		FRAME_attak326,
		FRAME_attak327,
		FRAME_attak328,
		FRAME_attak329,
		FRAME_attak330,
		FRAME_attak331,
		FRAME_attak332,
		FRAME_attak333,
		FRAME_attak334,
		FRAME_attak335,
		FRAME_attak336,
		FRAME_attak337,
		FRAME_attak338,
		FRAME_attak339,
		FRAME_attak340,
		FRAME_attak341,
		FRAME_attak342,
		FRAME_attak343,
		FRAME_attak344,
		FRAME_attak345,
		FRAME_attak346,
		FRAME_attak347,
		FRAME_attak348,
		FRAME_attak349,
		FRAME_attak350,
		FRAME_attak351,
		FRAME_attak352,
		FRAME_attak353,
		FRAME_attak401,
		FRAME_attak402,
		FRAME_attak403,
		FRAME_attak404,
		FRAME_attak405,
		FRAME_attak406,
		FRAME_attak407,
		FRAME_attak408,
		FRAME_attak409,
		FRAME_attak410,
		FRAME_attak411,
		FRAME_attak412,
		FRAME_attak413,
		FRAME_attak414,
		FRAME_attak415,
		FRAME_attak416,
		FRAME_attak417,
		FRAME_attak418,
		FRAME_attak419,
		FRAME_attak420,
		FRAME_attak421,
		FRAME_attak422,
		FRAME_attak423,
		FRAME_attak424,
		FRAME_attak425,
		FRAME_attak426,
		FRAME_attak427,
		FRAME_attak428,
		FRAME_attak429,
		FRAME_pain101,
		FRAME_pain102,
		FRAME_pain103,
		FRAME_pain104,
		FRAME_pain201,
		FRAME_pain202,
		FRAME_pain203,
		FRAME_pain204,
		FRAME_pain205,
		FRAME_pain301,
		FRAME_pain302,
		FRAME_pain303,
		FRAME_pain304,
		FRAME_pain305,
		FRAME_pain306,
		FRAME_pain307,
		FRAME_pain308,
		FRAME_pain309,
		FRAME_pain310,
		FRAME_pain311,
		FRAME_pain312,
		FRAME_pain313,
		FRAME_pain314,
		FRAME_pain315,
		FRAME_pain316,
		FRAME_death101,
		FRAME_death102,
		FRAME_death103,
		FRAME_death104,
		FRAME_death105,
		FRAME_death106,
		FRAME_death107,
		FRAME_death108,
		FRAME_death109,
		FRAME_death110,
		FRAME_death111,
		FRAME_death112,
		FRAME_death113,
		FRAME_death114,
		FRAME_death115,
		FRAME_death116,
		FRAME_death117,
		FRAME_death118,
		FRAME_death119,
		FRAME_death120,
		FRAME_death121,
		FRAME_death122,
		FRAME_death123,
		FRAME_death124,
		FRAME_death125,
		FRAME_death126,
		FRAME_death127,
		FRAME_death128,
		FRAME_death129,
		FRAME_death130,
		FRAME_death131,
		FRAME_death132,
		FRAME_recln101,
		FRAME_recln102,
		FRAME_recln103,
		FRAME_recln104,
		FRAME_recln105,
		FRAME_recln106,
		FRAME_recln107,
		FRAME_recln108,
		FRAME_recln109,
		FRAME_recln110,
		FRAME_recln111,
		FRAME_recln112,
		FRAME_recln113,
		FRAME_recln114,
		FRAME_recln115,
		FRAME_recln116,
		FRAME_recln117,
		FRAME_recln118,
		FRAME_recln119,
		FRAME_recln120,
		FRAME_recln121,
		FRAME_recln122,
		FRAME_recln123,
		FRAME_recln124,
		FRAME_recln125,
		FRAME_recln126,
		FRAME_recln127,
		FRAME_recln128,
		FRAME_recln129,
		FRAME_recln130,
		FRAME_recln131,
		FRAME_recln132,
		FRAME_recln133,
		FRAME_recln134,
		FRAME_recln135,
		FRAME_recln136,
		FRAME_recln137,
		FRAME_recln138,
		FRAME_recln139,
		FRAME_recln140,
	};

	virtual inline const char	*GetMonsterName() { return "Tank"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_THUD,
		SOUND_PAIN,
		SOUND_IDLE,
		SOUND_DIE,
		SOUND_STEP,
		SOUND_SIGHT,
		SOUND_WINDUP,
		SOUND_STRIKE,

		SOUND_MAX
	);

	CTank (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	void Attack ();
	void Idle ();
	void Run ();
	void Sight ();
	void Stand ();
	void Walk ();

	void Footstep ();
	void Thud ();
	void Windup ();
	void Blaster();
	void Strike ();
	void Rocket ();
	void MachineGun();
	void ReFireBlaster ();
	void ReAttackBlaster ();
	void PostStrike ();
	void DoAttackRocket ();
	void ReFireRocket ();

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	virtual void Spawn ();
	
	MONSTER_ID_HEADER
};

class CTankCommander : public CTank
{
public:
	CTankCommander (uint32 ID);

	inline const char	*GetMonsterName() { return "Tank Commander"; }

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
