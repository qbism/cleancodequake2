/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
This source file is contained as part of CleanCode Quake2, a project maintained
by Paril, to 'clean up' and make Quake2 an easier source base to read and work with.

You may use any part of this code to help create your own bases and own mods off
this code if you wish. It is under the same license as Quake 2 source (as above),
therefore you are free to have to fun with it. All I ask is you email me so I can
list the mod on my page for CleanCode Quake2 to help get the word around. Thanks.
*/

//
// cc_irc.cpp
// 
//

#include "cc_local.h"

bool CIRCClient::Connected ()
{
	return (IRCServer.status != DISCONNECTED);
};

CIRCClient::CIRCClient (std::string HostName, std::string Nick, std::string User, std::string Pass, std::string RealName, int port)
{
	IRCServer.port = port;
	IRCServer.server = Mem_StrDup(HostName.c_str());
	IRCServer.nick = Mem_StrDup(Nick.c_str());
	IRCServer.user = Mem_StrDup(User.c_str());
	IRCServer.pass = Mem_StrDup(Pass.c_str());
	IRCServer.real_name = Mem_StrDup(RealName.c_str());
	IRCServer.server_name = "Q2IRC";

    irc_ini_server(&IRCServer);
};

CIRCClient::~CIRCClient ()
{
	irc_disconnect (&IRCServer, "Quit");

	QDelete IRCServer.server;
	QDelete IRCServer.nick;
	QDelete IRCServer.user;
	QDelete IRCServer.pass;
	QDelete IRCServer.real_name;
};

void CIRCClient::Connect ()
{
	if (Connected ())
		return;

	irc_connect (&IRCServer);
	_net_setnonblocking (IRCServer.sock);
};

// One update frame
void CIRCClient::Update ()
{
	irc_receive(&IRCServer, 0);

	// If read characters is > 0 ...
	if (IRCServer.received > 0)
	{
		// If server pings, we pong.
		if (IRCServer.msg.command == IRC_PING)
			irc_pong(&IRCServer);

		if (IRCServer.msg.command == PRIVMSG)
		{
			// Get name
			std::string otherName = IRCServer.msg.prefix;
			otherName = otherName.substr(0, otherName.find_first_of('!'));

			std::string message = IRCServer.msg.params;
			message = message.substr (message.find_first_of(':') + 1);

			Player->PrintToClient (PRINT_CHAT, "[IRC] <%s> %s\n", otherName.c_str(), message.c_str());
		}
		else
			Player->PrintToClient (PRINT_HIGH, "[IRC] %s\n", IRCServer.msg.raw);
	}

	// clear message
	IRCServer.received = 0;
};

void CIRCClient::SendMessage (std::string Message)
{
	irc_send_cmd(&IRCServer, PRIVMSG, "#tastycast :%s", Message.c_str());
	Player->PrintToClient (PRINT_CHAT, "[IRC] <%s> %s\n", IRCServer.nick, Message.c_str());
};

void CIRCClient::JoinChannel (std::string ChannelName)
{
	irc_send_cmd(&IRCServer, JOIN, "%s", ChannelName.c_str());
};