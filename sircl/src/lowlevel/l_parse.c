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

int
_parse_count_params (struct irc_server *server)
{
  int params;
  char *beg;

  // First count the parameters.

  params = 0;
  beg = server->msg.params;

  while (beg)
    {
      if (_parse_istrailing (beg))
	{
	  params++;

	  // last parameter.
	  beg = NULL;
	}
      else if (*(beg) == ' ')
	{
	  params++;

	  beg = strchr (beg + 1, ' ');
	}
    }

  return params;
}

void
_parse_prefix (struct irc_server *server)
{
  char *beg, *end;

  _clear_msg_prefix(server);

  // ':'<prefix> ::=
  //     <servername> | <nick> [ '!' <user> ] [ '@' <host> ]

  if ( server->msg.raw && server->msg.raw[0] == ':' )
    {
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
    }
  else
    server->msg.prefix = NULL;
}

void
_parse_command (struct irc_server *server)
{
  // <command> ::=
  //     <letter> { <letter> } | <number> <number> <number>

  char *beg, *end;

  _clear_msg_command(server);

  if ( server->msg.raw && server->msg.prefix)
    {
      beg = strchr (server->msg.raw, ' ');
      beg++;
    }
  else
     if ( server->msg.raw )
	beg = server->msg.raw;
     else
     {
	beg = NULL;
	server->msg.command_str = NULL;
	server->msg.command = NONE;
     }

  if (beg)
    end = strchr (beg + 1, ' ');

  if (beg && end && (beg < end))
    {
      server->msg.command_str = (char *) malloc ((end - beg) + 1);
      strncpy (server->msg.command_str, beg, (end - beg));
      server->msg.command_str[(end - beg)] = '\0';

      if (_command2int (server->msg.command_str))
	server->msg.command = _command2int (server->msg.command_str);
      else
	server->msg.command = atoi (server->msg.command_str);
    }
}

void
_parse_params (struct irc_server *server)
{
  char *beg, *end;

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

  if (server->msg.raw && server->msg.prefix)
    {
      beg = strchr (server->msg.raw, ' ');
      beg = strchr (beg + 1, ' ');
    }
  else
     if ( server->msg.raw )
	beg = strchr (server->msg.raw, ' ');
     else
	beg = NULL;

  if (beg)
    end = strchr (beg, '\0');

  if (beg && end && (beg < end))
  {
     server->msg.params = strdup (beg);
    
     // Count parameters.
     server->msg.number_of_params = _parse_count_params (server);

     // Parse individual parameters.
     _parse_ind_params (server);

     // Set trailing to 1 or 0.
     server->msg.trailing =
       _parse_istrailing (server->msg.param[server->msg.number_of_params - 1]);
  }
  else
  {
    server->msg.params = NULL;
    server->msg.number_of_params = 0;
    server->msg.trailing = 0;
  }

}

void
_parse_msg (struct irc_server *server)
{
  // <message> ::=
  //    [':' <prefix> <SPACE> ] <command> <params> <crlf>
  // <crlf> ::=
  //    CR LF
  //
  // <SPACE> ::=
  //    ' ' { ' ' }

  if (server->msg.raw)
    {
      _parse_prefix (server);
      _parse_command (server);
      _parse_params (server);
    }
}

void
_parse_ind_params (struct irc_server *server)
{
  char *beg, *end;
  int i;

  server->msg.param = (char **) malloc (server->msg.number_of_params);

  beg = server->msg.params;

  for (i = 0; i < server->msg.number_of_params; i++)
    {
      // beg points to the next param.
      beg = strchr (beg, ' ');

      // Ignore first ' ' char.
      beg++;

      if (beg)
	{
	  // If it's the last param, the end char is '\0' not ' '.
	  if (i == server->msg.number_of_params - 1)
	    {
	      end = strchr (beg, '\0');

	      // If it's <trailing>, ignore ':' first char.
	      if (server->msg.trailing)
		beg++;
	    }
	  else
	    // It isn't the last param, son the end char is ' '.
	    end = strchr (beg, ' ');
	}

      if (beg && end && (beg < end))
	{
	  server->msg.param[i] = (char *) malloc ((end - beg) + 1);
	  strncpy (server->msg.param[i], beg, (end - beg));
	  server->msg.param[i][(end - beg)] = '\0';
	}
    }
}

int
_parse_istrailing (char *string)
{
  if (string)
    if (*(string) == ':')
      return 1;

  if (string && string + 1)
    if (*(string) == ' ' && *(string + 1) == ':')
      return 1;

  return 0;
}
