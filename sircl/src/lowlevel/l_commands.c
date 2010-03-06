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


char *
_int2command (int command)
{
  switch (command)
    {
      /* Channel operations */

    case JOIN:
      return "JOIN";
      break;

    case PART:
      return "PART";
      break;

    case MODE:
      return "MODE";
      break;

    case TOPIC:
      return "TOPIC";
      break;

    case NAMES:
      return "NAMES";
      break;

    case LIST:
      return "LIST";
      break;

    case INVITE:
      return "INVITE";
      break;

    case KICK:
      return "KICK";
      break;

      /* Connection messages */

    case PASS:
      return "PASS";
      break;

    case NICK:
      return "NICK";
      break;

    case USER:
      return "USER";
      break;

    case SERVER:
      return "SERVER";
      break;

    case OPER:
      return "OPER";
      break;

    case QUIT:
      return "QUIT";
      break;

    case SQUIT:
      return "SQUIT";
      break;

      /* messages */

    case PRIVMSG:
      return "PRIVMSG";
      break;

    case NOTICE:
      return "NOTICE";
      break;

      /* Miscellanea messages */

    case IRC_KILL:
      return "KILL";
      break;

    case IRC_PING:
      return "PING";
      break;

    case IRC_PONG:
      return "PONG";
      break;

    case IRC_ERROR:
      return "ERROR";
      break;

      /* Optional messages */

    case AWAY:
      return "AWAY";
      break;

    case REHASH:
      return "REHASH";
      break;

    case RESTART:
      return "RESTART";
      break;

    case SUMMON:
      return "SUMMON";
      break;

    case USERS:
      return "USERS";
      break;

    case WALLOPS:
      return "WALLOPS";
      break;

    case USERHOST:
      return "USERHOST";
      break;

    case ISON:
      return "ISON";
      break;

      /* Server queries and commands */

    case VERSION:
      return "VERSION";
      break;

    case STATS:
      return "STATS";
      break;

    case LINKS:
      return "LINKS";
      break;

    case TIME:
      return "TIME";
      break;

    case CONNECT:
      return "CONNECT";
      break;

    case TRACE:
      return "TRACE";
      break;

    case ADMIN:
      return "ADMIN";
      break;

    case INFO:
      return "INFO";
      break;

      /* User queries */

    case WHO:
      return "WHO";
      break;

    case WHOIS:
      return "WHOIS";
      break;

    case WHOWAS:
      return "WHOWAS";
      break;

    default:
      return NULL;
      break;

    }
}

int
_command2int (char *command)
{
  if (!command)
    return UNKNOW;

  /* Channel operations */

  if (!strcmp (command, "JOIN"))
    return JOIN;

  if (!strcmp (command, "PART"))
    return PART;

  if (!strcmp (command, "MODE"))
    return MODE;

  if (!strcmp (command, "TOPIC"))
    return TOPIC;

  if (!strcmp (command, "NAMES"))
    return NAMES;

  if (!strcmp (command, "INVITE"))
    return INVITE;

  if (!strcmp (command, "KICK"))
    return KICK;

  /* Connection messages */

  if (!strcmp (command, "PASS"))
    return PASS;

  if (!strcmp (command, "NICK"))
    return NICK;

  if (!strcmp (command, "USER"))
    return USER;

  if (!strcmp (command, "SERVER"))
    return SERVER;

  if (!strcmp (command, "OPER"))
    return OPER;

  if (!strcmp (command, "QUIT"))
    return QUIT;

  if (!strcmp (command, "SQUIT"))
    return SQUIT;

  /* messages */

  if (!strcmp (command, "PRIVMSG"))
    return PRIVMSG;

  if (!strcmp (command, "NOTICE"))
    return NOTICE;

  /* miscellanea messages */

  if (!strcmp (command, "KILL"))
    return IRC_KILL;

  if (!strcmp (command, "PING"))
    return IRC_PING;

  if (!strcmp (command, "PONG"))
    return IRC_PONG;

  if (!strcmp (command, "ERROR"))
    return IRC_ERROR;

  /* optional messages */

  if (!strcmp (command, "AWAY"))
    return AWAY;

  if (!strcmp (command, "REHASH"))
    return REHASH;

  if (!strcmp (command, "RESTART"))
    return RESTART;

  if (!strcmp (command, "SUMMON"))
    return SUMMON;

  if (!strcmp (command, "USERS"))
    return USERS;

  if (!strcmp (command, "WALLOPS"))
    return WALLOPS;

  if (!strcmp (command, "USERHOST"))
    return USERHOST;

  if (!strcmp (command, "ISON"))
    return ISON;

  /* Server queries and commands */

  if (!strcmp (command, "VERSION"))
    return VERSION;

  if (!strcmp (command, "STATS"))
    return STATS;

  if (!strcmp (command, "LINKS"))
    return LINKS;

  if (!strcmp (command, "TIME"))
    return TIME;

  if (!strcmp (command, "CONNECT"))
    return CONNECT;

  if (!strcmp (command, "TRACE"))
    return TRACE;

  if (!strcmp (command, "ADMIN"))
    return ADMIN;

  if (!strcmp (command, "INFO"))
    return INFO;

  /* User queries */

  if (!strcmp (command, "WHO"))
    return WHO;

  if (!strcmp (command, "WHOIS"))
    return WHOIS;

  if (!strcmp (command, "WHOWAS"))
    return WHOWAS;

  return UNKNOW;
}

