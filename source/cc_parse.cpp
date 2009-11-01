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
// cc_parse.cpp
// 
//

#include "cc_local.h"

// Error management
void PS_Error (const parse_t *ps, const char *errorMsg, ...)
{
#ifdef _DEBUG
#ifdef WIN32
	OutputDebugString (errorMsg);
#endif
	assert (0);
#endif
}

void PS_Warning (const parse_t *ps, const char *errorMsg, ...)
{
#ifdef _DEBUG
#ifdef WIN32
	OutputDebugString (errorMsg);
#endif
	assert (0);
#endif
}

/*
==============================================================================

	SESSION CREATION
 
==============================================================================
*/

// Session struct
struct parse_s {
	uint32		currentCol;
	uint32		currentLine;
	char		currentToken[MAX_PS_TOKCHARS];

	char		*dataPtr;
	char		*dataPtrLast;
	bool		inUse;

	uint32		numErrors;
	uint32		numWarnings;

	uint32		properties;
};

static parse_t	ps_sessionList[MAX_PARSE_SESSIONS];
static byte		ps_numSessions;

static char		ps_scratchToken[MAX_PS_TOKCHARS];	// Used for temporary storage during data-type/post parsing
static const char *ps_dataTypeStr[] = {
	"char",
	"bool",
	"byte",
	"double",
	"float",
	"int",
	"uint"
};

/*
============
PS_StartSession
============
*/
parse_t *PS_StartSession (char *dataPtr, uint32 properties)
{
	parse_t	*ps = NULL;
	int		i;

	// Make sure incoming data is valid
	if (!dataPtr)
		return NULL;

	// Find an available session
	for (i=0 ; i<ps_numSessions ; i++) {
		if (!ps_sessionList[i].inUse) {
			ps = &ps_sessionList[i];
			break;
		}
	}

	if (i == ps_numSessions) {
		if (ps_numSessions+1 >= MAX_PARSE_SESSIONS)
			PS_Error (NULL, "PS_StartSession: MAX_PARSE_SESSIONS\n");

		ps = &ps_sessionList[ps_numSessions++];
	}

	// Fill in the values
	ps->currentCol = 1;
	ps->currentLine = 1;
	ps->currentToken[0] = '\0';

	ps->dataPtr = dataPtr;
	ps->dataPtrLast = dataPtr;
	ps->inUse = true;

	ps->numErrors = 0;
	ps->numWarnings = 0;

	ps->properties = properties;
	return ps;
}


/*
============
PS_EndSession
============
*/
void PS_EndSession (parse_t *ps)
{
	if (!ps)
		return;

	ps->currentCol = 1;
	ps->currentLine = 1;
	ps->currentToken[0] = '\0';

	ps->dataPtr = NULL;
	ps->dataPtrLast = NULL;
	ps->inUse = false;

	ps->numErrors = 0;
	ps->numWarnings = 0;

	ps->properties = 0;
}

/*
==============================================================================

	ERROR HANDLING
 
==============================================================================
*/

/*
============
PS_AddErrorCount
============
*/
void PS_AddErrorCount (parse_t *ps, uint32 *errors, uint32 *warnings)
{
	if (ps) {
		if (errors)
			*errors += ps->numErrors;
		if (warnings)
			*warnings += ps->numWarnings;
	}
}


/*
============
PS_GetErrorCount
============
*/
void PS_GetErrorCount (parse_t *ps, uint32 *errors, uint32 *warnings)
{
	if (ps) {
		if (errors)
			*errors = ps->numErrors;
		if (warnings)
			*warnings = ps->numWarnings;
	}
}


/*
============
PS_GetPosition
============
*/
void PS_GetPosition (parse_t *ps, uint32 *line, uint32 *col)
{
	if (ps) {
		if (line)
			*line = ps->currentLine;
		if (col)
			*col = ps->currentCol;
	}
}

/*
==============================================================================

	PARSING
 
==============================================================================
*/

