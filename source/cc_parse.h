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

// Session properties
enum {
	PSP_COMMENT_BLOCK	= 1,		// Treat "/*" "*/" as block-comment marker
	PSP_COMMENT_LINE	= 2,		// Treat "//" as a line-comment marker
	PSP_COMMENT_POUND	= 4,		// Treat "#" as a line-comment marker
};

#define PSP_COMMENT_MASK	(PSP_COMMENT_BLOCK|PSP_COMMENT_LINE|PSP_COMMENT_POUND)

#include <climits>

class CParser
{
	std::cc_string		ps_scratchToken;	// Used for temporary storage during data-type/post parsing

public:
	class CParseSession
	{
	public:
		CParseSession () :
		  currentCol(1),
		  currentLine(1),
		  dataPtr(NULL),
		  dataPtrLast(NULL),
		  numErrors(0),
		  numWarnings(0),
		  properties(0)
		{
			currentToken[0] = '\0';
		};

		CParseSession (const char *dataPtr, uint32 properties) :
		  currentCol(1),
		  currentLine(1),
		  dataPtr(dataPtr),
		  dataPtrLast(dataPtr),
		  numErrors(0),
		  numWarnings(0),
		  properties(properties)
		{
			// Make sure incoming data is valid
			if (!dataPtr)
				return;

			currentToken[0] = '\0';
		};

		inline uint32 GetErrorCount ()
		{
			return numErrors;
		}

		inline uint32 GetWarningCount ()
		{
			return numWarnings;
		}

		inline uint32 GetLine ()
		{
			return currentLine;
		}
		
		inline uint32 GetColumn ()
		{
			return currentCol;
		}

		friend class CParser;

	private:
		uint32		currentCol;
		uint32		currentLine;
		char		currentToken[MAX_PS_TOKCHARS];

		const char		*dataPtr;
		const char		*dataPtrLast;

		uint32		numErrors;
		uint32		numWarnings;

		uint32		properties;
	} Session;

	CParser () :
	  ps_scratchToken(),
	  Session()
	  {
	  };

	CParser (const char *dataPtr, uint32 properties) :
	  ps_scratchToken(),
	  Session(dataPtr, properties)
	{
	};

	~CParser ()
	{
	};

	void Start (const char *dataPtr, uint32 properties)
	{
		Session = CParseSession (dataPtr, properties);
	};

	bool ParseToken (uint32 flags, char **target)
	{
		// Check if the incoming data offset is valid (see if we hit EOF last the last run)
		const char *data = Session.dataPtr;
		if (!data)
		{
			AddError ("PARSE ERROR: called PS_ParseToken and already hit EOF\n");
			return false;
		}
		Session.dataPtrLast = Session.dataPtr;

		// Clear the current token
		Session.currentToken[0] = '\0';
		int len = 0, c = 0;

		while (true)
		{
			// Skip whitespace
			while ((c = *data) <= ' ')
			{
				switch (c)
				{
				case '\0':
					Session.dataPtr = NULL;
					return false;

				case '\n':
					if (!(flags & PSF_ALLOW_NEWLINES))
					{
						Session.dataPtr = data;
						if (!Session.currentToken[0])
							return false;

						*target = Session.currentToken;
						return true;
					}

					Session.currentCol = 0;
					Session.currentLine++;
					break;

				default:
					Session.currentCol++;
					break;
				}

				data++;
			}

			// Skip comments
			if (SkipComments (&data, flags))
			{
				if (!data)
				{
					Session.dataPtr = NULL;
					return false;
				}
			}
			else
				// No comment, don't skip anymore whitespace
				break;
		} 

		// Handle quoted strings specially
		// FIXME: PSP_QUOTES_TOKENED
		if (c == '\"')
		{
			Session.currentCol++;
			data++;

			while (true)
			{
				c = *data++;
				switch (c)
				{
				case '\0':
					Session.currentCol++;
					Session.dataPtr = data;
					AddError ("PARSE ERROR: hit EOF while inside quotation\n");
					return false;

				case '\"':
					Session.currentCol++;
					Session.dataPtr = data;
					Session.currentToken[len] = '\0';

					// Empty token
					if (!Session.currentToken[0])
						return false;

					// Lower-case if desired
					if (flags & PSF_TO_LOWER)
						Q_strlwr (Session.currentToken);

					if (flags & PSF_CONVERT_NEWLINE)
					{
						if (!ConvertEscape (flags))
							return false;
					}

					*target = Session.currentToken;
					return true;

				case '\n':
					if (!(flags & PSF_ALLOW_NEWLINES))
					{
						Session.dataPtr = data;
						if (!Session.currentToken[0])
							return false;

						*target = Session.currentToken;
						return true;
					}

					Session.currentCol = 0;
					Session.currentLine++;
					break;

				default:
					Session.currentCol++;
					break;
				}

				if (len < MAX_PS_TOKCHARS)
					Session.currentToken[len++] = c;
			}
		}

		// Parse a regular word
		while (true)
		{
			if (c <= ' ' || c == '\"')	// FIXME: PSP_QUOTES_TOKENED
				break;	// Stop at spaces and quotation marks

			// Stop at opening comments
			if (Session.properties & PSP_COMMENT_MASK)
			{
				if (c == '#' && Session.properties & PSP_COMMENT_POUND)
					break;
				if (c == '/')
				{
					if (data[1] == '/' && Session.properties & PSP_COMMENT_LINE)
						break;
					if (data[1] == '*' && Session.properties & PSP_COMMENT_BLOCK)
						break;
				}

				if (c == '*' && data[1] == '/' && Session.properties & PSP_COMMENT_BLOCK)
				{
					Session.dataPtr = data;
					AddError ("PARSE ERROR: end-comment '*/' with no opening\n");
					return false;
				}
			}

			// Store character
			if (len < MAX_PS_TOKCHARS)
				Session.currentToken[len++] = c;

			Session.currentCol++;
			c = *++data;
		}

		// Check length
		if (len >= MAX_PS_TOKCHARS-1)
		{
			AddError ("PARSE ERROR: token too long!\n");
			Session.dataPtr = data;

			return false;
		}

		// Done
		Session.currentToken[len] = '\0';
		Session.dataPtr = data;

		// Empty token
		if (!Session.currentToken[0])
			return false;

		// Lower-case if desired
		if (flags & PSF_TO_LOWER)
			Q_strlwr (Session.currentToken);
		if (flags & PSF_CONVERT_NEWLINE)
		{
			if (!ConvertEscape (flags))
				return false;
		}

		*target = Session.currentToken;
		return true;
	};
	
