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

#define MASTER "ckroom"
#define NICK "sircl-bot"
#define PASS ""
#define USER "sircl"
#define REAL_NAME "Simple IRC Library http://sircl.sourceforge.net"
#define SERVER "irc.freenode.org"
//#define SERVER "atreides.irc-hispano.org"
#define PORT 6667

// Stores the neccesary information to stablish a connection
// to an irc daemon.
struct irc_server server0;


int main(void)
{

    int retval, do_first, i;

    // Server hostname or ip.
    server0.server = strdup(SERVER);

    // Server port.
    server0.port = PORT;

    // Server nick.
    server0.nick = strdup(NICK);

    // Server pass.
    server0.pass = strdup(PASS);

    // Server user name.
    server0.user = strdup(USER);

    // Server real name.
    server0.real_name = strdup(REAL_NAME);

    // Initializes the struct.
    irc_ini_server(&server0);

    // Then we use irc_connect to stablish the connection.
    irc_connect(&server0);

    do_first = 1;
    
    // main loop.
    while (server0.status != DISCONNECTED) {

	// Blocks until new data arrives, then gets it.
        retval = irc_receive(&server0, -1);

	// Only exec one time and exec when registered.
	if( do_first && server0.status == CONNECTED )
	{
	   // irc_send_cmd(&server0, JOIN, "#linux");
	   do_first = 0;
	}

	// If read characters is > 0 ...
	if (server0.received > 0) 
	{
	   // Prints the raw data received.
	   printf("%s\n", server0.msg.raw);

	   // Prints the prefix.
	   printf("prefix: \"%s\"\n", server0.msg.prefix);
	  
	   // Prints the parameters.
	   printf("params: \"%s\"\n", server0.msg.params);
	  
	   // Prints the parameters.
	   for ( i = 0; i < server0.msg.number_of_params; i++ )
  	       printf("param[%d]: \"%s\"\n", i, server0.msg.param[i]);
	   
	   // If server pings, we pong.
	   if( server0.msg.command == PING ){
	      irc_pong(&server0);
	   }
	}

	// If read characters is < 0, an error occurred or connection is
	// closed.
	if (server0.received < 0)
	{
	   printf("\nReceived %d chars.\n", server0.received);
	   printf("An error occurred.\n");
	}

    }

    // This closes all the file descriptors too.
    irc_disconnect(&server0, "Bye bye. http://sircl.sourceforge.net");

    return 0;

}
