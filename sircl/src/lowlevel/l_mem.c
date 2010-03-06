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


void
_clear_msg (struct irc_server *server)
{
   _clear_msg_raw(server);
   _clear_msg_prefix(server);
   _clear_msg_command(server);
   _clear_msg_params(server);
}

void
_clear_msg_raw (struct irc_server *server)
{
  if (server->msg.raw)
    {
      free (server->msg.raw);
      server->msg.raw = NULL;
    }
}

void
_clear_msg_prefix (struct irc_server *server)
{
  if (server->msg.prefix)
    {
      free (server->msg.prefix);
      server->msg.prefix = NULL;
    }
}

void
_clear_msg_command (struct irc_server *server)
{
  if (server->msg.command_str)
  {
    free (server->msg.command_str);
    server->msg.command_str = NULL;
  }

  server->msg.command = NONE;
}

// BUGGY BUUGY BUGYY
void
_clear_msg_params (struct irc_server *server)
{
  int i;
  printf ("START %d\n", server->msg.number_of_params);
  if( server->msg.number_of_params == 0 )
	  return;

  for (i = 0; i < server->msg.number_of_params; i++)
    {
      if ( (server->msg).param[i] )
	{
	  //free ( server->msg.param[i] );
	}
    }

  if (server->msg.param)
    {
      //free ((server->msg).param);
    }
  
  if (server->msg.params)
    {
      //free (server->msg.params);
    }

  server->msg.number_of_params = 0;
  server->msg.trailing = 0;

  printf ("EXIT %d\n", server->msg.number_of_params);
}