	void UndoParse ()
	{
		Session.dataPtr = Session.dataPtrLast;
	};

	void SkipLine ()
	{
		// Check if the incoming data offset is valid (see if we hit EOF last the last run)
		const char	*data = Session.dataPtr;

		if (!data)
		{
			AddError ("PARSE ERROR: called PS_SkipLine and already hit EOF\n");
			return;
		}

		Session.dataPtrLast = Session.dataPtr;

		// Skip to the end of the line
		while (*data && *data != '\n')
		{
			data++;
			Session.currentCol++;
		}

		Session.dataPtr = data;
	};

	template <typename TType>
	bool ParseDataType (uint32 flags, void *target, uint32 numVecs)
	{
		char *token;
		// Parse the next token
		if (!ParseToken (flags, &token))
			return false;

		// Individual tokens
		// FIXME: support commas and () [] {} brackets
		if (!strchr (token, ' ') && !strchr (token, ','))
		{
			for (uint32 i = 0; i < numVecs; i++)
			{
				if (i)
				{
					// Parse the next token
					if (!ParseToken (flags, &token))
					{
						AddError ("PARSE ERROR: vector missing parameters!\n");
						return false;
					}

					// Storage position
					target = (TType *)target+1;
				}

				// Check the data type
				if (!PS_VerifyVec<TType> (token, target))
				{
					AddError ("PARSE ERROR: does not evaluate to desired data type!\n");
					return false;
				}
			}

			return true;
		}

		// Single token with all vectors
		// FIXME: support () [] {} brackets
		char *data = token;

		for (uint32 i = 0; i < numVecs; i++)
		{
			char c;

			ps_scratchToken.clear ();
			size_t len = 0;

			// Skip white-space
			while (true)
			{
				c = *data++;

				if (c <= ' ' || c == ',')
					continue;
				
				if (c == '\0')
					break;
				
				break;
			}

			// Parse this token into a sub-token stored in ps_scratchToken
			while (true)
			{
				if (c <= ' ' || c == ',')
				{
					data--;
					break;	// Stop at white space and commas
				}

				ps_scratchToken.push_back (c);
				len++;
				c = *data++;
			}

			len++;

			// Too few vecs
			if (ps_scratchToken.empty())
			{
				AddError ("PARSE ERROR: missing vector parameters!\n");
				return false;
			}

			// Check the data type and set the target
			if (!PS_VerifyVec<TType> (ps_scratchToken.c_str(), target))
			{
				AddError ("PARSE ERROR: '%s' does not evaluate to desired data type %s!\n", ps_scratchToken.c_str(), PS_DataName<TType> ());
				return false;
			}

			// Next storage position
			target = (TType *)target+1;
		}

		// Check for too much data
		while (true)
		{
			char c = *data++;

			if (c == '\0')
				break;

			if (c > ' ')
			{
				AddError ("PARSE ERROR: too many vector parameters!\n");
				return false;
			}
		}

		return true;
	}