/*
============
PS_ParseToken

Sets data to NULL when a '\0' is hit, or when a broken comment is detected.
Returns true only when a comment block was handled.
============
*/
static bool PS_SkipComments (parse_t *ps, char **data, uint32 flags)
{
	char	*p;

	// See if any comment types are allowed
	if (!(ps->properties & PSP_COMMENT_MASK))
		return false;

	p = *data;

	switch (*p) {
	case '#':
		// Skip "# comments"
		if (ps->properties & PSP_COMMENT_POUND) {
			while (*p != '\n') {
				if (*p == '\0') {
					*data = NULL;
					return true;
				}

				p++;
				ps->currentCol++;
			}

			*data = p;
			return true;
		}
		break;

	case '*':
		// This shouldn't happen with proper commenting
		if (p[1] == '/' && ps->properties & PSP_COMMENT_BLOCK) {
			p += 2;
			PS_Error (ps, "PARSE ERROR: end-comment '*/' with no opening\n");
			*data = NULL;
			return true;
		}
		break;

	case '/':
		// Skip "// comments"
		if (p[1] == '/' && ps->properties & PSP_COMMENT_LINE) {
			while (*p != '\n') {
				if (*p == '\0') {
					*data = NULL;
					return true;
				}

				p++;
				ps->currentCol++;
			}

			*data = p;
			return true;
		}

		// Skip "/* comments */"
		if (p[1] == '*' && ps->properties & PSP_COMMENT_BLOCK) {
			// Skip initial "/*"
			p += 2;
			ps->currentCol += 2;

			// Skip everything until "*/"
			while (*p && (*p != '*' || p[1] != '/')) {
				if (*p == '\n') {
					ps->currentCol = 0;
					ps->currentLine++;
				}
				else {
					ps->currentCol++;
				}

				p++;
			}

			// Skip the final "*/"
			if (*p == '*' && p[1] == '/') {
				p += 2;
				ps->currentCol += 2;
				*data = p;
				return true;
			}

			// Didn't find final "*/" (hit EOF)
			PS_Error (ps, "PARSE ERROR: unclosed comment and hit EOF\n");
			*data = NULL;
			return true;
		}
		break;
	}

	// No comment block handled
	return false;
}


/*
============
PS_ConvertEscape
============
*/
static bool PS_ConvertEscape (parse_t *ps, uint32 flags)
{
	uint32	len, i;
	char	*source, *scratch;

	// If it's blank then why even try?
	len = strlen (ps->currentToken);
	if (!len)
		return true;

	// Convert escape characters
	source = &ps->currentToken[0];
	scratch = &ps_scratchToken[0];
	for (i=0 ; i<len ; i++) {
		if (source[0] != '\\') {
			*scratch++ = *source++;
			continue;
		}

		// Hit a '\'
		switch (source[1]) {
		case 'n':
			if (flags & PSF_CONVERT_NEWLINE) {
				*scratch++ = '\n';
				source += 2;
				continue;
			}
			break;

		default:
			PS_Warning (ps, "PARSE WARNING: unknown escape character '%c%c', ignoring\n", source[0], source[1]);
			*scratch++ = *source++;
			*scratch++ = *source++;
			break;
		}
	}
	*scratch = '\0';

	// Copy scratch back to the current token
	strncpy (ps->currentToken, ps_scratchToken, sizeof(ps->currentToken));
	return true;
}


