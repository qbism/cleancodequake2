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
// cc_ctfadmin.cpp
// 
//

#include "cc_local.h"
#include "cc_menu.h"

#if CLEANCTF_ENABLED
bool CTFBeginElection(CPlayerEntity *Player, EElectState type, char *msg);
void CTFResetAllPlayers();

class CCTFSettingsMenu : public CMenu
{
public:
	CMenu_Slider<sint32>	*MatchStartLength;
	CMenu_Slider<float>	*MatchLength,
						*MatchSetupLength;

	CMenu_Spin			*WeaponsStaySpin,
						*InstantItemsSpin,
						*QuadDropSpin,
						*InstantWeaponsSpin,
						*MatchLockSpin;

	class CCloseLabel : public CMenu_Label
	{
	public:
		CCloseLabel(CCTFSettingsMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *Player)
		{
			return true;
		};
	};
	
	class CApplyLabel : public CMenu_Label
	{
	public:
		CCTFSettingsMenu *MyMenu;

		CApplyLabel(CCTFSettingsMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y),
		MyMenu(Menu)
		{
		};

		bool Select (CPlayerEntity *Player)
		{
			if (MyMenu->MatchLength->Value != CvarList[CV_MATCH_TIME].Float())
			{
				BroadcastPrintf (PRINT_HIGH, "%s changed the match length to %d minutes.\n",
					Player->Client.Persistent.Name.c_str(), MyMenu->MatchLength->Value);

				if (ctfgame.match == MATCH_GAME)
					// in the middle of a match, change it on the fly
					ctfgame.matchtime = (ctfgame.matchtime - CvarList[CV_MATCH_TIME].Float()*600) + MyMenu->MatchLength->Value*600;

				CvarList[CV_MATCH_TIME].Set (MyMenu->MatchLength->Value);
			}

			if (MyMenu->MatchSetupLength->Value != CvarList[CV_MATCH_SETUP_TIME].Float())
			{
				BroadcastPrintf (PRINT_HIGH, "%s changed the match setup time to %d minutes.\n",
					Player->Client.Persistent.Name.c_str(), MyMenu->MatchSetupLength->Value);

				if (ctfgame.match == MATCH_SETUP)
					// in the middle of a match, change it on the fly
					ctfgame.matchtime = (ctfgame.matchtime - CvarList[CV_MATCH_SETUP_TIME].Float()*60) + MyMenu->MatchSetupLength->Value*60;


				CvarList[CV_MATCH_SETUP_TIME].Set (MyMenu->MatchSetupLength->Value);
			}

			if (MyMenu->MatchStartLength->Value != CvarList[CV_MATCH_START_TIME].Integer())
			{
				BroadcastPrintf(PRINT_HIGH, "%s changed the match start time to %d seconds.\n",
					Player->Client.Persistent.Name.c_str(), MyMenu->MatchStartLength->Value);

				if (ctfgame.match == MATCH_PREGAME)
					// in the middle of a match, change it on the fly
					ctfgame.matchtime = (ctfgame.matchtime - (CvarList[CV_MATCH_START_TIME].Integer()*10)) + (MyMenu->MatchStartLength->Value*10);

				CvarList[CV_MATCH_START_TIME].Set (MyMenu->MatchStartLength->Value);
			}

			sint32 i = CvarList[CV_DMFLAGS].Integer();
			if (!!MyMenu->WeaponsStaySpin->Index != DeathmatchFlags.dfWeaponsStay.IsEnabled())
			{
				BroadcastPrintf(PRINT_HIGH, "%s turned %s weapons stay.\n",
					Player->Client.Persistent.Name.c_str(), MyMenu->WeaponsStaySpin->Index ? "on" : "off");

				if (MyMenu->WeaponsStaySpin->Index)
					i |= DF_WEAPONS_STAY;
				else
					i &= ~DF_WEAPONS_STAY;
			}

			if (!!MyMenu->InstantItemsSpin->Index != DeathmatchFlags.dfInstantItems.IsEnabled())
			{
				BroadcastPrintf(PRINT_HIGH, "%s turned %s instant items.\n",
					Player->Client.Persistent.Name.c_str(), MyMenu->InstantItemsSpin->Index ? "on" : "off");

				if (MyMenu->InstantItemsSpin->Index)
					i |= DF_INSTANT_ITEMS;
				else
					i &= ~DF_INSTANT_ITEMS;
			}

			if (!!MyMenu->QuadDropSpin->Index != DeathmatchFlags.dfQuadDrop.IsEnabled())
			{
				BroadcastPrintf(PRINT_HIGH, "%s turned %s quad drop.\n",
					Player->Client.Persistent.Name.c_str(), MyMenu->QuadDropSpin->Index ? "on" : "off");

				if (MyMenu->QuadDropSpin->Index)
					i |= DF_QUAD_DROP;
				else
					i &= ~DF_QUAD_DROP;
			}

			CvarList[CV_DMFLAGS].Set (i);

			if (!!MyMenu->InstantWeaponsSpin->Index != CvarList[CV_INSTANT_WEAPONS].Boolean())
			{
				BroadcastPrintf(PRINT_HIGH, "%s turned %s instant weapons.\n",
					Player->Client.Persistent.Name.c_str(), MyMenu->InstantWeaponsSpin->Index ? "on" : "off");

				CvarList[CV_INSTANT_WEAPONS].Set (MyMenu->InstantWeaponsSpin->Index);
			}

			if (!!MyMenu->MatchLockSpin->Index != CvarList[CV_MATCH_LOCK].Boolean())
			{
				BroadcastPrintf(PRINT_HIGH, "%s turned %s match lock.\n",
					Player->Client.Persistent.Name.c_str(), MyMenu->MatchLockSpin->Index ? "on" : "off");

				CvarList[CV_MATCH_LOCK].Set (MyMenu->MatchLockSpin->Index);
			}

			return true;
		};
	};

