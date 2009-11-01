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
// cc_controls.h
// Common controls
//

#if !defined(__CC_CONTROLS_H__) || !defined(INCLUDE_GUARDS)
#define __CC_CONTROLS_H__

#include <sstream>
#include "cc_conchars.h"

CC_ENUM (uint8, ELabelFlags)
{
	LF_GREEN			=	1,
};

CC_ENUM (uint8, ELabelAlign)
{
	LA_LEFT,
	LA_CENTER,
	LA_RIGHT
};

class CMenu_Label : public CMenuItem
{
public:
	std::cc_string			LabelString;
	ELabelFlags				Flags;
	ELabelAlign				Align;

	CMenu_Label				(CMenu *Menu, int x, int y);
	virtual void Draw		(CPlayerEntity *ent, CStatusBar *DrawState);

	virtual bool	CanSelect (CPlayerEntity *ent)
	{
		return Enabled;
	}
	virtual bool	Select (CPlayerEntity *ent)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *ent)
	{
	}
};

class CMenu_Image : public CMenuItem
{
public:
	std::cc_string			ImageString;
	int						Width, Height;

	CMenu_Image				(CMenu *Menu, int x, int y);
	virtual void Draw		(CPlayerEntity *ent, CStatusBar *DrawState);

	virtual bool	CanSelect (CPlayerEntity *ent)
	{
		return Enabled;
	}
	virtual bool	Select (CPlayerEntity *ent)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *ent)
	{
	}
};

struct SSpinControlIndex
{
	char		*Text;
	char		*Value;
};

class CMenu_Spin : public CMenuItem
{
public:
	ELabelFlags				Flags;
	ELabelAlign				Align;
	int						Index; // Where we are in the spin control
	int						NumIndices;
	SSpinControlIndex		*Indices;

	CMenu_Spin				(CMenu *Menu, int x, int y, SSpinControlIndex *Indices);
	virtual void Draw		(CPlayerEntity *ent, CStatusBar *DrawState);

	virtual bool	CanSelect (CPlayerEntity *ent)
	{
		return Enabled;
	}
	virtual bool	Select (CPlayerEntity *ent)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *ent);
};

CC_ENUM (uint8, ESliderTextPosition)
{
	STP_RIGHT,
	STP_TOP,
	STP_LEFT,
	STP_BOTTOM,

	STP_CUSTOM // Allows programmer to shove the text where ever he wants
};

template <typename TType>
class CMenu_Slider : public CMenuItem
{
public:
	ELabelAlign				Align;
	ESliderTextPosition		TextAlign;
	int						TextX, TextY;
	uint8					Width;

	TType					Min;
	TType					Max;
	TType					Step;
	TType					Value;
	std::cc_string			AppendText;

	CMenu_Slider			(CMenu *Menu, int x, int y) :
	  CMenuItem(Menu, x, y)
	{
	};

	virtual void Draw		(CPlayerEntity *ent, CStatusBar *DrawState)
	{
		if (Width > (MAX_INFO_KEY*2)-3)
			Width = (MAX_INFO_KEY*2)-3;

		int drawX = x;

		switch (Align)
		{
		case LA_LEFT:
			drawX += 160;
			break;
		case LA_RIGHT:
			drawX += 160 - (Width * 8);
			break;
		case LA_CENTER:
			break;
		}
		DrawState->AddVirtualPoint_X (drawX);
		DrawState->AddVirtualPoint_Y (y + 120);

		char Buffer[MAX_INFO_KEY*2];
		Buffer[0] = CCHAR_DOWNLOADBAR_LEFT;

		// Which number is closest to the value?
		float Percent = (((!Value) ? 0.1 : ((float)Value / (float)Max)));
		int BestValue = ((Width-1) * Percent);

		if (BestValue > Width)
			BestValue = Width;

		for (int i = (int)Min; i <= (int)Width; i++)
		{
			Buffer[((i-(int)Min)+1)] = (i == BestValue) ? CCHAR_DOWNLOADBAR_THUMB : CCHAR_DOWNLOADBAR_CENTER;
		}

		Buffer[Width+1] = CCHAR_DOWNLOADBAR_RIGHT;
		Buffer[Width+2] = '\0';

		DrawState->AddString (Buffer, false, (Align == LA_CENTER));

		// Draw the value if desired
		switch (Align)
		{
		case LA_LEFT:
			drawX = x + 190 + (Width * 8);
			break;
		case LA_RIGHT:
			drawX = x + 190;
			break;
		case LA_CENTER:
			drawX = x + 145 + (Width * 8);
			break;
		}

		DrawState->AddVirtualPoint_X (drawX);

		std::stringstream str;
		str << Value << AppendText;
		DrawState->AddString (str.str().c_str(), Selected, false);
	};

	virtual bool	CanSelect (CPlayerEntity *ent)
	{
		return Enabled;
	}
	virtual bool	Select (CPlayerEntity *ent)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *ent)
	{
		switch (ent->Client.Respawn.MenuState.Key)
		{
		case CMenuState::KEY_RIGHT:
			if (Value == Max)
				return; // Can't do that, Dave

			Value += Step;
			if (Value > Max)
				Value = Max;
			break;
		case CMenuState::KEY_LEFT:
			if (Value == Min)
				return;

			Value -= Step;
			if (Value < Min)
				Value = Min;
			break;
		}
	};
};

class CMenu_Box : public CMenuItem
{
private:
	bool					Enabled;

public:
	ELabelAlign				Align;

	int						Width;
	int						Height;
	int						Type;

	CMenu_Box			(CMenu *Menu, int x, int y);
	virtual void Draw		(CPlayerEntity *ent, CStatusBar *DrawState);

	// Can't select
	bool	CanSelect (CPlayerEntity *ent)
	{
		return false;
	}
	bool	Select (CPlayerEntity *ent)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *ent)
	{
	};
};

#else
FILE_WARNING
#endif