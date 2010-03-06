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


struct hostent *
_net_resolve (char *server)
{
   return gethostbyname (server);
}

void
_net_setnonblocking (int sock)
{
#ifndef _WIN32
  int opts;

  opts = fcntl (sock, F_GETFL);

  if (opts < 0)
    {
      perror ("fcntl(F_GETFL)");
    }

  opts = (opts | O_NONBLOCK);

  if (fcntl (sock, F_SETFL, opts) < 0)
    {
      perror ("fcntl(F_SETFL)");
    }
#else
	u_long val = 1;
	if (ioctlsocket(sock, FIONBIO, &val) != 0)
		perror ("ioctlsocket(FIONBIO)");
#endif
  return;
}

fd_set
_net_build_select_list (int *sock)
{
  fd_set socks;

  FD_ZERO (&socks);

  FD_SET (*sock, &socks);

  return socks;
}

void
_net_create_sock (irc_server *server)
{
  if ((server->sock = socket (AF_INET, SOCK_STREAM, 0)) == -1)
      exit (-1);
}

