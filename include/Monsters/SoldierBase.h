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
// cc_soldier_base.h
// The base for soldier-like monsters.
//

#if !defined(CC_GUARD_SOLDIER_BASE_H) || !INCLUDE_GUARDS
#define CC_GUARD_SOLDIER_BASE_H

class CSoldierBase : public IMonster
{
public:
	enum
	{
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
		FRAME_attak301,
		FRAME_attak302,
		FRAME_attak303,
		FRAME_attak304,
		FRAME_attak305,
		FRAME_attak306,
		FRAME_attak307,
		FRAME_attak308,
		FRAME_attak309,
		FRAME_attak401,
		FRAME_attak402,
		FRAME_attak403,
		FRAME_attak404,
		FRAME_attak405,
		FRAME_attak406,
		FRAME_duck01,
		FRAME_duck02,
		FRAME_duck03,
		FRAME_duck04,
		FRAME_duck05,
		FRAME_pain101,
		FRAME_pain102,
		FRAME_pain103,
		FRAME_pain104,
		FRAME_pain105,
		FRAME_pain201,
		FRAME_pain202,
		FRAME_pain203,
		FRAME_pain204,
		FRAME_pain205,
		FRAME_pain206,
		FRAME_pain207,
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
		FRAME_pain317,
		FRAME_pain318,
		FRAME_pain401,
		FRAME_pain402,
		FRAME_pain403,
		FRAME_pain404,
		FRAME_pain405,
		FRAME_pain406,
		FRAME_pain407,
		FRAME_pain408,
		FRAME_pain409,
		FRAME_pain410,
		FRAME_pain411,
		FRAME_pain412,
		FRAME_pain413,
		FRAME_pain414,
		FRAME_pain415,
		FRAME_pain416,
		FRAME_pain417,
		FRAME_run01,
		FRAME_run02,
		FRAME_run03,
		FRAME_run04,
		FRAME_run05,
		FRAME_run06,
		FRAME_run07,
		FRAME_run08,
		FRAME_run09,
		FRAME_run10,
		FRAME_run11,
		FRAME_run12,
		FRAME_runs01,
		FRAME_runs02,
		FRAME_runs03,
		FRAME_runs04,
		FRAME_runs05,
		FRAME_runs06,
		FRAME_runs07,
		FRAME_runs08,
		FRAME_runs09,
		FRAME_runs10,
		FRAME_runs11,
		FRAME_runs12,
		FRAME_runs13,
		FRAME_runs14,
		FRAME_runs15,
		FRAME_runs16,
		FRAME_runs17,
		FRAME_runs18,
		FRAME_runt01,
		FRAME_runt02,
		FRAME_runt03,
		FRAME_runt04,
		FRAME_runt05,
		FRAME_runt06,
		FRAME_runt07,
		FRAME_runt08,
		FRAME_runt09,
		FRAME_runt10,
		FRAME_runt11,
		FRAME_runt12,
		FRAME_runt13,
		FRAME_runt14,
		FRAME_runt15,
		FRAME_runt16,
		FRAME_runt17,
		FRAME_runt18,
		FRAME_runt19,
		FRAME_stand101,
		FRAME_stand102,
		FRAME_stand103,
		FRAME_stand104,
		FRAME_stand105,
		FRAME_stand106,
		FRAME_stand107,
		FRAME_stand108,
		FRAME_stand109,
		FRAME_stand110,
		FRAME_stand111,
		FRAME_stand112,
		FRAME_stand113,
		FRAME_stand114,
		FRAME_stand115,
		FRAME_stand116,
		FRAME_stand117,
		FRAME_stand118,
		FRAME_stand119,
		FRAME_stand120,
		FRAME_stand121,
		FRAME_stand122,
		FRAME_stand123,
		FRAME_stand124,
		FRAME_stand125,
		FRAME_stand126,
		FRAME_stand127,
		FRAME_stand128,
		FRAME_stand129,
		FRAME_stand130,
		FRAME_stand301,
		FRAME_stand302,
		FRAME_stand303,
		FRAME_stand304,
		FRAME_stand305,
		FRAME_stand306,
		FRAME_stand307,
		FRAME_stand308,
		FRAME_stand309,
		FRAME_stand310,
		FRAME_stand311,
		FRAME_stand312,
		FRAME_stand313,
		FRAME_stand314,
		FRAME_stand315,
		FRAME_stand316,
		FRAME_stand317,
		FRAME_stand318,
		FRAME_stand319,
		FRAME_stand320,
		FRAME_stand321,
		FRAME_stand322,
		FRAME_stand323,
		FRAME_stand324,
		FRAME_stand325,
		FRAME_stand326,
		FRAME_stand327,
		FRAME_stand328,
		FRAME_stand329,
		FRAME_stand330,
		FRAME_stand331,
		FRAME_stand332,
		FRAME_stand333,
		FRAME_stand334,
		FRAME_stand335,
		FRAME_stand336,
		FRAME_stand337,
		FRAME_stand338,
		FRAME_stand339,
		FRAME_walk101,
		FRAME_walk102,
		FRAME_walk103,
		FRAME_walk104,
		FRAME_walk105,
		FRAME_walk106,
		FRAME_walk107,
		FRAME_walk108,
		FRAME_walk109,
		FRAME_walk110,
		FRAME_walk111,
		FRAME_walk112,
		FRAME_walk113,
		FRAME_walk114,
		FRAME_walk115,
		FRAME_walk116,
		FRAME_walk117,
		FRAME_walk118,
		FRAME_walk119,
		FRAME_walk120,
		FRAME_walk121,
		FRAME_walk122,
		FRAME_walk123,
		FRAME_walk124,
		FRAME_walk125,
		FRAME_walk126,
		FRAME_walk127,
		FRAME_walk128,
		FRAME_walk129,
		FRAME_walk130,
		FRAME_walk131,
		FRAME_walk132,
		FRAME_walk133,
		FRAME_walk201,
		FRAME_walk202,
		FRAME_walk203,
		FRAME_walk204,
		FRAME_walk205,
		FRAME_walk206,
		FRAME_walk207,
		FRAME_walk208,
		FRAME_walk209,
		FRAME_walk210,
		FRAME_walk211,
		FRAME_walk212,
		FRAME_walk213,
		FRAME_walk214,
		FRAME_walk215,
		FRAME_walk216,
		FRAME_walk217,
		FRAME_walk218,
		FRAME_walk219,
		FRAME_walk220,
		FRAME_walk221,
		FRAME_walk222,
		FRAME_walk223,
		FRAME_walk224,
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
		FRAME_death133,
		FRAME_death134,
		FRAME_death135,
		FRAME_death136,
		FRAME_death201,
		FRAME_death202,
		FRAME_death203,
		FRAME_death204,
		FRAME_death205,
		FRAME_death206,
		FRAME_death207,
		FRAME_death208,
		FRAME_death209,
		FRAME_death210,
		FRAME_death211,
		FRAME_death212,
		FRAME_death213,
		FRAME_death214,
		FRAME_death215,
		FRAME_death216,
		FRAME_death217,
		FRAME_death218,
		FRAME_death219,
		FRAME_death220,
		FRAME_death221,
		FRAME_death222,
		FRAME_death223,
		FRAME_death224,
		FRAME_death225,
		FRAME_death226,
		FRAME_death227,
		FRAME_death228,
		FRAME_death229,
		FRAME_death230,
		FRAME_death231,
		FRAME_death232,
		FRAME_death233,
		FRAME_death234,
		FRAME_death235,
		FRAME_death301,
		FRAME_death302,
		FRAME_death303,
		FRAME_death304,
		FRAME_death305,
		FRAME_death306,
		FRAME_death307,
		FRAME_death308,
		FRAME_death309,
		FRAME_death310,
		FRAME_death311,
		FRAME_death312,
		FRAME_death313,
		FRAME_death314,
		FRAME_death315,
		FRAME_death316,
		FRAME_death317,
		FRAME_death318,
		FRAME_death319,
		FRAME_death320,
		FRAME_death321,
		FRAME_death322,
		FRAME_death323,
		FRAME_death324,
		FRAME_death325,
		FRAME_death326,
		FRAME_death327,
		FRAME_death328,
		FRAME_death329,
		FRAME_death330,
		FRAME_death331,
		FRAME_death332,
		FRAME_death333,
		FRAME_death334,
		FRAME_death335,
		FRAME_death336,
		FRAME_death337,
		FRAME_death338,
		FRAME_death339,
		FRAME_death340,
		FRAME_death341,
		FRAME_death342,
		FRAME_death343,
		FRAME_death344,
		FRAME_death345,
		FRAME_death401,
		FRAME_death402,
		FRAME_death403,
		FRAME_death404,
		FRAME_death405,
		FRAME_death406,
		FRAME_death407,
		FRAME_death408,
		FRAME_death409,
		FRAME_death410,
		FRAME_death411,
		FRAME_death412,
		FRAME_death413,
		FRAME_death414,
		FRAME_death415,
		FRAME_death416,
		FRAME_death417,
		FRAME_death418,
		FRAME_death419,
		FRAME_death420,
		FRAME_death421,
		FRAME_death422,
		FRAME_death423,
		FRAME_death424,
		FRAME_death425,
		FRAME_death426,
		FRAME_death427,
		FRAME_death428,
		FRAME_death429,
		FRAME_death430,
		FRAME_death431,
		FRAME_death432,
		FRAME_death433,
		FRAME_death434,
		FRAME_death435,
		FRAME_death436,
		FRAME_death437,
		FRAME_death438,
		FRAME_death439,
		FRAME_death440,
		FRAME_death441,
		FRAME_death442,
		FRAME_death443,
		FRAME_death444,
		FRAME_death445,
		FRAME_death446,
		FRAME_death447,
		FRAME_death448,
		FRAME_death449,
		FRAME_death450,
		FRAME_death451,
		FRAME_death452,
		FRAME_death453,
		FRAME_death501,
		FRAME_death502,
		FRAME_death503,
		FRAME_death504,
		FRAME_death505,
		FRAME_death506,
		FRAME_death507,
		FRAME_death508,
		FRAME_death509,
		FRAME_death510,
		FRAME_death511,
		FRAME_death512,
		FRAME_death513,
		FRAME_death514,
		FRAME_death515,
		FRAME_death516,
		FRAME_death517,
		FRAME_death518,
		FRAME_death519,
		FRAME_death520,
		FRAME_death521,
		FRAME_death522,
		FRAME_death523,
		FRAME_death524,
		FRAME_death601,
		FRAME_death602,
		FRAME_death603,
		FRAME_death604,
		FRAME_death605,
		FRAME_death606,
		FRAME_death607,
		FRAME_death608,
		FRAME_death609,
		FRAME_death610,
	};