/*
============
PS_ParseToken
- Skip whitespace and skip comments. If a comment gets skipped start over at
  skip whitespace to make sure we're right up at the tail of the next token.
- If it's a quoted string, copy the string into the currentToken until an
  end-quote is hit.
- If it's not a quoted string, store off characters into currentToken until
  a quotation, comment, or blank space is hit.
============
*/
bool PS_ParseToken (parse_t *ps, uint32 flags, char **target)
{
	int		c, len;
	char	*data;

	if (!ps)
		return false;

	// Check if the incoming data offset is valid (see if we hit EOF last the last run)
	data = ps->dataPtr;
	if (!data) {
		PS_Error (ps, "PARSE ERROR: called PS_ParseToken and already hit EOF\n");
		return false;
	}
	ps->dataPtrLast = ps->dataPtr;

	// Clear the current token
	ps->currentToken[0] = '\0';
	len = 0;

	for ( ; ; ) {
		// Skip whitespace
		while ((c = *data) <= ' ') {
			switch (c) {
			case '\0':
				ps->dataPtr = NULL;
				return false;

			case '\n':
				if (!(flags & PSF_ALLOW_NEWLINES)) {
					ps->dataPtr = data;
					if (!ps->currentToken[0])
						return false;

					*target = ps->currentToken;
					return true;
				}

				ps->currentCol = 0;
				ps->currentLine++;
				break;

			default:
				ps->currentCol++;
				break;
			}

			data++;
		}

		// Skip comments
		if (PS_SkipComments (ps, &data, flags)) {
			if (!data) {
				ps->dataPtr = NULL;
				return false;
			}
		}
		else {
			// No comment, don't skip anymore whitespace
			break;
		}
	} 

	// Handle quoted strings specially
	// FIXME: PSP_QUOTES_TOKENED
	if (c == '\"') {
		ps->currentCol++;
		data++;

		for ( ; ; ) {
			c = *data++;
			switch (c) {
			case '\0':
				ps->currentCol++;
				ps->dataPtr = data;
				PS_Error (ps, "PARSE ERROR: hit EOF while inside quotation\n");
				return false;

			case '\"':
				ps->currentCol++;
				ps->dataPtr = data;
				ps->currentToken[len] = '\0';

				// Empty token
				if (!ps->currentToken[0])
					return false;

				// Lower-case if desired
				if (flags & PSF_TO_LOWER)
					Q_strlwr (ps->currentToken);
				if (flags & PSF_CONVERT_NEWLINE) {
					if (!PS_ConvertEscape (ps, flags))
						return false;
				}

				*target = ps->currentToken;
				return true;

			case '\n':
				if (!(flags & PSF_ALLOW_NEWLINES)) {
					ps->dataPtr = data;
					if (!ps->currentToken[0])
						return false;

					*target = ps->currentToken;
					return true;
				}

				ps->currentCol = 0;
				ps->currentLine++;
				break;

			default:
				ps->currentCol++;
				break;
			}

			if (len < MAX_PS_TOKCHARS)
				ps->currentToken[len++] = c;
		}
	}

	// Parse a regular word
	for ( ; ; ) {
		if (c <= ' ' || c == '\"')	// FIXME: PSP_QUOTES_TOKENED
			break;	// Stop at spaces and quotation marks

		// Stop at opening comments
		if (ps->properties & PSP_COMMENT_MASK) {
			if (c == '#' && ps->properties & PSP_COMMENT_POUND)
				break;
			if (c == '/') {
				if (data[1] == '/' && ps->properties & PSP_COMMENT_LINE)
					break;
				if (data[1] == '*' && ps->properties & PSP_COMMENT_BLOCK)
					break;
			}
			if (c == '*' && data[1] == '/' && ps->properties & PSP_COMMENT_BLOCK) {
				ps->dataPtr = data;
				PS_Error (ps, "PARSE ERROR: end-comment '*/' with no opening\n");
				return false;
			}
		}

		// Store character
		if (len < MAX_PS_TOKCHARS)
			ps->currentToken[len++] = c;

		ps->currentCol++;
		c = *++data;
	}

	// Check length
	if (len >= MAX_PS_TOKCHARS-1) {
		PS_Error (ps, "PARSE ERROR: token too long!\n");
		ps->dataPtr = data;
		return false;
	}

	// Done
	ps->currentToken[len] = '\0';
	ps->dataPtr = data;

	// Empty token
	if (!ps->currentToken[0])
		return false;

	// Lower-case if desired
	if (flags & PSF_TO_LOWER)
		Q_strlwr (ps->currentToken);
	if (flags & PSF_CONVERT_NEWLINE) {
		if (!PS_ConvertEscape (ps, flags))
			return false;
	}
	*target = ps->currentToken;
	return true;
}


/*
============
PS_UndoParse

Simply move back to the point parsing was at before the last token parse.
Has issues with datatype parsing and "1" "1" "1" etc token values.
============
*/
void PS_UndoParse (parse_t *ps)
{
	if (!ps)
		return;

	ps->dataPtr = ps->dataPtrLast;
}


