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

#ifndef _l_irc_h
#define _l_irc_h


/*******************************************************************/
//  IRC low-level functions.
/*******************************************************************/

void _irc_create_sock(irc_server * );

void _irc_register(const irc_server * );

int _irc_new_data(irc_server *, int );

/*******************************************************************/


#endif

