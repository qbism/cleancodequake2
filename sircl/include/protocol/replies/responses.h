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
        
#ifndef _sircl_responses_h
#define _sircl_responses_h


/* Numeric command responses */


/* RFC2812 Internet Relay Chat: Client Protocol */

enum {

   RPL_WELCOME = 001,
   RPL_YOURHOST = 002,
   RPL_CREATED = 003,
   RPL_MYINFO = 004,
   RPL_BOUNCE = 005

};


/* RFC1459 Internet Relay Chat Protocol */

enum {

   RPL_NONE = 300, 
   RPL_USERHOST = 302,
   RPL_ISON = 303,
   RPL_AWAY = 301,
   RPL_UNAWAY = 305,
   RPL_NOWAWAY = 306,
   RPL_WHOISUSER = 311,
   RPL_WHOISSERVER = 312,
   RPL_WHOISOPERATOR = 313,
   RPL_WHOISIDLE = 317, 
   RPL_ENDOFWHOIS = 318,
   RPL_WHOISCHANNELS = 319,
   RPL_WHOWASUSER = 314,
   RPL_ENDOFWHOWAS = 369,
   RPL_LISTSTART = 321,
   RPL_LIST = 322,
   RPL_LISTEND = 323,
   RPL_CHANNELMODEIS = 324,
   RPL_NOTOPIC = 331,
   RPL_TOPIC = 332,
   RPL_INVITING = 341,
   RPL_SUMMONING = 342,
   RPL_VERSION = 351,
   RPL_WHOREPLY,
   RPL_ENDOFWHO = 315,
   RPL_NAMREPLY = 353,
   RPL_ENDOFNAMES = 366,
   RPL_LINKS = 364,
   RPL_ENDOFLINKS,
   RPL_BANLIST = 367,
   RPL_ENDOFBANLIST,
   RPL_INFO = 371,
   RPL_ENDOFINFO = 374,
   RPL_MOTDSTART = 375,
   RPL_MOTD = 372,
   RPL_ENDOFMOTD = 376,
   RPL_YOUREOPER = 381,
   RPL_REHASHING = 382,
   RPL_TIME = 391,
   RPL_USERSSTART = 392,
   RPL_USERS = 393,
   RPL_ENDOFUSERS = 394,
   RPL_NOUSERS = 395,
   RPL_TRACELINK = 200,
   RPL_TRACECONNECTING = 201,
   RPL_TRACEHANDSHAKE = 202,
   RPL_TRACEUNKNOWN = 203,
   RPL_TRACEOPERATOR = 204,
   RPL_TRACEUSER = 205,
   RPL_TRACESERVER = 206,
   RPL_TRACENEWTYPE = 208,
   RPL_TRACELOG = 261,
   RPL_STATSLINKINFO = 211,
   RPL_STATSCOMMANDS = 212,
   RPL_STATSCLINE = 213,
   RPL_STATSNLINE = 214,
   RPL_STATSILINE = 215,
   RPL_STATSKLINE = 216,
   RPL_STATSYLINE = 218,
   RPL_ENDOFSTATS = 219,
   RPL_STATSLLINE = 241,
   RPL_STATSUPTIME = 242,
   RPL_STATSOLINE = 243,
   RPL_STATSHLINE = 244,
   RPL_UMODEIS = 221,
   RPL_LUSERCLIENT = 251,
   RPL_LUSEROP = 252,
   RPL_LUSERUNKNOWN = 253,
   RPL_LUSERCHANNELS = 254,
   RPL_LUSERME = 255,
   RPL_ADMINME = 256,
   RPL_ADMINLOC1 = 257,
   RPL_ADMINLOC2 = 258,
   RPL_ADMINEMAIL = 259
      
};
   
   

#endif

