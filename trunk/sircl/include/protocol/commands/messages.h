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

#ifndef _sircl_messages_h
#define _sircl_messages_h

// IRC messages by standard

enum
{
	// connection messages (1000-1999)
	CMD_CONNECTION = 1000,
	PASS,
	NICK,
	USER,
	SERVER,
	OPER,
	QUIT,
	SQUIT,

	// Queries and commands (3000-3999)
	CMD_SERVER = 3000,
	VERSION,
	STATS,
	LINKS,
	TIME,
	CONNECT,
	TRACE,
	ADMIN,
	INFO,
    
	// channel operations (2000-2999)
	CMD_CHANNEL = 2000,
	JOIN,
	PART,
	MODE,
	TOPIC,
	NAMES,
	LIST,
	INVITE,
	KICK,

	// messages (4000-4999)
	CMD_MESSAGES = 4000,
	PRIVMSG,
	NOTICE,

	// User based queries (5000-5999)
	CMD_USER = 5000,
	WHO,
	WHOIS,
	WHOWAS,

	// miscellaneous messages (6000-6999)
	CMD_MISC = 6000,
	KILL,
	PING,
	PONG,
	EERROR,

	// Optionals messages (7000-7999)
	CMD_OPTIONALS = 7000,
	AWAY,
	REHASH,
	RESTART,
	SUMMON,
	USERS,
	WALLOPS,
	USERHOST,
	ISON,
};


#endif

