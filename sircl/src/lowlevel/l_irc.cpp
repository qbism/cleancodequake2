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


void _irc_register (const irc_server *server)
{
	irc_raw_send (server, "PASS inetlib");
	irc_raw_send (server, "NICK %s %s", server->nick.c_str(), server->pass.c_str());
	irc_raw_send (server, "USER %s 127.0.0.1 %s :%s", server->user.c_str(),
		server->server.c_str(), server->real_name.c_str());
}

int _irc_new_data (irc_server *server, int t_out)
{
	fd_set socks;
	struct timeval timeout;
	timeout.tv_sec = timeout.tv_usec = 0;

	socks = _net_build_select_list (&(server->sock));

	if (t_out != -1)
		return select ((server->sock) + 1, &socks, (fd_set *) 0, (fd_set *) 0, &timeout);
	else
	{
		timeout.tv_sec = t_out;
		return select ((server->sock) + 1, &socks, (fd_set *) 0, (fd_set *) 0, NULL);
	}
}

