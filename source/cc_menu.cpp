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
// cc_menu.cpp
// Menu system
//

#include "cc_local.h"
#include "cc_menu.h"

CMenuItem::CMenuItem (CMenu *Menu, int x, int y) :
Menu(Menu),
x(x),
y(y),
Selected(Selected),
Enabled(true)
{
};

bool CMenuItem::CanSelect (CPlayerEntity *ent)
{
	return Enabled;
}

void CMenuItem::Update (CPlayerEntity *ent)
{
}

bool CMenuItem::Select (CPlayerEntity *ent)
{
	return false;
}

CMenu::CMenu (CPlayerEntity *ent) :
Cursor(0),
ent(ent)
{
};

CMenu::~CMenu ()
{
	for (uint32 i = 0; i < Items.size(); i++)
	{
		CMenuItem *Item = Items[i];
		QDelete Item;
	}
};

void CMenu::AddItem (CMenuItem *Item)
{
	// Insert in the list
	Items.push_back (Item);
}

void CMenu::DrawItems (CStatusBar *Bar)
{
	for (std::vector<CMenuItem*, std::level_allocator<CMenuItem*> >::iterator it = Items.begin(); it < Items.end(); it++)
		(*it)->Draw (ent, Bar);
}

CMenuState::CMenuState ()
{
};

CMenuState::CMenuState (CPlayerEntity *ent) :
ent(ent),
Cursor(-1),
CurrentMenu(NULL),
InMenu(false)
{
};

void CMenuState::OpenMenu ()
{
	if (!CurrentMenu)
	{
		DebugPrintf ("CMenuState::OpenMenu called with no menu!\n");
		return;
	}

	if (!CurrentMenu->Open ())
	{
		// No dice
		QDelete CurrentMenu;
		return;
	}

	// Menu successfully opened!
	Cursor = CurrentMenu->Cursor;
	InMenu = true;

	CurrentMenu->Items.at(Cursor)->Selected = true;
};

void CMenuState::CloseMenu ()
{
	if (!CurrentMenu)
	{
		DebugPrintf ("CMenuState::CloseMenu called with no menu!\n");
		return;
	}

	CurrentMenu->Close (); // Shut all of it down
	QDelete CurrentMenu; // Delete it

	Cursor = -1;
	InMenu = false;
}

void CMenuState::SelectNext ()
{
	int i = 0;

	if (!CurrentMenu)
	{
		DebugPrintf ("CMenuState::SelectNext called with no menu!\n");
		return;
	}

	// Unselect what is selected currently
	if (Cursor != -1)
		CurrentMenu->Items.at(Cursor)->Selected = false;

	// Find the next selectable item
	if (Cursor == -1)
		Cursor = 0; // Temporary

	for (i = Cursor; i < (signed)CurrentMenu->Items.size(); i++)
	{
		if (i == Cursor)
			continue;

		CMenuItem *Item = CurrentMenu->Items.at(i);
		if (Item && Item->CanSelect(ent))
		{
			Cursor = i;
			CurrentMenu->Items.at(Cursor)->Selected = true;
			return;
		}
	}
	
	// Couldn't find an item after cursor
	// Try from 0
	for (i = 0; i < (signed)CurrentMenu->Items.size(); i++)
	{
		if (i == Cursor)
			continue;

		CMenuItem *Item = CurrentMenu->Items.at(i);
		if (Item && Item->CanSelect(ent))
		{
			Cursor = i;
			CurrentMenu->Items.at(Cursor)->Selected = true;
			return;
		}
	}

	// Couldn't find any item
	Cursor = -1;
	return;
}

void CMenuState::SelectPrev ()
{
	int i = 0;

	if (!CurrentMenu)
	{
		DebugPrintf ("CMenuState::SelectPrev called with no menu!\n");
		return;
	}

	// Unselect what is selected currently
	if (Cursor != -1)
		CurrentMenu->Items.at(Cursor)->Selected = false;

	// Find the previous selectable item
	if (Cursor == -1)
		Cursor = 0; // Temporary

	for (i = Cursor; i >= 0; i--)
	{
		if (i == Cursor)
			continue;

		CMenuItem *Item = CurrentMenu->Items.at(i);
		if (Item && Item->CanSelect(ent))
		{
			Cursor = i;
			CurrentMenu->Items.at(Cursor)->Selected = true;
			return;
		}
	}

	// Couldn't find an item before cursor
	// Try from end
	for (i = (signed)CurrentMenu->Items.size()-1; i >= 0; i--)
	{
		if (i == Cursor)
			continue;

		CMenuItem *Item = CurrentMenu->Items.at(i);
		if (Item && Item->CanSelect(ent))
		{
			Cursor = i;
			CurrentMenu->Items.at(Cursor)->Selected = true;
			return;
		}
	}

	// Couldn't find any item
	Cursor = -1;
	return;
}

void CMenuState::Select ()
{
	if (!CurrentMenu)
	{
		DebugPrintf ("CMenuState::Select called with no menu!\n");
		return;
	}

	if (Cursor == -1)
		return;

	if (Cursor > (signed)(CurrentMenu->Items.size()-1))
	{
		DebugPrintf ("Funny cursor size\n");
		return;
	}

	if (CurrentMenu->Items.at(Cursor)->Select(ent))
		CloseMenu ();
}

void Cmd_MenuLeft_t (CPlayerEntity *Player)
{
	if (!Player->Client.Respawn.MenuState.InMenu)
		return;

	Player->Client.Respawn.MenuState.Key = CMenuState::KEY_LEFT;

	// Update the currently selected control
	if (Player->Client.Respawn.MenuState.Cursor != -1)
		Player->Client.Respawn.MenuState.CurrentMenu->Items.at(Player->Client.Respawn.MenuState.Cursor)->Update(Player);

	Player->Client.Respawn.MenuState.Key = CMenuState::KEY_NONE;
}

void Cmd_MenuRight_t (CPlayerEntity *Player)
{
	if (!Player->Client.Respawn.MenuState.InMenu)
		return;

	Player->Client.Respawn.MenuState.Key = CMenuState::KEY_RIGHT;

	// Update the currently selected control
	if (Player->Client.Respawn.MenuState.Cursor != -1)
		Player->Client.Respawn.MenuState.CurrentMenu->Items.at(Player->Client.Respawn.MenuState.Cursor)->Update(Player);

	Player->Client.Respawn.MenuState.Key = CMenuState::KEY_NONE;
}