	CCTFSettingsMenu	(CPlayerEntity *Player) :
	  CMenu (Player),
	  WeaponsStaySpin (NULL),
	  InstantItemsSpin (NULL),
	  QuadDropSpin (NULL),
	  InstantWeaponsSpin (NULL),
	  MatchLockSpin (NULL)
	  {
	  };

	bool Open ()
	{
		sint32 x = 0, y = 0;
		static SSpinControlIndex YesNoValues[] =
		{
			{ "Off", "0" },
			{ "On", "1" },
			{ NULL, NULL},
		};

		CMenu_Image *Background = QNew (com_levelPool, 0) CMenu_Image (this, x, y);
		Background->ImageString = "inventory";
		Background->Width = 256;
		Background->Height = 192;
		Background->Enabled = false;

		y = -76; // Top
		CMenu_Label *TopName = QNew (com_levelPool, 0) CMenu_Label(this, x, y);
		TopName->Enabled = false;
		TopName->Align = LA_CENTER;
		TopName->Flags = LF_GREEN;
		TopName->LabelString = "Settings Menu";

		x = -98;
		y += 8 * 3;

		x += (8 * 16);
		MatchLength = QNew (com_levelPool, 0) CMenu_Slider<float> (this, x, y);
		MatchLength->Align = LA_LEFT;
		MatchLength->AppendText = " min";
		MatchLength->Min = 0.5f;
		MatchLength->Max = 60.0f;
		MatchLength->Step = 0.5f;
		MatchLength->Width = 6;
		MatchLength->Value = CvarList[CV_MATCH_LOCK].Float();
		MatchLength->Enabled = true;
		MatchLength->x += 8 * 2;

		y += 8;
		MatchSetupLength = QNew (com_levelPool, 0) CMenu_Slider<float> (this, x, y);
		MatchSetupLength->Align = LA_LEFT;
		MatchSetupLength->AppendText = " min";
		MatchSetupLength->Min = 0.5f;
		MatchSetupLength->Max = 60.0f;
		MatchSetupLength->Step = 0.5f;
		MatchSetupLength->Width = 6;
		MatchSetupLength->Value = CvarList[CV_MATCH_SETUP_TIME].Float();
		MatchSetupLength->Enabled = true;
		MatchSetupLength->x += 8 * 2;

		y += 8;
		MatchStartLength = QNew (com_levelPool, 0) CMenu_Slider<sint32> (this, x, y);
		MatchStartLength->Align = LA_LEFT;
		MatchStartLength->AppendText = " sec";
		MatchStartLength->Min = 0;
		MatchStartLength->Max = 60.0f;
		MatchStartLength->Step = 1;
		MatchStartLength->Width = 6;
		MatchStartLength->Value = CvarList[CV_MATCH_START_TIME].Integer();
		MatchStartLength->Enabled = true;
		MatchStartLength->x += 8 * 2;

		y += 8;
		WeaponsStaySpin = QNew (com_levelPool, 0) CMenu_Spin (this, x, y, &YesNoValues[0]);
		WeaponsStaySpin->Index = DeathmatchFlags.dfWeaponsStay.IsEnabled();
		WeaponsStaySpin->x += (8 * 4);
		WeaponsStaySpin->Align = LA_LEFT;

		y += 8;
		InstantItemsSpin = QNew (com_levelPool, 0) CMenu_Spin (this, x, y, &YesNoValues[0]);
		InstantItemsSpin->Index = DeathmatchFlags.dfInstantItems.IsEnabled();
		InstantItemsSpin->x += (8 * 4);
		InstantItemsSpin->Align = LA_LEFT;

		y += 8;
		QuadDropSpin = QNew (com_levelPool, 0) CMenu_Spin (this, x, y, &YesNoValues[0]);
		QuadDropSpin->Index = DeathmatchFlags.dfQuadDrop.IsEnabled();
		QuadDropSpin->x += (8 * 4);
		QuadDropSpin->Align = LA_LEFT;

		y += 8;
		InstantWeaponsSpin = QNew (com_levelPool, 0) CMenu_Spin (this, x, y, &YesNoValues[0]);
		InstantWeaponsSpin->Index = CvarList[CV_INSTANT_WEAPONS].Boolean();
		InstantWeaponsSpin->x += (8 * 4);
		InstantWeaponsSpin->Align = LA_LEFT;

		y += 8;
		MatchLockSpin = QNew (com_levelPool, 0) CMenu_Spin (this, x, y, &YesNoValues[0]);
		MatchLockSpin->Index = CvarList[CV_MATCH_LOCK].Boolean();
		MatchLockSpin->x += (8 * 4);
		MatchLockSpin->Align = LA_LEFT;

		x = -98;
		y = -76 + 8 * 3; // Top

		CMenu_Label *MatchLengthLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		MatchLengthLabel->Enabled = false;
		MatchLengthLabel->LabelString = "Match Len";
		MatchLengthLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *MatchSetupLengthLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		MatchSetupLengthLabel->Enabled = false;
		MatchSetupLengthLabel->LabelString = "Match Setup Len";
		MatchSetupLengthLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *MatchStartLengthLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		MatchStartLengthLabel->Enabled = false;
		MatchStartLengthLabel->LabelString = "Match Start Len";
		MatchStartLengthLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *WeaponsStayLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		WeaponsStayLabel->Enabled = false;
		WeaponsStayLabel->LabelString = "Weapons Stay";
		WeaponsStayLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *InstantItemsLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		InstantItemsLabel->Enabled = false;
		InstantItemsLabel->LabelString = "Instant Items";
		InstantItemsLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *QuadDropLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		QuadDropLabel->Enabled = false;
		QuadDropLabel->LabelString = "Quad Drop";
		QuadDropLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *InstantWeaponsLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		InstantWeaponsLabel->Enabled = false;
		InstantWeaponsLabel->LabelString = "Instant Weapons";
		InstantWeaponsLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *MatchLockLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		MatchLockLabel->Enabled = false;
		MatchLockLabel->LabelString = "Match Lock";
		MatchLockLabel->Flags = LF_GREEN;

		y += 16;
		CApplyLabel *ApplyLabel = QNew (com_levelPool, 0) CApplyLabel (this, x, y);
		ApplyLabel->Enabled = true;
		ApplyLabel->LabelString = "Apply";
		ApplyLabel->Flags = LF_GREEN;

		y += 8;
		CCloseLabel *CancelLabel = QNew (com_levelPool, 0) CCloseLabel (this, x, y);
		CancelLabel->Enabled = true;
		CancelLabel->LabelString = "Cancel";
		CancelLabel->Flags = LF_GREEN;

		return true;
	};

