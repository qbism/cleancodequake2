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

#ifndef _l_parse_h
#define _l_parse_h


/*******************************************************************/
//  Data-parsing low-level functions.
/*******************************************************************/

int _parse_istrailing (const char *string);
int _parse_count_params (irc_server *server);
void _parse_prefix (irc_server *server);
void _parse_command (irc_server *server);
void _parse_params (irc_server *server);
void _parse_msg (irc_server *server);
void _parse_ind_params (irc_server *server);

/*******************************************************************/


#endif