	// Error management
	void AddError (const char *errorMsg, ...)
	{
	#ifdef _DEBUG
	#ifdef WIN32
		OutputDebugString (errorMsg);
	#endif
		assert (0);
	#endif
	}

	void AddWarning (const char *errorMsg, ...)
	{
	#ifdef _DEBUG
	#ifdef WIN32
		OutputDebugString (errorMsg);
	#endif
		assert (0);
	#endif
	}

	// Private interface
private:

	template <typename TType>
	static bool PS_VerifyVec (const char *token, void *target)
	{
		_CC_ASSERT_EXPR (0, "PS_VerifyVec called without type specialization");
		return false;
	}

	// Parse integral value
	template <typename TType>
	static bool PS_VerifyVecI (const char *token, void *target, const TType MinValue, const TType MaxValue)
	{
		uint32 i, 
			len = strlen (token);

		for (i = 0; i < len; i++)
		{
			if (token[i] >= '0' || token[i] <= '9')
				continue;
			if (token[i] == '-' && i == 0)
				continue;
			break;
		}

		if (i != len)
			return false;

		TType temp = atol (token);
		if ((MinValue && (temp < MinValue)) || temp > MaxValue)
			return false;

		*(TType*)target = (TType)temp;
		return true;
	}

	template <>
	static bool PS_VerifyVec <sint8> (const char *token, void *target)
	{
		return PS_VerifyVecI <sint8> (token, target, SCHAR_MIN, SCHAR_MAX);
	}

	template <>
	static bool PS_VerifyVec <uint8> (const char *token, void *target)
	{
		return PS_VerifyVecI <uint8> (token, target, 0, CHAR_MAX);
	}

	template <>
	static bool PS_VerifyVec <sint16> (const char *token, void *target)
	{
		return PS_VerifyVecI <sint16> (token, target, SHRT_MIN, SHRT_MAX);
	}

	template <>
	static bool PS_VerifyVec <uint16> (const char *token, void *target)
	{
		return PS_VerifyVecI <uint16> (token, target, 0, USHRT_MAX);
	}

	template <>
	static bool PS_VerifyVec <sint32> (const char *token, void *target)
	{
		return PS_VerifyVecI <sint32> (token, target, INT_MIN, INT_MAX);
	}

	template <>
	static bool PS_VerifyVec <uint32> (const char *token, void *target)
	{
		return PS_VerifyVecI <uint32> (token, target, 0, UINT_MAX);
	}

	#ifndef _I64_MIN
	/* minimum signed 64 bit value */
	#define _I64_MIN    (-9223372036854775807i64 - 1)
	/* maximum signed 64 bit value */
	#define _I64_MAX      9223372036854775807i64
	/* maximum unsigned 64 bit value */
	#define _UI64_MAX     0xffffffffffffffffui64
	#endif

	template <>
	static bool PS_VerifyVec <long> (const char *token, void *target)
	{
		return PS_VerifyVecI <long> (token, target, LONG_MIN, LONG_MAX);
	}

	template <>
	static bool PS_VerifyVec <unsigned long> (const char *token, void *target)
	{
		return PS_VerifyVecI <unsigned long> (token, target, 0, ULONG_MAX);
	}

	template <>
	static bool PS_VerifyVec <sint64> (const char *token, void *target)
	{
		return PS_VerifyVecI <sint64> (token, target, _I64_MIN, _I64_MAX);
	}

	template <>
	static bool PS_VerifyVec <uint64> (const char *token, void *target)
	{
		return PS_VerifyVecI <uint64> (token, target, 0, _UI64_MAX);
	}

	// Parse float
	template <typename TType>
	static bool PS_VerifyVecF (const char *token, void *target)
	{
		bool		dot = false;
		uint32		i, len = strlen (token);

		for (i = 0; i < len; i++)
		{
			if (token[i] >= '0' || token[i] <= '9')
				continue;
			
			if (token[i] == '.' && !dot)
			{
				dot = true;
				continue;
			}
			
			if (i == 0)
			{
				if (token[i] == '-')
					continue;
			}

			else if (i == (len-1) && (Q_tolower(token[i]) == 'f' || Q_tolower(token[i]) == 'd'))
				continue;

			break;
		}

		if (i != len)
			return false;

		*(TType*)target = (TType)atof (token);
		return true;
	}

	template <>
	static bool PS_VerifyVec <float> (const char *token, void *target)
	{
		return PS_VerifyVecF <float> (token, target);
	}

