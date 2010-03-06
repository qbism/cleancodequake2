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

int
irc_connect (struct irc_server *server)
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

  server->status = CONNECTING;

  _irc_register (server);

  return 1;

}

int
irc_receive (struct irc_server *server, int timeout)
{
  int ret;

  ret = _irc_new_data (server, timeout);

  if (ret > 0)
    {
      ret = irc_raw_receive (server);

      _parse_msg (server);

      if (strcmp(server->msg.command_str,"001") == 0)
	server->status = CONNECTED;

      if (server->msg.command == RPL_MYINFO )
	{
	  if (server->server_name)
	    free (server->server_name);

	  if( server->msg.number_of_params >= 1 )
	     server->server_name = strdup ( server->msg.param[1] );
	}
    }

  return ret;

}

void
irc_disconnect (struct irc_server *server, char *msg)
{
  _clear_msg (server);

  irc_raw_send (server, "QUIT :%s", msg);

#ifndef _WIN32
  close (server->sock);
#else
  closesocket (server->sock);
#endif
}

int
irc_pong (struct irc_server *server)
{
  char *pingkey = NULL;

  if (server->msg.raw && !strncmp (server->msg.raw, "PING ", 5))
    {

      pingkey = strchr (server->msg.raw, ':');
      irc_raw_send (server, "PONG %s", ++pingkey);

      return 1;
    }
  else
    irc_raw_send (server, "PONG");

  return 0;
}

int
irc_raw_send (const struct irc_server *server, char *fmt, ...)
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

int
irc_raw_receive (struct irc_server *server)
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
	  server->status = DISCONNECTED;
	  perror ("read()");
	  return -1;
	}

      tmp[i++] = c;

      retval = recv (server->sock, &c_next, 1, MSG_PEEK);

      if (retval == -1)
	{
	  server->received = -1;
	  server->status = DISCONNECTED;
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
	  server->status = DISCONNECTED;
	  return retval;
	}
    }

  if (i > 1)
    {
      server->msg.raw = (char *) malloc (i);

      if( server->msg.raw )
      {
         strncpy (server->msg.raw, tmp, i-1);
         server->msg.raw[i-1] = '\0';
      }
      else
	 perror("malloc()");
    }

  server->received = strlen (server->msg.raw);

  return strlen (server->msg.raw);

}

int
irc_send_cmd (struct irc_server *server, int command, char *fmt, ...)
{
  va_list ap;
  char tmp[BUF_SIZE];

  memset (tmp, '\0', BUF_SIZE);

  va_start (ap, fmt);
  vsnprintf (tmp, BUF_SIZE - 1, fmt, ap);
  va_end (ap);

  return irc_raw_send (server, "%s %s", _int2command (command), tmp);
}

void
irc_ini_server (struct irc_server *server)
{  
  _net_create_sock (server);
  
  server->port = 6667;
  server->status = DISCONNECTED;
  server->received = 0;

  server->msg.from = NONE;
  server->msg.number_of_params = 0;
  server->msg.trailing = 0;
}
