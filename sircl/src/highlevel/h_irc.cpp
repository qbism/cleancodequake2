/* sircl
* Copyright (C) 2004 Francisco Javier Yáñez Fernández.       
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

int irc_connect (irc_server *server)
{
	struct sockaddr_in conexion;
	struct hostent *host;

	host = _net_resolve (server->server);

	if (host == NULL)
	{
		return -2;
	}

	conexion.sin_family = AF_INET;
	conexion.sin_port = htons (server->port);

	conexion.sin_addr = *((struct in_addr *) host->h_addr);
	memset (&(conexion.sin_zero), 0, 8);


	if (connect (server->sock, (struct sockaddr *) &conexion,
		sizeof (struct sockaddr)) == -1)
	{
		return -1;
	}

	server->status = IRC::CONNECTING;

	_irc_register (server);

	return 1;

}

int irc_receive (irc_server *server, int timeout)
{
	int ret;

	ret = _irc_new_data (server, timeout);

	if (ret > 0)
	{
		ret = irc_raw_receive (server);

		_parse_msg (server);

		if (server->msg.command_str == "001")
			server->status = IRC::CONNECTED;

		if (server->msg.command == IRC::RPL_MYINFO )
		{
			server->server_name.clear();

			if( server->msg.number_of_params >= 1 )
				server->server_name = server->msg.param[1];
		}
	}

	return ret;

}

void irc_disconnect (irc_server *server, char *msg)
{
	_clear_msg (server);

	irc_raw_send (server, "QUIT :%s", msg);

#ifndef _WIN32
	close (server->sock);
#else
	closesocket (server->sock);
#endif
}

int irc_pong (irc_server *server)
{
	if (!server->msg.raw.empty() && !strncmp (server->msg.raw.c_str(), "PING ", 5))
	{
		size_t pingkey = server->msg.raw.find_first_of (':');
		irc_raw_send (server, "PONG %s", server->msg.raw.substr(++pingkey).c_str());

		return 1;
	}
	else
		irc_raw_send (server, "PONG");

	return 0;
}

int irc_raw_send (const irc_server *server, char *fmt, ...)
{
	int w;
	va_list ap;
	char tmp[BUF_SIZE];

	memset (tmp, '\0', BUF_SIZE);

	va_start (ap, fmt);
	vsnprintf (tmp, BUF_SIZE - 1, fmt, ap);
	va_end (ap);

	w = strlen (tmp);

	if (w > 510)
		w = 510;

	tmp[w++] = '\r';
	tmp[w++] = '\n';

	return 
#ifndef _WIN32
		write (server->sock, tmp, w);
#else
		send (server->sock, tmp, w, 0);
#endif
}

int irc_raw_receive (irc_server *server)
{
	char c, c_next;
	int i, retval;
	char tmp[BUF_SIZE];

	memset (tmp, '\0', BUF_SIZE);
	i = 0;
	c = '\0';
	c_next = '\0';

	while ((c != '\r' || c_next != '\n') && i < BUF_SIZE)
	{

		retval = 
#ifndef _WIN32
			read (server->sock, &c, 1);
#else
			recv (server->sock, &c, 1, 0);
#endif

		if (retval == 0)
		{
			server->received = -2;
			return -2;
		}
		else if (retval == -1)
		{
			server->received = -1;
			server->status = IRC::DISCONNECTED;
			perror ("read()");
			return -1;
		}

		tmp[i++] = c;

		retval = recv (server->sock, &c_next, 1, MSG_PEEK);

		if (retval == -1)
		{
			server->received = -1;
			server->status = IRC::DISCONNECTED;
			perror ("recv()");
			return -1;
		}
	}

	if (c_next == '\n')
	{
		retval = 
#ifndef _WIN32
			read (server->sock, &c, 1);
#else
			recv (server->sock, &c, 1, 0);
#endif

		if (retval == -1)
		{
			perror ("read()");
			server->status = IRC::DISCONNECTED;
			return retval;
		}
	}

	if (i > 1)
		server->msg.raw = tmp;

	server->received = server->msg.raw.size();
	return server->msg.raw.size();
}

int irc_send_cmd (irc_server *server, int command, char *fmt, ...)
{
	va_list ap;
	char tmp[BUF_SIZE];

	memset (tmp, '\0', BUF_SIZE);

	va_start (ap, fmt);
	vsnprintf (tmp, BUF_SIZE - 1, fmt, ap);
	va_end (ap);

	return irc_raw_send (server, "%s %s", _int2command (command), tmp);
}

void irc_ini_server (irc_server *server)
{  
	_net_create_sock (server);

	server->port = 6667;
	server->status = IRC::DISCONNECTED;
	server->received = 0;

	server->msg.from = IRC::NONE;
	server->msg.number_of_params = 0;
	server->msg.trailing = 0;
}
