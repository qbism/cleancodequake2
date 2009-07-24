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
// cc_menu.h
// Menu system
//

class CMenuItem abstract
{
public:
	int				x, y;
	bool			Selected;
	bool			Enabled;
	class CMenu		*Menu; // Menu that owns this object

	CMenuItem		(class CMenu *Menu, int x, int y);

	virtual void	Draw (CPlayerEntity *ent, CStatusBar *DrawState) = 0;

	virtual bool	CanSelect (CPlayerEntity *ent); // Returns false if the option can't be selected
	virtual void	Update (CPlayerEntity *ent); // Updates the control
	virtual bool	Select (CPlayerEntity *ent); // What happens when you hit enter (return true to close the menu)
};

class CMenu abstract 
{
public:
	int							Cursor; // Where to position the cursor on start
	CPlayerEntity				*ent; // A pointer to the entity that is running the show (needed?)
	std::vector<CMenuItem*>		Items;

	CMenu						(CPlayerEntity *ent);
	~CMenu						();

	virtual bool				Open () = 0;
	virtual void				Close () = 0;

	void						DrawItems(CStatusBar *Bar);
	virtual void				Draw (bool reliable) = 0;
	void						AddItem (CMenuItem *Item);
};

enum EMenuKeys
{
	KEY_NONE,
	KEY_LEFT,
	KEY_RIGHT
};

// All players have a copy of this class.
class CMenuState
{
public:
	CPlayerEntity		*ent; // A pointer to the entity that is running the show (needed?)
	int					Cursor; // Cursor position (relative to order)
	EMenuKeys			Key; // Key hit this frame
	// CurrentMenu is set to a copy of whatever menu class
	// you need
	CMenu				*CurrentMenu;
	bool				InMenu;

	CMenuState			();
	CMenuState			(CPlayerEntity *ent);

	void OpenMenu		(); // Do this AFTER setting CurrentMenu
	void CloseMenu		();

	void SelectNext		(); // invnext
	void SelectPrev		(); // invprev
	void Select			(); // invuse
};

void Cmd_MenuLeft_t (CPlayerEntity *ent);
void Cmd_MenuRight_t (CPlayerEntity *ent);

// Controls
#include "cc_controls.h"