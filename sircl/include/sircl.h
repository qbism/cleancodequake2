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
  * @brief Small, compact and optimized IRC Library in C.
  *
  * @file sircl.h
  *
  * @author F. J. Yáñez Fernández.
  * @date 2004
  * @version 0.4.1
  *
  */

#ifndef _sircl_h
#define _sircl_h


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#ifndef _WIN32
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#else
#include <WinSock2.h>
#endif


/* structs */

#include "lowlevel/l_structs.h"
#include "highlevel/structs.h"

/* low-level implementation */

#include "lowlevel/l_commands.h"
#include "lowlevel/l_ctcp.h"
#include "lowlevel/l_dcc.h"
#include "lowlevel/l_irc.h"
#include "lowlevel/l_mem.h"
#include "lowlevel/l_net.h"
#include "lowlevel/l_parse.h"

/* rfc1459 related */

#include "protocol/commands/channel.h"
#include "protocol/commands/connection.h"
#include "protocol/commands/messages.h"
#include "protocol/commands/misc.h"
#include "protocol/commands/optional.h"
#include "protocol/commands/server.h"
#include "protocol/commands/user.h"
#include "protocol/replies/error.h"
#include "protocol/replies/responses.h"

/* other */

#include "protocol/misc.h"


/**
  * @brief Buffer size.
  * The max size of messages from/to server + 1 (+'\\0').
  */
#define BUF_SIZE 513


/**
  * @brief Server status.
  *
  * - DISCONNECTED You are disconnected from the server.
  * - CONNECTING You are connecting to the server.
  * - CONNECTED You connected (and registered) to the server.
  *
  * @pre Use irc_ini_server(&your_server) first!
  */
enum { DISCONNECTED, CONNECTING, CONNECTED };

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************/
//  IRC high-level functions.
/*******************************************************************/

/**
  *
  * @brief Makes a connection to an irc server.
  * @param server Struct which stores the necessary information of the
  * target server.
  * @retval 1 when connected.
  * @retval -2 if the server host couldn't be resolved.
  * @retval -1 if the connection atempt failed.
  *
  */
extern int irc_connect(irc_server *server );

/**
  *
  * @brief Disconnects from a specified irc server, and closes neccesary
  * file descriptors.
  * @param server Struct which stores the necessary information of the
  * target server.
  * @param quit_msg quit message.
  *
  */
extern void irc_disconnect(irc_server *server, char *quit_msg );

/**
  *
  * @brief Receives data from an irc server.
  * @param server Struct which stores the necessary information of the
  * target server.
  * @param timeout Time to block and wait for data. When it's 0, the
  * function returns inmediatly, but if it's -1 the function blocks
  * until new data is received.
  * @return The number of characters read.
  * @retval -1 An I/O error ocurred.
  * @retval -2 The connection is closed.
  *
  */
extern int irc_receive(irc_server *server, int timeout);

/**
  *
  * @brief Check if the server did a ping, and reply if it is necessary.
  * @param server Struct which stores the necessary information of the 
  * target server.
  * @retval 1 The server did a ping and it was replied.
  * @retval 0 No ping from server detected.
  * command.
  *
  */
extern int irc_pong(irc_server *server );



/**
  *
  * @brief Sends data to an irc server.
  * @param server Struct which stores the necessary information of the
  * target server.
  * @param string text string to be sent.
  * @retval -1 if an error ocurred.
  * @return The number of characters sent
  *
  */
extern int irc_raw_send(const irc_server *server, 
                 char *string, ... );

/**
  *
  * @brief Receives data from an irc server.
  * @param server Struct which stores the necessary information of the
  * target server.
  * @return The number of characters read.
  * @retval -1 An I/O error ocurred.
  * @retval -2 The connection is closed.
  *
  */
extern int irc_raw_receive(irc_server *server );

/**
  *
  * @brief Sends a command to an irc server.
  * @param server Struct which stores the necessary information of the
  * target server.
  * @param command Command to be sent.
  * @param parameters Command's parameters.
  * @return Number of characters sent.
  * @retval -1 An error ocurred.
  *
  */
extern int irc_send_cmd(irc_server *server,
                 int command, 
                 char  *parameters, ... );

/**
  *
  * @brief Initializes a irc_server struct.
  * @param server irc_server struct to initialize.
  *
  */
extern void irc_ini_server(irc_server *server);

/*******************************************************************/


/*******************************************************************/
//  IRC high-level commands.
/*******************************************************************/



/*******************************************************************/


/*******************************************************************/
//  ctcp high-level functions.
/*******************************************************************/



/*******************************************************************/


/*******************************************************************/
//  DCC high-level functions.
/*******************************************************************/



/*******************************************************************/

#ifdef __cplusplus
}
#endif

#endif