	void				Close ()
	{
	};

	void				Draw (bool reliable)
	{
		CStatusBar Bar;

		DrawItems(&Bar);
		Bar.SendMsg(Player, reliable);
	};
};

void CTFOpenAdminSettings (CPlayerEntity *Player)
{
	if (Player->Client.Respawn.MenuState.InMenu)
		return;

	Player->Client.Respawn.MenuState.CurrentMenu = QNew (com_levelPool, 0) CCTFSettingsMenu(Player);
	Player->Client.Respawn.MenuState.OpenMenu ();

	Player->Client.Respawn.MenuState.CurrentMenu->Draw (true);
}

class CCTFAdminMenu : public CMenu
{
public:
	CCTFAdminMenu		(CPlayerEntity *Player) :
	  CMenu (Player)
	  {
	  };
	  
	class CMatchSetLabel : public CMenu_Label
	{
	public:
		CMatchSetLabel(CCTFAdminMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *Player)
		{
			switch (ctfgame.match)
			{
			case MATCH_SETUP:
				BroadcastPrintf (PRINT_CHAT, "Match has been forced to start.\n");
				ctfgame.match = MATCH_PREGAME;
				ctfgame.matchtime = Level.Frame + (CvarList[CV_MATCH_START_TIME].Float() * 10);
				break;
			case MATCH_GAME:
				BroadcastPrintf(PRINT_CHAT, "Match has been forced to terminate.\n");
				ctfgame.match = MATCH_SETUP;
				ctfgame.matchtime = Level.Frame + (CvarList[CV_MATCH_SETUP_TIME].Float() * 60);
				CTFResetAllPlayers();
				break;
			}
			return true;
		};
	};
	  