	template <>
	static bool PS_VerifyVec <double> (const char *token, void *target)
	{
		return PS_VerifyVecF <double> (token, target);
	}

	template <>
	static bool PS_VerifyVec <bool> (const char *token, void *target)
	{
		if (!strcmp (token, "1") || !Q_stricmp (token, "true"))
		{
			*(bool*)target = true;
			return true;
		}

		if (!strcmp (token, "0") || !Q_stricmp (token, "false"))
		{
			*(bool*)target = false;
			return true;
		}

		return false;
	}

	template <typename TType> static const char *PS_DataName () { return "<unknown>"; }
	template <> static const char *PS_DataName <sint8> () { return "<sint8>"; }
	template <> static const char *PS_DataName <uint8> () { return "<uint8>"; }
	template <> static const char *PS_DataName <sint16> () { return "<sint16>"; }
	template <> static const char *PS_DataName <uint16> () { return "<uint16>"; }
	template <> static const char *PS_DataName <sint32> () { return "<sint32>"; }
	template <> static const char *PS_DataName <uint32> () { return "<uint32>"; }
	template <> static const char *PS_DataName <long> () { return "<long>"; }
	template <> static const char *PS_DataName <unsigned long> () { return "<unsigned long>"; }
	template <> static const char *PS_DataName <sint64> () { return "<sint64>"; }
	template <> static const char *PS_DataName <uint64> () { return "<uint64>"; }
	template <> static const char *PS_DataName <float> () { return "<float>"; }
	template <> static const char *PS_DataName <double> () { return "<double>"; }
	template <> static const char *PS_DataName <bool> () { return "<bool>"; }

	bool SkipComments (const char **data, uint32 flags)
	{
		// See if any comment types are allowed
		if (!(Session.properties & PSP_COMMENT_MASK))
			return false;

		const char *p = *data;

		switch (*p)
		{
		case '#':
			// Skip "# comments"
			if (Session.properties & PSP_COMMENT_POUND)
			{
				while (*p != '\n')
				{
					if (*p == '\0')
					{
						*data = NULL;
						return true;
					}

					p++;
					Session.currentCol++;
				}

				*data = p;
				return true;
			}
			break;

		case '*':
			// This shouldn't happen with proper commenting
			if (p[1] == '/' && (Session.properties & PSP_COMMENT_BLOCK))
			{
				p += 2;
				AddError ("PARSE ERROR: end-comment '*/' with no opening\n");
				*data = NULL;
				return true;
			}
			break;

		case '/':
			// Skip "// comments"
			if (p[1] == '/' && (Session.properties & PSP_COMMENT_LINE))
			{
				while (*p != '\n')
				{
					if (*p == '\0')
					{
						*data = NULL;
						return true;
					}

					p++;
					Session.currentCol++;
				}

				*data = p;
				return true;
			}

			// Skip "/* comments */"
			if (p[1] == '*' && (Session.properties & PSP_COMMENT_BLOCK))
			{
				// Skip initial "/*"
				p += 2;
				Session.currentCol += 2;

				// Skip everything until "*/"
				while (*p && (*p != '*' || p[1] != '/'))
				{
					if (*p == '\n')
					{
						Session.currentCol = 0;
						Session.currentLine++;
					}
					else
						Session.currentCol++;

					p++;
				}

				// Skip the final "*/"
				if (*p == '*' && p[1] == '/')
				{
					p += 2;
					Session.currentCol += 2;
					*data = p;
					return true;
				}

				// Didn't find final "*/" (hit EOF)
				AddError ("PARSE ERROR: unclosed comment and hit EOF\n");
				*data = NULL;
				return true;
			}
			break;
		}

		// No comment block handled
		return false;
	}

	bool ConvertEscape (uint32 flags)
	{
		// If it's blank then why even try?
		uint32 len = strlen (Session.currentToken);
		if (!len)
			return true;

		// Convert escape characters
		char *source = &Session.currentToken[0];

		for (uint32 i = 0; i < len; i++)
		{
			if (source[0] != '\\')
			{
				ps_scratchToken.push_back (*source++);
				continue;
			}

			// Hit a '\'
			switch (source[1])
			{
			case 'n':
				if (flags & PSF_CONVERT_NEWLINE)
				{
					ps_scratchToken.push_back ('\n');
					source += 2;
					continue;
				}
				break;

			default:
				AddWarning ("PARSE WARNING: unknown escape character '%c%c', ignoring\n", source[0], source[1]);
				ps_scratchToken.push_back (*source++);
				ps_scratchToken.push_back (*source++);
				break;
			}
		}

		// Copy scratch back to the current token
		strncpy (Session.currentToken, ps_scratchToken.c_str(), sizeof(Session.currentToken));
		return true;
	}
};
