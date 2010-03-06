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

/**
  *
  * @file structs.h
  *
  * @brief Basic structures to be used by end users.
  *
  * @author F. J. Yáñez Fernández.
  * @version 0.4
  * @date 2004
  *
  */

#ifndef _sircl_structs_h
#define _sircl_structs_h


/*******************************************************************/
//  IRC high-level structs.
/*******************************************************************/


/**
  *
  * @brief Struct which stores information about an irc server.
  *
  */
struct irc_server 
{
    //! Connection's socket.
    int sock;

    //! Server hostname or ip.
    char *server;

    //! Connected server name.
    char *server_name;

    //! Server port.
    int port;

    //! Desired nick to connect with.
    char *nick;

    //! Desired pass to connect with.
    char *pass;

    //! Desired user to connect with.
    char *user;

    //! Desired user name to connect with.
    char *real_name;

    //! Where the parsed data will be stored.
    struct _irc_message msg;

    //! Can be CONNECTED, DISCONNECTED, CONNECTING.
    int status;
    
    //! Stores the last returned value from irc_receive() or
    //irc_raw_received();
    int received;
    
};


#endif
