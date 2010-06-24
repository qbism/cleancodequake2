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


const char *_int2command (int command)
{
	switch (command)
	{
		/* Channel operations */

	case IRC::JOIN:
		return "JOIN";
		break;

	case IRC::PART:
		return "PART";
		break;

	case IRC::MODE:
		return "MODE";
		break;

	case IRC::TOPIC:
		return "TOPIC";
		break;

	case IRC::NAMES:
		return "NAMES";
		break;

	case IRC::LIST:
		return "LIST";
		break;

	case IRC::INVITE:
		return "INVITE";
		break;

	case IRC::KICK:
		return "KICK";
		break;

		/* Connection messages */

	case IRC::PASS:
		return "PASS";
		break;

	case IRC::NICK:
		return "NICK";
		break;

	case IRC::USER:
		return "USER";
		break;

	case IRC::SERVER:
		return "SERVER";
		break;

	case IRC::OPER:
		return "OPER";
		break;

	case IRC::QUIT:
		return "QUIT";
		break;

	case IRC::SQUIT:
		return "SQUIT";
		break;

		/* messages */

	case IRC::PRIVMSG:
		return "PRIVMSG";
		break;

	case IRC::NOTICE:
		return "NOTICE";
		break;

		/* Miscellanea messages */

	case IRC::KILL:
		return "KILL";
		break;

	case IRC::PING:
		return "PING";
		break;

	case IRC::PONG:
		return "PONG";
		break;

	case IRC::EERROR:
		return "ERROR";
		break;

		/* Optional messages */

	case IRC::AWAY:
		return "AWAY";
		break;

	case IRC::REHASH:
		return "REHASH";
		break;

	case IRC::RESTART:
		return "RESTART";
		break;

	case IRC::SUMMON:
		return "SUMMON";
		break;

	case IRC::USERS:
		return "USERS";
		break;

	case IRC::WALLOPS:
		return "WALLOPS";
		break;

	case IRC::USERHOST:
		return "USERHOST";
		break;

	case IRC::ISON:
		return "ISON";
		break;

		/* Server queries and commands */

	case IRC::VERSION:
		return "VERSION";
		break;

	case IRC::STATS:
		return "STATS";
		break;

	case IRC::LINKS:
		return "LINKS";
		break;

	case IRC::TIME:
		return "TIME";
		break;

	case IRC::CONNECT:
		return "CONNECT";
		break;

	case IRC::TRACE:
		return "TRACE";
		break;

	case IRC::ADMIN:
		return "ADMIN";
		break;

	case IRC::INFO:
		return "INFO";
		break;

		/* User queries */

	case IRC::WHO:
		return "WHO";
		break;

	case IRC::WHOIS:
		return "WHOIS";
		break;

	case IRC::WHOWAS:
		return "WHOWAS";
		break;

	default:
		return NULL;
		break;

	}
}

int _command2int (const char *command)
{
	if (!command || command[0] == 0)
		return IRC::UNKNOW;

	/* Channel operations */

	if (!strcmp (command, "JOIN"))
		return IRC::JOIN;

	if (!strcmp (command, "PART"))
		return IRC::PART;

	if (!strcmp (command, "MODE"))
		return IRC::MODE;

	if (!strcmp (command, "TOPIC"))
		return IRC::TOPIC;

	if (!strcmp (command, "NAMES"))
		return IRC::NAMES;

	if (!strcmp (command, "INVITE"))
		return IRC::INVITE;

	if (!strcmp (command, "KICK"))
		return IRC::KICK;

	/* Connection messages */

	if (!strcmp (command, "PASS"))
		return IRC::PASS;

	if (!strcmp (command, "NICK"))
		return IRC::NICK;

	if (!strcmp (command, "USER"))
		return IRC::USER;

	if (!strcmp (command, "SERVER"))
		return IRC::SERVER;

	if (!strcmp (command, "OPER"))
		return IRC::OPER;

	if (!strcmp (command, "QUIT"))
		return IRC::QUIT;

	if (!strcmp (command, "SQUIT"))
		return IRC::SQUIT;

	/* messages */

	if (!strcmp (command, "PRIVMSG"))
		return IRC::PRIVMSG;

	if (!strcmp (command, "NOTICE"))
		return IRC::NOTICE;

	/* miscellanea messages */

	if (!strcmp (command, "KILL"))
		return IRC::KILL;

	if (!strcmp (command, "PING"))
		return IRC::PING;

	if (!strcmp (command, "PONG"))
		return IRC::PONG;

	if (!strcmp (command, "ERROR"))
		return IRC::EERROR;

	/* optional messages */

	if (!strcmp (command, "AWAY"))
		return IRC::AWAY;

	if (!strcmp (command, "REHASH"))
		return IRC::REHASH;

	if (!strcmp (command, "RESTART"))
		return IRC::RESTART;

	if (!strcmp (command, "SUMMON"))
		return IRC::SUMMON;

	if (!strcmp (command, "USERS"))
		return IRC::USERS;

	if (!strcmp (command, "WALLOPS"))
		return IRC::WALLOPS;

	if (!strcmp (command, "USERHOST"))
		return IRC::USERHOST;

	if (!strcmp (command, "ISON"))
		return IRC::ISON;

	/* Server queries and commands */

	if (!strcmp (command, "VERSION"))
		return IRC::VERSION;

	if (!strcmp (command, "STATS"))
		return IRC::STATS;

	if (!strcmp (command, "LINKS"))
		return IRC::LINKS;

	if (!strcmp (command, "TIME"))
		return IRC::TIME;

	if (!strcmp (command, "CONNECT"))
		return IRC::CONNECT;

	if (!strcmp (command, "TRACE"))
		return IRC::TRACE;

	if (!strcmp (command, "ADMIN"))
		return IRC::ADMIN;

	if (!strcmp (command, "INFO"))
		return IRC::INFO;

	/* User queries */

	if (!strcmp (command, "WHO"))
		return IRC::WHO;

	if (!strcmp (command, "WHOIS"))
		return IRC::WHOIS;

	if (!strcmp (command, "WHOWAS"))
		return IRC::WHOWAS;

	return IRC::UNKNOW;
}

