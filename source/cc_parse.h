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
// parse.h
//

/*
==============================================================================

	PARSE SESSIONS
 
==============================================================================
*/

#define MAX_PARSE_SESSIONS	256
#define MAX_PS_TOKCHARS		512

// PS_Parse* flags
enum {
	PSF_ALLOW_NEWLINES		= 1,	// Allow token parsing to go onto the next line
	PSF_CONVERT_NEWLINE		= 2,	// Convert newline characters in quoted tokens to their escape character
	PSF_TO_LOWER			= 4,	// Lower-case the token before returning
	PSF_WARNINGS_AS_ERRORS	= 8,	// Treat all warnings as errors
};

// PS_ParseDataType types
enum {
	PSDT_CHAR,
	PSDT_BOOLEAN,
	PSDT_BYTE,
	PSDT_DOUBLE,
	PSDT_FLOAT,
	PSDT_INTEGER,
	PSDT_UINTEGER,
};

// Session properties
enum {
	PSP_COMMENT_BLOCK	= 1,		// Treat "/*" "*/" as block-comment marker
	PSP_COMMENT_LINE	= 2,		// Treat "//" as a line-comment marker
	PSP_COMMENT_POUND	= 4,		// Treat "#" as a line-comment marker
};

#define PSP_COMMENT_MASK	(PSP_COMMENT_BLOCK|PSP_COMMENT_LINE|PSP_COMMENT_POUND)

typedef struct parse_s parse_t;

// Session handling
parse_t		*PS_StartSession (char *dataPtr, uint32 properties);
void		PS_EndSession (parse_t *ps);

// Error handling
void		PS_AddErrorCount (parse_t *ps, uint32 *errors, uint32 *warnings);	// Adds
void		PS_GetErrorCount (parse_t *ps, uint32 *errors, uint32 *warnings);	// Sets
void		PS_GetPosition (parse_t *ps, uint32 *line, uint32 *col);

// Parsers
bool		PS_ParseToken (parse_t *ps, uint32 flags, char **target);
void		PS_UndoParse (parse_t *ps);
void		PS_SkipLine (parse_t *ps);
bool		PS_ParseDataType (parse_t *ps, uint32 flags, uint32 dataType, void *target, uint32 numVecs);

// C++ wrapper
class CParser
{
	parse_t *ps;
public:

	CParser () :
	  ps(NULL)
	  {
	  };

	CParser (char *dataPtr, uint32 properties)
	{
		ps = PS_StartSession (dataPtr, properties);
	};

	~CParser ()
	{
		End ();
	};

	void Start (char *dataPtr, uint32 properties)
	{
		End ();
		ps = PS_StartSession (dataPtr, properties);
	};

	void End ()
	{
		if (ps)
			PS_EndSession (ps);
	};

	void AddErrorCount (uint32 *errors, uint32 *warnings)
	{
		if (ps)
			PS_AddErrorCount (ps, errors, warnings);
	};

	void GetErrorCount (uint32 *errors, uint32 *warnings)
	{
		if (ps)
			PS_GetErrorCount (ps, errors, warnings);
	};

	void GetPosition (uint32 *line, uint32 *col)
	{
		if (ps)
			PS_GetPosition (ps, line, col);
	};

	bool ParseToken (uint32 flags, char **token)
	{
		if (ps)
			return PS_ParseToken (ps, flags, token);
		return true;
	};
	
	void UndoParse ()
	{
		if (ps)
			PS_UndoParse (ps);
	};

	void SkipLine ()
	{
		if (ps)
			PS_SkipLine (ps);
	};

	bool ParseDataType (uint32 flags, uint32 datatype, void *target, uint32 numVecs)
	{
		if (ps)
			return PS_ParseDataType (ps, flags, datatype, target, numVecs);
		return true;
	};
};