	class CMatchModeLabel : public CMenu_Label
	{
	public:
		CMatchModeLabel(CCTFAdminMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *Player)
		{
			if (ctfgame.match != MATCH_SETUP)
			{
				if (CvarList[CV_COMPETITION].Integer() < 3)
					CvarList[CV_COMPETITION].Set (2);
				ctfgame.match = MATCH_SETUP;
				CTFResetAllPlayers();
			}

			return true;
		};
	};

	class CCloseLabel : public CMenu_Label
	{
	public:
		CCloseLabel(CCTFAdminMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *Player)
		{
			return true;
		};
	};

	class COpenSettingsLabel : public CMenu_Label
	{
	public:
		COpenSettingsLabel(CCTFAdminMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *Player)
		{
			Player->Client.Respawn.MenuState.CloseMenu();
			CTFOpenAdminSettings (Player);

			return false;
		};
	};

	bool Open ()
	{
		sint32 x = 0, y = 0;

		CMenu_Image *Background = QNew (com_levelPool, 0) CMenu_Image (this, x, y);
		Background->ImageString = "inventory";
		Background->Width = 256;
		Background->Height = 192;
		Background->Enabled = false;

		y = -76; // Top
		CMenu_Label *TopName = QNew (com_levelPool, 0) CMenu_Label(this, x, y);
		TopName->Enabled = false;
		TopName->Align = LA_CENTER;
		TopName->Flags = LF_GREEN;
		TopName->LabelString = "Quake II\nCleanCode Capture the Flag\nAdministration Menu";

		x = -98;
		y += 8 * 5;
		COpenSettingsLabel *SettingsLabel = QNew (com_levelPool, 0) COpenSettingsLabel(this, x, y);
		SettingsLabel->Enabled = true;
		SettingsLabel->Align = LA_LEFT;
		SettingsLabel->LabelString = "Settings";

		y += 8;

		if (CvarList[CV_COMPETITION].Integer())
		{
			CMenu_Label *MatchLabel;

			switch (ctfgame.match)
			{
			case MATCH_SETUP:
				MatchLabel = QNew (com_levelPool, 0) CMatchSetLabel(this, x, y);
				MatchLabel->LabelString = "Force match start";
				break;
			case MATCH_GAME:
				MatchLabel = QNew (com_levelPool, 0) CMatchSetLabel(this, x, y);
				MatchLabel->LabelString = "Cancel match";
				break;
			case MATCH_NONE:
			default:
				MatchLabel = QNew (com_levelPool, 0) CMatchModeLabel(this, x, y);
				MatchLabel->LabelString = "Switch to match mode";
				break;
			};

			MatchLabel->Enabled = true;
			MatchLabel->Align = LA_LEFT;
		}

		y += 8 * 2;
		CCloseLabel *CloseLabel = QNew (com_levelPool, 0) CCloseLabel(this, x, y);
		CloseLabel->Enabled = true;
		CloseLabel->Align = LA_LEFT;
		CloseLabel->LabelString = "Close";

		return true;
	};

	void				Close ()
	{
	};

	void				Draw (bool reliable)
	{
		CStatusBar Bar;

		DrawItems(&Bar);
		Bar.SendMsg(Player, reliable);
	};
};

void CTFAdmin(CPlayerEntity *Player)
{
	char text[1024];

	if (ArgCount() > 1 && CvarList[CV_ADMIN_PASSWORD].String() && *CvarList[CV_ADMIN_PASSWORD].String() &&
		!Player->Client.Respawn.CTF.Admin && strcmp(CvarList[CV_ADMIN_PASSWORD].String(), ArgGets(1).c_str()) == 0)
	{
		Player->Client.Respawn.CTF.Admin = true;
		BroadcastPrintf(PRINT_HIGH, "%s has become an admin.\n", Player->Client.Persistent.Name.c_str());
		Player->PrintToClient (PRINT_HIGH, "Type 'admin' to access the adminstration menu.\n");

		return;
	}

	if (!Player->Client.Respawn.CTF.Admin)
	{
		Q_snprintfz(text, sizeof(text), "%s has requested admin rights.",
			Player->Client.Persistent.Name.c_str());
		CTFBeginElection(Player, ELECT_ADMIN, text);
		return;
	}

	if (Player->Client.Respawn.MenuState.InMenu)
		return;

	Player->Client.Respawn.MenuState.CurrentMenu = QNew (com_levelPool, 0) CCTFAdminMenu(Player);
	Player->Client.Respawn.MenuState.OpenMenu ();

	Player->Client.Respawn.MenuState.CurrentMenu->Draw (true);
}
#endif