/*
============
PS_SkipLine

Unlike other parsing functions in this file, this does not route through
PS_ParseToken simply for the small performance gain.

Simply skip from the current position to the first new-line character.
============
*/
void PS_SkipLine (parse_t *ps)
{
	char	*data;

	if (!ps)
		return;

	// Check if the incoming data offset is valid (see if we hit EOF last the last run)
	data = ps->dataPtr;
	if (!data) {
		PS_Error (ps, "PARSE ERROR: called PS_SkipLine and already hit EOF\n");
		return;
	}
	ps->dataPtrLast = ps->dataPtr;

	// Skip to the end of the line
	while (*data && *data != '\n') {
		data++;
		ps->currentCol++;
	}
	ps->dataPtr = data;
}

/*
==============================================================================

	DATA-TYPE PARSING
 
==============================================================================
*/

/*
============
PS_VerifyCharVec
============
*/
static bool PS_VerifyCharVec (char *token, char *target)
{
	uint32	len, i;
	int		temp;

	len = strlen (token);
	for (i=0 ; i<len ; i++) {
		if (token[i] >= '0' || token[i] <= '9')
			continue;
		if (token[i] == '-' && i == 0)
			continue;
		break;
	}
	if (i != len)
		return false;

	temp = atoi (token);
	if (temp < -128 || temp > 127)
		return false;

	*target = temp;
	return true;
}


/*
============
PS_VerifyBooleanVec
============
*/
static bool PS_VerifyBooleanVec (char *token, bool *target)
{
	if (!strcmp (token, "1") || !strcmp (token, "true")) {
		*target = true;
		return true;
	}
	if (!strcmp (token, "0") || !strcmp (token, "false")) {
		*target = false;
		return true;
	}

	return false;
}


/*
============
PS_VerifyByteVec
============
*/
static bool PS_VerifyByteVec (char *token, byte *target)
{
	uint32	len, i;
	int		temp;

	len = strlen (token);
	for (i=0 ; i<len ; i++) {
		if (token[i] >= '0' || token[i] <= '9')
			continue;
		if (token[i] == '-' && i == 0)
			continue;
		break;
	}
	if (i != len)
		return false;

	temp = atoi (token);
	if (temp < 0 || temp > 255)
		return false;

	*target = temp;
	return true;
}


/*
============
PS_VerifyDoubleVec
============
*/
static bool PS_VerifyDoubleVec (char *token, double *target)
{
	uint32	len, i;
	bool	dot;

	dot = false;
	len = strlen (token);
	for (i=0 ; i<len ; i++) {
		if (token[i] >= '0' || token[i] <= '9')
			continue;
		if (token[i] == '-' && i == 0)
			continue;
		if (token[i] == '.' && !dot) {
			dot = true;
			continue;
		}
		break;
	}
	if (i != len)
		return false;

	*target = atof (token);
	return true;
}


/*
============
PS_VerifyFloatVec
============
*/
static bool PS_VerifyFloatVec (char *token, float *target)
{
	uint32	len, i;
	bool	dot;

	dot = false;
	len = strlen (token);
	for (i=0 ; i<len ; i++) {
		if (token[i] >= '0' || token[i] <= '9')
			continue;
		if (token[i] == '.' && !dot) {
			dot = true;
			continue;
		}
		if (i == 0) {
			if (token[i] == '-')
				continue;
		}
		else if (i == len-1 && (token[i] == 'f' || token[i] == 'F'))
			continue;
		break;
	}
	if (i != len)
		return false;

	*target = (float)atof (token);
	return true;
}


/*
============
PS_VerifyIntegerVec
============
*/
static bool PS_VerifyIntegerVec (char *token, int *target)
{
	uint32	len, i;

	len = strlen (token);
	for (i=0 ; i<len ; i++) {
		if (token[i] >= '0' || token[i] <= '9')
			continue;
		if (token[i] == '-' && i == 0)
			continue;
		break;
	}
	if (i != len)
		return false;

	*target = atoi (token);
	return true;
}


/*
============
PS_VerifyUIntegerVec
============
*/
static bool PS_VerifyUIntegerVec (char *token, uint32 *target)
{
	uint32	len, i;

	len = strlen (token);
	for (i=0 ; i<len ; i++) {
		if (token[i] >= '0' || token[i] <= '9')
			continue;
		break;
	}
	if (i != len)
		return false;

	*target = atoi (token);
	return true;
}