	inline float GetScale () { return 1.2f; }

	MONSTER_SOUND_ENUM
	(
		SOUND_IDLE,
		SOUND_SIGHT1,
		SOUND_SIGHT2,
		SOUND_COCK,
		SOUND_PAIN,
		SOUND_DEATH,

		SOUND_MAX
	);

	/**
	\typedef	uint8 ESoldierAIType
	
	\brief	Defines an alias representing soldier AI weapon types.
	**/
	typedef uint8 ESoldierAIType;

	/**
	\enum	
	
	\brief	Values that represent soldier AI weapon types. 
	**/
	enum
	{
		AI_BLASTER,			// Regular firing, run-fire
		AI_MACHINEGUN,		// Special fire frame, no running
		AI_SHOTGUN,			// Regular firing, no run fire
	};

	ESoldierAIType		SoldierAI;

	CSoldierBase (uint32 ID);

	virtual inline const char	*GetMonsterName() { return "Soldier"; }

	void SaveMonsterFields (CFile &File)
	{ 
		SAVE_MONSTER_SOUNDS
		File.Write<ESoldierAIType> (SoldierAI);
	}
	void LoadMonsterFields (CFile &File)
	{
		LOAD_MONSTER_SOUNDS
		SoldierAI = File.Read<ESoldierAIType> ();
	}

