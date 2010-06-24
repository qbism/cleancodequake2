/* sircl
* Copyright (C) 2004 Francisco Javier Yáñez Fernndez.       
*                                                                      
* This program is free software; you can redistribute it and/or modify 
* it under the terms of the GNU General Public License as published by 
* the Free Software Foundation; either version 2 of the License, or    
* (at your option) any later version.                                  
*                                                                      
* This program is distributed in the hope that it will be useful,      
* but WITHOUT ANY WARRANTY; without even the implied warranty of       
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        
* GNU General Public License for more details.                         
*                                                                      
* You should have received a copy of the GNU General Public License    
* along with this program; if not, write to the Free Software          
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA                  
*                                                                      
*/

#include "sircl.h"

int _parse_count_params (irc_server *server)
{
	int params;
	size_t beg;

	// First count the parameters.
	params = 0;
	beg = 0;

	while (beg != std::string::npos)
	{
		if (_parse_istrailing (server->msg.params.substr(beg).c_str()))
		{
			params++;

			// last parameter.
			beg = std::string::npos;
		}
		else if (server->msg.params.substr(beg)[0] == ' ')
		{
			params++;
			beg = server->msg.params.find_first_of(' ', beg + 1);
		}
	}

	return params;
}

void _parse_prefix (irc_server *server)
{
	_clear_msg_prefix(server);

	// ':'<prefix> ::=
	//     <servername> | <nick> [ '!' <user> ] [ '@' <host> ]

	if (!server->msg.raw.empty() && server->msg.raw[0] == ':' )
	{
		size_t beg = 1, end;

		if (server->msg.raw[beg])
			end = server->msg.raw.find_first_of(' ', beg);

		if (beg != std::string::npos && end != std::string::npos && (beg < end))
			server->msg.prefix = server->msg.raw.substr (beg, (end - beg));
#if 0
		beg = server->msg.raw;
		beg++;

		if (beg)
			end = strchr (beg, ' ');

		if (beg && end && (beg < end))
		{
			server->msg.prefix = (char *) malloc ((end - beg) + 1);
			strncpy (server->msg.prefix, beg, (end - beg));
			server->msg.prefix[(end - beg)] = '\0';
		}
#endif
	}
	else
		server->msg.prefix.clear();
}

void _parse_command (irc_server *server)
{
	// <command> ::=
	//     <letter> { <letter> } | <number> <number> <number>

	size_t beg, end;

	_clear_msg_command(server);

	if ( !server->msg.raw.empty() && !server->msg.prefix.empty())
		beg = server->msg.raw.find_first_of (' ') + 1;
	else
	{
		if (!server->msg.raw.empty())
			beg = 0;
		else
		{
			beg = std::string::npos;
			server->msg.command_str.clear();
			server->msg.command = IRC::NONE;
		}
	}

	if (beg != std::string::npos)
		end = server->msg.raw.find_first_of (' ', beg + 1);

	if (beg != std::string::npos && end != std::string::npos && (beg < end))
	{
		server->msg.command_str = server->msg.raw.substr(beg, (end - beg));

		if (!(server->msg.command = _command2int (server->msg.command_str.c_str())))
			server->msg.command = atoi (server->msg.command_str.c_str());
	}
}

void _parse_params (irc_server *server)
{
	size_t beg, end;

	_clear_msg_params(server);

	// <params> ::=
	//    <SPACE> [ ':' <trailing> | <middle> <params> ]
	// 
	// <middle> ::=
	//    <Any *non-empty* sequence of octets not including SPACE or NUL
	//    or CR or LF, the first of which may not be ':'>
	//
	// <trailing> ::=
	//    <Any, possibly *empty*, sequence of octets not including NUL or
	//    CR or LF>

	if (!server->msg.raw.empty() && !server->msg.prefix.empty())
		beg = server->msg.raw.find_first_of(' ', server->msg.raw.find_first_of(' ') + 1);
	else if (!server->msg.raw.empty())
		beg = server->msg.raw.find_first_of(' ');
	else
		beg = NULL;

	if (beg != std::string::npos)
		end = server->msg.raw.length();

	if (beg != std::string::npos && end != std::string::npos && (beg < end))
	{
		server->msg.params = server->msg.raw.substr(beg, (beg - end));

		// Count parameters.
		server->msg.number_of_params = _parse_count_params (server);

		// Parse individual parameters.
		_parse_ind_params (server);

		// Set trailing to 1 or 0.
		server->msg.trailing =
			_parse_istrailing (server->msg.param[server->msg.number_of_params - 1].c_str());
	}
	else
	{
		server->msg.params.clear();
		server->msg.number_of_params = 0;
		server->msg.trailing = 0;
	}

}

void _parse_msg (irc_server *server)
{
	// <message> ::=
	//    [':' <prefix> <SPACE> ] <command> <params> <crlf>
	// <crlf> ::=
	//    CR LF
	//
	// <SPACE> ::=
	//    ' ' { ' ' }

	if (!server->msg.raw.empty())
	{
		_parse_prefix (server);
		_parse_command (server);
		_parse_params (server);
	}
}

void _parse_ind_params (irc_server *server)
{
	size_t beg, end;
	int i;

	beg = 0;

	for (i = 0; i < server->msg.number_of_params; i++)
	{
		// beg points to the next param.
		beg = server->msg.params.find_first_of (' ', beg);

		if (beg != std::string::npos)
		{
			// Ignore first ' ' char.
			beg++;

			// If it's the last param, the end char is '\0' not ' '.
			if (i == server->msg.number_of_params - 1)
			{
				end = server->msg.params.length();

				// If it's <trailing>, ignore ':' first char.
				if (server->msg.trailing)
					beg++;
			}
			else
				// It isn't the last param, son the end char is ' '.
				end = server->msg.params.find_first_of(' ', beg);
		}

		if (beg != std::string::npos && end != std::string::npos && (beg < end))
		{
			std::string par = server->msg.params.substr (beg, (end - beg));
			while (par[0] == ' ' || par[0] == 1 || par[0] == 13)
				par.erase(0, 1);
			while (par[par.length()-1] == ' ' || par[par.length()-1] == 1 || par[par.length()-1] == 13)
				par.erase (par.length()-1, 1);
			server->msg.param.push_back (par);
		}
	}
}

int _parse_istrailing (const char *string)
{
	return (string && (*(string) == ':' || (string + 1 && (*(string) == ' ' && *(string + 1) == ':'))));
}
