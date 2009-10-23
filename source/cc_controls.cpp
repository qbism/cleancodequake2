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
// cc_controls.cpp
// Common controls
//

#include "cc_local.h"
#include "cc_menu.h"
#include "cc_conchars.h"

CMenu_Label::CMenu_Label (CMenu *Menu, int x, int y) :
CMenuItem(Menu, x, y)
{
};

void CMenu_Label::Draw (CPlayerEntity *ent, CStatusBar *DrawState)
{
	bool high = (Flags & LF_GREEN);
	DrawState->AddVirtualPoint_Y (y + 120);

	if (Selected)
	{
		char arrowThing[2] = {CCHAR_ARROW,'\0'};
		int arrowX = x;

		switch (Align)
		{
		case LA_LEFT:
			arrowX += (160 - 12);
			break;
		case LA_CENTER:
			arrowX -= 8 + (((int)strlen(LabelString)*8)/2);
			break;
		case LA_RIGHT:
			arrowX += (160 - ((int)strlen(LabelString)*8)) - 12;
			break;
		}
		DrawState->AddVirtualPoint_X (arrowX);

		DrawState->AddString (arrowThing, true, (Align == LA_CENTER));
		high = true;
	}
	int drawX = x;

	switch (Align)
	{
	case LA_LEFT:
		drawX += 160;
		break;
	case LA_CENTER:
		break;
	case LA_RIGHT:
		drawX += 160 - ((int)strlen(LabelString)*8);
		break;
	}

	DrawState->AddVirtualPoint_X (drawX);
	DrawState->AddString (LabelString, high, (Align == LA_CENTER));
};

CMenu_Image::CMenu_Image (CMenu *Menu, int x, int y) :
CMenuItem(Menu, x, y)
{
};

void CMenu_Image::Draw (CPlayerEntity *ent, CStatusBar *DrawState)
{
	DrawState->AddVirtualPoint_X ((x + 160) - Width/2);
	DrawState->AddVirtualPoint_Y ((y + 120) - Height/2);
	DrawState->AddPic (ImageString);

	if (Selected)
	{
		DrawState->AddVirtualPoint_X ((x + 160) - Width);		
		char arrowThing[2] = {CCHAR_ARROW,'\0'};
		DrawState->AddString (arrowThing, true, false);
	}
};

CMenu_Spin::CMenu_Spin (CMenu *Menu, int x, int y, SSpinControlIndex *Indices) :
CMenuItem(Menu, x, y),
Indices(Indices)
{
	Index = 0;
	//NumIndices = (sizeof(Indices) / sizeof(SSpinControlIndex

	while (Indices->Text && *(Indices)->Text)
	{
		NumIndices++;
		Indices++;
	}
	Indices = Indices;
};

void CMenu_Spin::Draw (CPlayerEntity *ent, CStatusBar *DrawState)
{
	int drawX = x;

	switch (Align)
	{
	case LA_LEFT:
		drawX += 160;
		break;
	case LA_CENTER:
		break;
	case LA_RIGHT:
		drawX += 160 - ((int)strlen(Indices[Index].Text)*8);
		break;
	}

	DrawState->AddVirtualPoint_X (drawX);
	DrawState->AddVirtualPoint_Y (y + 120);

	DrawState->AddString (Indices[Index].Text, Selected, (Align == LA_CENTER));

	if (Selected)
	{
		int numCharsOfSpace = (int)strlen(Indices[Index].Text)*8;
		// Is there any more indices to the left?
		if (Index > 0)
		{
			switch (Align)
			{
			case LA_LEFT:
				drawX = x + 136;
				break;
			case LA_CENTER:
				drawX = x - ((numCharsOfSpace / 2) + 24);
				break;
			case LA_RIGHT:
				drawX = x + (136 + (8 * 2)) - (numCharsOfSpace + (8 * 3));
				break;
			};
			DrawState->AddVirtualPoint_X (drawX);
			DrawState->AddString ("<", false, (Align == LA_CENTER));
		}

		// To the right?
		if (Index < (NumIndices-1))
		{
			switch (Align)
			{
			case LA_LEFT:
				drawX = x + 160 + numCharsOfSpace + (8 * 2);
				break;
			case LA_CENTER:
				drawX = x + ((numCharsOfSpace / 2) + 24);
				break;
			case LA_RIGHT:
				drawX = x + 136 + (8 * 6);
				break;
			};
			DrawState->AddVirtualPoint_X (drawX);
			DrawState->AddString (">", false, (Align == LA_CENTER));
		}
	}
};

void CMenu_Spin::Update (CPlayerEntity *ent)
{
	switch (ent->Client.Respawn.MenuState.Key)
	{
	case CMenuState::KEY_RIGHT:
		if (Index == (NumIndices-1))
			return; // Can't do that, Dave

		Index++;
		break;
	case CMenuState::KEY_LEFT:
		if (Index == 0)
			return;

		Index--;
		break;
	}
};