/*
============
PS_ParseDataVector
============
*/
static bool PS_ParseDataVector (char *token, uint32 dataType, void *target)
{
	switch (dataType) {
	case PSDT_CHAR:
		return PS_VerifyCharVec (token, (char *)target);
	case PSDT_BOOLEAN:
		return PS_VerifyBooleanVec (token, (bool *)target);
	case PSDT_BYTE:
		return PS_VerifyByteVec (token, (byte *)target);
	case PSDT_DOUBLE:
		return PS_VerifyDoubleVec (token, (double *)target);
	case PSDT_FLOAT:
		return PS_VerifyFloatVec (token, (float *)target);
	case PSDT_INTEGER:
		return PS_VerifyIntegerVec (token, (int *)target);
	case PSDT_UINTEGER:
		return PS_VerifyUIntegerVec (token, (uint32 *)target);
	}

	return false;
}


/*
============
PS_ParseDataType
============
*/
bool PS_ParseDataType (parse_t *ps, uint32 flags, uint32 dataType, void *target, uint32 numVecs)
{
	char	*token, *data;
	uint32	len, i;
	int		c;

	// Parse the next token
	if (!PS_ParseToken (ps, flags, &token))
		return false;

	// Individual tokens
	// FIXME: support commas and () [] {} brackets
	if (!strchr (token, ' ') && !strchr (token, ',')) {
		for (i=0 ; i<numVecs ; i++) {
			if (i) {
				// Parse the next token
				if (!PS_ParseToken (ps, flags, &token)) {
					PS_Error (ps, "PARSE ERROR: vector missing parameters!\n");
					return false;
				}

				// Storage position
				switch (dataType) {
				case PSDT_CHAR:		target = (char *)target+1;		break;
				case PSDT_BOOLEAN:	target = (bool *)target+1;		break;
				case PSDT_BYTE:		target = (byte *)target+1;		break;
				case PSDT_DOUBLE:	target = (double *)target+1;	break;
				case PSDT_FLOAT:	target = (float *)target+1;		break;
				case PSDT_INTEGER:	target = (int *)target+1;		break;
				case PSDT_UINTEGER:	target = (uint32 *)target+1;	break;
				}
			}

			// Check the data type
			if (!PS_ParseDataVector (token, dataType, target)) {
				PS_Error (ps, "PARSE ERROR: does not evaluate to desired data type!\n");
				return false;
			}
		}

		return true;
	}

	// Single token with all vectors
	// FIXME: support () [] {} brackets
	data = token;
	for (i=0 ; i<numVecs ; i++) {
		ps_scratchToken[0] = '\0';
		len = 0;

		// Skip white-space
		for ( ; ; ) {
			c = *data++;
			if (c <= ' ' || c == ',')
				continue;
			if (c == '\0')
				break;
			break;
		}

		// Parse this token into a sub-token stored in ps_scratchToken
		for ( ; ; ) {
			if (c <= ' ' || c == ',') {
				data--;
				break;	// Stop at white space and commas
			}

			ps_scratchToken[len++] = c;
			c = *data++;
		}
		ps_scratchToken[len++] = '\0';

		// Too few vecs
		if (!ps_scratchToken[0]) {
			PS_Error (ps, "PARSE ERROR: missing vector parameters!\n");
			return false;
		}

		// Check the data type and set the target
		if (!PS_ParseDataVector (ps_scratchToken, dataType, target)) {
			PS_Error (ps, "PARSE ERROR: '%s' does not evaluate to desired data type %s!\n", ps_scratchToken, ps_dataTypeStr[dataType]);
			return false;
		}

		// Next storage position
		switch (dataType) {
		case PSDT_CHAR:		target = (char *)target+1;		break;
		case PSDT_BOOLEAN:	target = (bool *)target+1;		break;
		case PSDT_BYTE:		target = (byte *)target+1;		break;
		case PSDT_DOUBLE:	target = (double *)target+1;	break;
		case PSDT_FLOAT:	target = (float *)target+1;		break;
		case PSDT_INTEGER:	target = (int *)target+1;		break;
		case PSDT_UINTEGER:	target = (uint32 *)target+1;	break;
		}
	}

	// Check for too much data
	for ( ; ; ) {
		c = *data++;
		if (c == '\0')
			break;
		if (c > ' ') {
			PS_Error (ps, "PARSE ERROR: too many vector parameters!\n");
			return false;
		}
	}

	return true;
}