	virtual void Attack () = 0;
#if !ROGUE_FEATURES
	void Dodge (IBaseEntity *Attacker, float eta);
#else
	void Dodge (IBaseEntity *Attacker, float eta, CTrace *tr) { MonsterDodge (Attacker, eta, tr); };
#endif
	void Idle ();
	void Run ();
	void Sight ();
	void Stand ();
	void Walk ();

	void CockGun ();
#if !ROGUE_FEATURES
	void Duck_Down ();
	void Duck_Hold ();
	void Duck_Up ();
#else
	void StartCharge ();
	void StopCharge();

	void SideStep ();
	void Duck (float eta);
#endif
	virtual void FireGun (sint32 FlashNumber) = 0;

	void Walk1_Random ();

	void Fire1 ();
	void Fire2 ();
	void Fire3 ();
	void Fire4 ();
	void Fire7 ();
	void Fire6 ();
	void Fire8 ();
	void Attack1_Refire1 ();
	void Attack1_Refire2 ();
	void Attack2_Refire1 ();
	void Attack2_Refire2 ();
	void Attack3_Refire ();
	void Attack6_Refire ();
#if ROGUE_FEATURES
	void Attack6_RefireBlaster();
#endif

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn (); // Initialize "commonalities"
	virtual void SpawnSoldier () = 0; // Initialize health, etc.
};

#else
FILE_WARNING
#endif