CMenu_Slider::CMenu_Slider (CMenu *Menu, int x, int y) :
CMenuItem(Menu, x, y)
{
};

void CMenu_Slider::Draw (CPlayerEntity *ent, CStatusBar *DrawState)
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
	float Percent = (((Value == 0) ? 0.1 : ((float)Value / (float)Max)));
	int BestValue = ((Width-1) * Percent);

	if (BestValue > Width)
		BestValue = Width;

	for (int i = Min; i <= Width; i++)
	{
		Buffer[((i-Min)+1)] = (i == BestValue) ? CCHAR_DOWNLOADBAR_THUMB : CCHAR_DOWNLOADBAR_CENTER;
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
	itoa (Value, Buffer, 10);

	DrawState->AddString (Buffer, Selected, false);
};

void CMenu_Slider::Update (CPlayerEntity *ent)
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

CMenu_Box::CMenu_Box (CMenu *Menu, int x, int y) :
CMenuItem(Menu, x, y)
{
};

void CMenu_Box::Draw (CPlayerEntity *ent, CStatusBar *DrawState)
{
	static char	*Buf = QNew (com_genericPool, 0) char[MAX_COMPRINT/2];
	int			DrawX = (Align == LA_CENTER) ? x : x + 160, DrawY = y + 120, Index = 0;
	int			W = Width+2, H = Height+2;

	DrawState->AddVirtualPoint_X (DrawX);
	DrawState->AddVirtualPoint_Y (DrawY);

	// Setup buffer
	switch (Type)
	{
	case 0:
		for (int tY = 0; tY < H; tY++)
		{
			for (int tX = 0; tX < W; tX++)
			{
				if (tY == 0)
				{
					if (tX == 0)
						Buf[Index++] = CCHAR_CONTAINER1_UPPERLEFT;
					else if (tX == (W - 1))
						Buf[Index++] = CCHAR_CONTAINER1_UPPERRIGHT;
					else
						Buf[Index++] = CCHAR_CONTAINER1_UPPERCENTER;
				}
				else if (tY == (H - 1))
				{
					if (tX == 0)
						Buf[Index++] = CCHAR_CONTAINER1_LOWERLEFT;
					else if (tX == (W - 1))
						Buf[Index++] = CCHAR_CONTAINER1_LOWERRIGHT;
					else
						Buf[Index++] = CCHAR_CONTAINER1_LOWERCENTER;
				}
				else
				{
					if (tX == 0)
						Buf[Index++] = CCHAR_CONTAINER1_MIDDLELEFT;
					else if (tX == (W - 1))
						Buf[Index++] = CCHAR_CONTAINER1_MIDDLERIGHT;
					else
						Buf[Index++] = CCHAR_CONTAINER1_MIDDLECENTER;
				}
			}
			Buf[Index++] = '\n';
		}
		break;
	case 1:
		for (int tY = 0; tY < H; tY++)
		{
			for (int tX = 0; tX < W; tX++)
			{
				if (tY == 0)
				{
					if (tX == 0)
						Buf[Index++] = CCHAR_CONTAINER2_UPPERLEFT;
					else if (tX == (W - 1))
						Buf[Index++] = CCHAR_CONTAINER2_UPPERRIGHT;
					else
						Buf[Index++] = CCHAR_CONTAINER2_UPPERCENTER;
				}
				else if (tY == (H - 1))
				{
					if (tX == 0)
						Buf[Index++] = CCHAR_CONTAINER2_LOWERLEFT;
					else if (tX == (W - 1))
						Buf[Index++] = CCHAR_CONTAINER2_LOWERRIGHT;
					else
						Buf[Index++] = CCHAR_CONTAINER2_LOWERCENTER;
				}
				else
				{
					if (tX == 0)
						Buf[Index++] = CCHAR_CONTAINER2_MIDDLELEFT;
					else if (tX == (W - 1))
						Buf[Index++] = CCHAR_CONTAINER2_MIDDLERIGHT;
					else
						Buf[Index++] = CCHAR_CONTAINER2_MIDDLECENTER;
				}
			}
			Buf[Index++] = '\n';
		}
		break;
	case 2:
		for (int tX = 0; tX < W; tX++)
		{
			if (tX == 0)
				Buf[Index++] = CCHAR_BAR1_LEFT;
			else if (tX == (W - 1))
				Buf[Index++] = CCHAR_BAR1_RIGHT;
			else
				Buf[Index++] = CCHAR_BAR1_CENTER;
		}
		Buf[Index++] = '\n';
		break;
	}
	Buf[Index] = '\0';

	DrawState->AddString (Buf, false, (Align == LA_CENTER));
};