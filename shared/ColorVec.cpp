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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//
// ColorVec.cpp
//

#include "../source/cc_options.h"
#include "shared.h"

const colorf	Q_FColorBlack(0, 0, 0, 1);
const colorf	Q_FColorRed(1, 0, 0, 1);
const colorf	Q_FColorGreen(0, 1, 0, 1);
const colorf	Q_FColorYellow(1, 1, 0, 1);
const colorf	Q_FColorBlue(0, 0, 1, 1);
const colorf	Q_FColorCyan(0, 1, 1, 1);
const colorf	Q_FColorMagenta(1, 0, 1, 1);
const colorf	Q_FColorWhite(1, 1, 1, 1);

const colorf	Q_FColorLtGrey(0.75, 0.75, 0.75, 1);
const colorf	Q_FColorMdGrey(0.5, 0.5, 0.5, 1);
const colorf	Q_FColorDkGrey(0.25, 0.25, 0.25, 1);

const colorf	Q_FStrColorTable[9] =
{
	colorf(0.0f, 0.0f, 0.0f, 1.0f),
	colorf(1.0f, 0.0f, 0.0f, 1.0f),
	colorf(0.0f, 1.0f, 0.0f, 1.0f),
	colorf(1.0f, 1.0f, 0.0f, 1.0f),
	colorf(0.0f, 0.0f, 1.0f, 1.0f),
	colorf(0.0f, 1.0f, 1.0f, 1.0f),
	colorf(1.0f, 0.0f, 1.0f, 1.0f),
	colorf(1.0f, 1.0f, 1.0f, 1.0f),
	
	colorf(0.75f, 0.75f, 0.75f, 1.0f)
};

const colorb	Q_BColorBlack(0, 0, 0, 255);
const colorb	Q_BColorRed(255, 0, 0, 255);
const colorb	Q_BColorGreen(0, 255, 0, 255);
const colorb	Q_BColorYellow(255, 255, 0, 255);
const colorb	Q_BColorBlue(0, 0, 255, 255);
const colorb	Q_BColorCyan(0, 255, 255, 255);
const colorb	Q_BColorMagenta(255, 0, 255, 255);
const colorb	Q_BColorWhite(255, 255, 255, 255);

const colorb	Q_BColorLtGrey(191, 191, 191, 255);
const colorb	Q_BColorMdGrey(127, 127, 127, 255);
const colorb	Q_BColorDkGrey(63, 63, 63, 255);

const colorb Q_BStrColorTable[9] =
{
	colorb(0, 0, 0, 255),
	colorb(255, 0, 0, 255),
	colorb(0, 255, 0, 255),
	colorb(255, 255, 0, 255),
	colorb(0, 0, 255, 255),
	colorb(0, 255, 255, 255),
	colorb(255, 0, 255, 255),
	colorb(255, 255, 255, 255),

	colorb(191, 191, 191, 255)
};

/*
===============
Q_ColorCharCount
===============
*/
bool Q_IsColorString (const char *p)
{
	if (!*p || (*p & 127) != COLOR_ESCAPE)
		return false;

	switch (p[1] & 127)
	{
	case '0':	case '1':	case '2':	case '3':	case '4':
	case '5':	case '6':	case '7':	case '8':	case '9':
	case 'i':	case 'I':	// S_STYLE_ITALIC
	case 'r':	case 'R':	// S_STYLE_RETURN
	case 's':	case 'S':	// S_STYLE_SHADOW
	case '^':				// COLOR_ESCAPE
		return true;
	}

	return false;
}


/*
===============
Q_ColorCharCount
===============
*/
sint32 Q_ColorCharCount (const char *s, sint32 endPos)
{
	sint32 count = 0;

	for (const char	*end = s + endPos; *s && s < end; s++)
	{
		if ((s[0] & 127) != COLOR_ESCAPE)
			continue;

		switch (s[1] & 127)
		{
		case '0':	case '1':	case '2':	case '3':	case '4':
		case '5':	case '6':	case '7':	case '8':	case '9':
		case 'i':	case 'I':
		case 'r':	case 'R':
		case 's':	case 'S':
			count += 2;
			break;
		case '^':
			s++;
			count++;
			break;
		}
	}

	return endPos - count;
}


/*
===============
Q_ColorCharOffset
===============
*/
sint32 Q_ColorCharOffset (const char *s, sint32 charCount)
{
	const char	*start = s;

	for (bool skipNext = false; *s && charCount; s++)
	{
		if (skipNext)
			skipNext = false;
		else if (Q_IsColorString (s))
			skipNext = true;
		else
			charCount--;
	}

	return s - start;
}


/*
===============
Q_ColorStrLastColor
===============
*/
sint32 Q_ColorStrLastColor (char *s, sint32 byteOfs)
{
	sint32		lastClrIndex = Q_StrColorIndex (COLOR_WHITE);

	for (char *end = s + (byteOfs - 1); *s && s < end; s++)
	{
		if ((s[0] & 127) != COLOR_ESCAPE)
			continue;

		switch (s[1] & 127)
		{
		case '0':	case '1':	case '2':	case '3':	case '4':
		case '5':	case '6':	case '7':	case '8':	case '9':
			lastClrIndex = (s[1] & 127) - '0';
			break;
		case 'r':	case 'R':
			lastClrIndex = Q_StrColorIndex (COLOR_WHITE);
			break;
		default:
			continue;
		}

		s++;
	}

	return lastClrIndex;
}


/*
===============
Q_ColorStrLastStyle
===============
*/
sint32 Q_ColorStrLastStyle (char *s, sint32 byteOfs)
{
	uint32 lastStyle = 0;
	for (char *end = s + (byteOfs); // don't check last byte
		*s && s < end; s++)
	{
		if ((s[0] & 127) != COLOR_ESCAPE)
			continue;

		switch (s[1] & 127)
		{
		case 'i':	case 'I':
			lastStyle ^= FS_ITALIC;
			break;
		case 's':	case 'S':
			lastStyle ^= FS_SHADOW;
			break;
		case 'r':	case 'R':
			lastStyle = 0;
			break;
		default:
			continue;
		}

		s++;
	}

	return lastStyle;
}

