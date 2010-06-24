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

#if CLEANCODE_IRC

CIRCClientServerChannel *CIRCClientServer::FindChannel (std::string ChannelName)
{
	for (TIRCChannels::iterator it = Channels.begin(); it != Channels.end(); ++it)
	{
		CIRCClientServerChannel &Channel = (*it);

		if (Channel.ChannelName == ChannelName)
			return &Channel;
	}
	return NULL;
};

void CIRCClientServerChannel::Join (CPlayerEntity *Player)
{
	Players.push_back (Player);

	Server->PushMessage(IRC::PRIVMSG, FormatString("%s :%s has joined channel %s", ChannelName.c_str(), Player->Client.Respawn.IRC.Nick.c_str(), ChannelName.c_str()));
	for (TConnectedIRCPlayers::iterator it = Players.begin(); it != Players.end(); ++it)
		(*it)->PrintToClient (PRINT_CHAT, "[IRC] %s has joined channel %s\n", Player->Client.Respawn.IRC.Nick.c_str(), ChannelName.c_str());
	
	Player->Client.Respawn.IRC.Channel = ChannelName;
};

void CIRCClientServerChannel::Leave (CPlayerEntity *Player)
{
	Server->PushMessage(IRC::PRIVMSG, FormatString("%s :%s has left channel %s", ChannelName.c_str(), Player->Client.Respawn.IRC.Nick.c_str(), ChannelName.c_str()));
	for (TConnectedIRCPlayers::iterator it = Players.begin(); it != Players.end(); ++it)
		(*it)->PrintToClient (PRINT_CHAT, "[IRC] %s has left channel %s\n", Player->Client.Respawn.IRC.Nick.c_str(), ChannelName.c_str());

	for (TConnectedIRCPlayers::iterator it = Players.begin(); it != Players.end(); ++it)
	{
		if ((*it) == Player)
		{
			Players.erase (it);
			break;
		}
	}

	Player->Client.Respawn.IRC.Channel.clear();
};

bool CIRCClientServer::Connected ()
{
	return (IRCServer.status != IRC::DISCONNECTED);
};

void CIRCClientServer::Disconnect ()
{
	for (TConnectedIRCPlayers::iterator it = ConnectedPlayers.begin(); it != ConnectedPlayers.end(); ++it)
		(*it)->PrintToClient (PRINT_HIGH, "Disconnected from %s\n", IRCServer.server.c_str());

	irc_disconnect (&IRCServer, "Forced disconnect");
};

void CIRCClientServer::Connect (std::string HostName, std::string Nick, std::string User, std::string Pass, std::string RealName, int port)
{
	if (Connected ())
		return;

	IRCServer.port = port;
	IRCServer.server = HostName;
	IRCServer.nick = Nick;
	IRCServer.user = User;
	IRCServer.pass = Pass;
	IRCServer.real_name = RealName;

    irc_ini_server(&IRCServer);

	irc_connect (&IRCServer);
	_net_setnonblocking (IRCServer.sock);
};

class CIRCListPlayerCallback : public CForEachPlayerCallback
{
public:
	CIRCClientServer		*Server;
	std::string				&chan;
	int						Count;

	CIRCListPlayerCallback (CIRCClientServer *Server, std::string &chan) :
	Server(Server),
	chan(chan),
	Count(0)
	{
	};

	CIRCListPlayerCallback &operator= (CIRCListPlayerCallback&) { return *this; }

	void Callback (CPlayerEntity *Player)
	{
		Server->PushMessage (IRC::PRIVMSG, FormatString("%s :%i:   %s%s", chan.c_str(), Count, Player->Client.Persistent.Name.c_str(), (Player->Client.Respawn.IRC.Connected()) ? FormatString(" [In IRC as %s]", Player->Client.Respawn.IRC.Nick.c_str()).c_str() : ""));
	}
};

// One update frame
void CIRCClientServer::Update ()
{
	int retVal = irc_receive(&IRCServer, 0);
	if (retVal >= 0)
	{
		// If read characters is > 0 ...
		if (IRCServer.received > 0)
		{
			// If server pings, we pong.
			switch (IRCServer.msg.command)
			{
			case IRC::RPL_CREATED:
				HostName = IRCServer.msg.prefix;
				break;
			case IRC::PING:
				irc_pong(&IRCServer);
				break;
			case IRC::PRIVMSG:
				{
					// Get name
					std::string otherName = IRCServer.msg.prefix.substr(0, IRCServer.msg.prefix.find_first_of('!'));
					std::string message = IRCServer.msg.param[1].substr(1);

					bool isAction = (message.length() > 6 && message[0] == 1 && (strncmp(message.c_str()+1, "ACTION ", 7) == 0));

					// Trim whitespace
					while (message[0] == ' ' || message[0] == 1 || message[0] == 13)
						message.erase(0, 1);
					while (message[message.length()-1] == ' ' || message[message.length()-1] == 1 || message[message.length()-1] == 13)
						message.erase (message.length()-1, 1);

					// Trim "ACTION "
					if (isAction)
						message.erase (0, 7);

					std::string channel = IRCServer.msg.param[0];
					CIRCClientServerChannel *Chan = FindChannel(channel);

					if (Q_strnicmp(message.c_str(), "!Q2Serv ", 8) == 0)
					{
						std::string cmds = message.substr(8);

						if (Q_stricmp(cmds.c_str(), "players") == 0)
						{
							PushMessage (IRC::PRIVMSG, FormatString("%s :The following players are in the server:", channel.c_str()));
							CIRCListPlayerCallback(this, channel).Query();
						}
						break;
					}
					else if (Q_stricmp(channel.c_str(), "Q2Serv") == 0)
					{
						PushMessage (IRC::PRIVMSG, FormatString("%s :I am not a bot, just a redirection service. Commands list coming soon.", otherName.c_str()));
						break;
					}

					for (TConnectedIRCPlayers::iterator it = Chan->Players.begin(); it != Chan->Players.end(); ++it)
					{
						if (!isAction)
							(*it)->PrintToClient (PRINT_CHAT, "[IRC] <%s> %s\n", otherName.c_str(), message.c_str());
						else
							(*it)->PrintToClient (PRINT_CHAT, "[IRC] %s %s\n", otherName.c_str(), message.c_str());
					}
				}
				break;
			case IRC::JOIN:
				{
					// Get name
					std::string otherName = IRCServer.msg.prefix.substr(0, IRCServer.msg.prefix.find_first_of('!'));

					if (otherName != IRCServer.nick)
					{
						std::string channel = IRCServer.msg.params;
						channel = channel.substr(1, channel.length()-2);
						CIRCClientServerChannel *Chan = FindChannel(channel);

						for (TConnectedIRCPlayers::iterator it = Chan->Players.begin(); it != Chan->Players.end(); ++it)
							(*it)->PrintToClient (PRINT_CHAT, "[IRC] %s has %s channel %s\n", otherName.c_str(), (IRCServer.msg.command == IRC::JOIN) ? "joined" : "left", channel.c_str());
					}
				}
				break;
			case IRC::PONG:
				CanSendMessages = true;
				break;
			}
		}

		IRCServer.received = 0;
	}
	else if (retVal < 0)
	{
		DebugPrintf ("IRC Shit broke\n");
		Disconnect ();
	}

	// Send messages
	SendMsgQueue ();
};

void CIRCClientServer::SendMessage (CPlayerEntity *Player, std::string Message)
{
	// "/me "
	if (Message.length() > 4 && strncmp(Message.c_str(), "/me ", 4) == 0)
	{
		Message.erase (0, 4);
		PushMessage(IRC::PRIVMSG, FormatString("%s :%cACTION >>>> %s %s", Player->Client.Respawn.IRC.Channel.c_str(), 1, Player->Client.Respawn.IRC.Nick.c_str(), Message.c_str()));
	
		for (TConnectedIRCPlayers::iterator it = ConnectedPlayers.begin(); it != ConnectedPlayers.end(); ++it)
			(*it)->PrintToClient (PRINT_CHAT, "[IRC] %s %s\n", Player->Client.Respawn.IRC.Nick.c_str(), Message.c_str());
	}
	else
	{
		PushMessage(IRC::PRIVMSG, FormatString("%s :<%s> %s", Player->Client.Respawn.IRC.Channel.c_str(), Player->Client.Respawn.IRC.Nick.c_str(), Message.c_str()));
	
		for (TConnectedIRCPlayers::iterator it = ConnectedPlayers.begin(); it != ConnectedPlayers.end(); ++it)
			(*it)->PrintToClient (PRINT_CHAT, "[IRC] <%s> %s\n", Player->Client.Respawn.IRC.Nick.c_str(), Message.c_str());
	}
};

void CIRCClientServer::PushMessage (int Cmd, std::string Str)
{
	IRCMsgQueue.push_back (TIRCMessage(Cmd, Str));

	size_t count = 0;
	for (size_t i = 0; i < IRCMsgQueue.size(); ++i)
	{
		if (IRCMsgQueue[i].first != IRC::PING)
			count++;
	}

	if ((count % 4) == 0)
		IRCMsgQueue.push_back (TIRCMessage(IRC::PING, ""));
};

void CIRCClientServer::SendMsgQueue ()
{
	if (!CanSendMessages)
		return;
	if (IRCMsgQueue.empty())
		return;

	for (TIRCMessageQueue::iterator it = IRCMsgQueue.begin(); it != IRCMsgQueue.end(); ++it)
	{
		if ((*it).first == IRC::PING)
		{
			CanSendMessages = false;
			irc_send_cmd (&IRCServer, IRC::PING, "%s", HostName.c_str());
			break;
		}

		irc_send_cmd (&IRCServer, (*it).first, "%s", (*it).second.c_str());
	}

	while (IRCMsgQueue.size())
	{
		if (IRCMsgQueue[0].first == IRC::PING)
		{
			IRCMsgQueue.erase(IRCMsgQueue.begin());
			break;
		}

		IRCMsgQueue.erase (IRCMsgQueue.begin());
	}
};

void CIRCClientServer::JoinChannel (CPlayerEntity *Player, std::string ChannelName)
{
	if (IRCServer.status != IRC::CONNECTED)
	{
		Player->PrintToClient (PRINT_HIGH, "[IRC] The server has not authenticated you yet; please wait a minute.\n");
		return;
	}

	CIRCClientServerChannel *Chan = NULL;

	if ((Chan = FindChannel(ChannelName)) != NULL)
	{
		// just join it, already opened
		Player->Client.Respawn.IRC.Channel = ChannelName;
		Chan->Join (Player);
	}
	else
	{
		CIRCClientServerChannel Channel;
		Channel.ChannelName = ChannelName;
		Channel.Server = this;

		// not created, make the channel
		PushMessage (IRC::JOIN, FormatString("%s", ChannelName.c_str()));

		Channel.Join (Player);
		Channels.push_back (Channel);
	}
};

void CIRCClientServer::LeaveChannel (CPlayerEntity *Player, std::string ChannelName)
{
	if (ChannelName.empty())
		return;

	CIRCClientServerChannel *Chan = FindChannel(ChannelName);

	// remove the player from the list
	Chan->Leave (Player);

	// No players, just leave
	if (!Chan->Players.size())
	{
		PushMessage (IRC::PART, FormatString("%s", Chan->ChannelName.c_str()));
		
		for (TIRCChannels::iterator it = Channels.begin(); it != Channels.end(); ++it)
		{
			if (&(*it) == Chan)
			{
				Channels.erase (it);
				break;
			}
		}
	}
};

TIRCServers	IRCServerList;

void UpdateIRCServers ()
{
	for (size_t i = 0; i < IRCServerList.size(); ++i)
		IRCServerList[i]->Update();
}

void CIRCClient::Disconnect ()
{
	if (!Player->Client.Respawn.IRC.Connected())
		return;

	CIRCClientServer *Server = IRCServerList[Player->Client.Respawn.IRC.ConnectedTo-1];

	// Leave the channel that we're in, if we are
	if (!Channel.empty())
		Server->LeaveChannel (Player, Channel);

	for (TConnectedIRCPlayers::iterator it = Server->ConnectedPlayers.begin(); it != Server->ConnectedPlayers.end(); ++it)
	{
		if ((*it) == Player)
		{
			Server->ConnectedPlayers.erase (it);
			break;
		}
	}
};

bool CIRCClient::Connected ()
{
	return (ConnectedTo != 0);
};

void CIRCClient::Connect (uint8 ServerIndex)
{
	CIRCClientServer *Server = IRCServerList[ServerIndex-1];

	// Add us to the list
	Server->ConnectedPlayers.push_back (Player);

	Player->PrintToClient (PRINT_HIGH, "You are now connected to %s\n", Server->IRCServer.server.c_str());

	Player->Client.Respawn.IRC.ConnectedTo = ServerIndex;
};

void CIRCClient::JoinChannel (std::string ChannelName)
{
	if (!Player->Client.Respawn.IRC.Connected())
		return;

	IRCServerList[Player->Client.Respawn.IRC.ConnectedTo-1]->JoinChannel (Player, ChannelName);
};

void CIRCClient::LeaveChannel ()
{
	if (!Player->Client.Respawn.IRC.Connected())
		return;

	IRCServerList[Player->Client.Respawn.IRC.ConnectedTo-1]->LeaveChannel (Player, Channel);
};

void CIRCClient::List ()
{
	std::string buf =	"The following IRC servers are available to connect to.\n"
						"This also lists channels that are already open to connect to.\n"
						"You can join a server by going \"irc join n\", where n\n"
						"is the number that is listed here.\n"
						"To join a channel (even make a new one), type \"irc join <name>\", where\n"
						"<name> is the channel you want to join.\n\n";

	for (size_t i = 0; i < IRCServerList.size(); ++i)
	{
		buf += FormatString ("%i            %s\n", i+1, IRCServerList[i]->IRCServer.server.c_str());
		for (size_t z = 0; z < IRCServerList[i]->Channels.size(); ++z)
			buf += FormatString ("        %i    %s\n", IRCServerList[i]->Channels[z].Players.size(), IRCServerList[i]->Channels[z].ChannelName.c_str());
	}

	Player->PrintToClient (PRINT_HIGH, "%s", buf.c_str());
};

void CIRCClient::SendMessage (std::string Msg)
{
	if (!Player->Client.Respawn.IRC.Connected())
		return;

	IRCServerList[Player->Client.Respawn.IRC.ConnectedTo-1]->SendMessage (Player, Msg);
};

void CIRCCommand::CIRCConnectCommand::operator () ()
{
	// irc connect n nick
	if (ArgCount() < 3)
		return;

	uint8 ServerIndex = ArgGeti(2);

	if (ServerIndex > IRCServerList.size()+1)
		return;

	if (Player->Client.Respawn.IRC.Connected())
	{
		Player->PrintToClient (PRINT_HIGH, "[IRC] You are already connected to an IRC server. Use \"irc disconnect\" to leave the server.\n");
		return;
	}

	std::string Nick;
	if (ArgCount() < 4)
	{
		if (Player->Client.Respawn.IRC.Nick.empty())
			Nick = Player->Client.Persistent.Name + "|Q2";
		else
			Nick = Player->Client.Respawn.IRC.Nick;
	}
	else if (ArgCount() >= 4)
		Nick = ArgGets(3);

	Player->Client.Respawn.IRC.Nick = Nick;
	Player->Client.Respawn.IRC.Connect (ServerIndex);
};

void CIRCCommand::CIRCJoinCommand::operator () ()
{
	// irc join channel
	if (ArgCount() < 3)
		return;

	Player->Client.Respawn.IRC.JoinChannel (ArgGets(2));
};

void CIRCCommand::CIRCListCommand::operator () ()
{
	Player->Client.Respawn.IRC.List ();
};

void CIRCCommand::CIRCSayCommand::operator () ()
{
	// irc say "xxx"
	if (ArgCount() < 3)
		return;

	Player->Client.Respawn.IRC.SendMessage (ArgGets(2));
};

void CIRCCommand::CIRCDisconnectCommand::operator () ()
{
	// irc disconnect
	if (ArgCount() < 2)
		return;

	Player->Client.Respawn.IRC.Disconnect ();
};

void CIRCCommand::CIRCLeaveCommand::operator () ()
{
	Player->Client.Respawn.IRC.LeaveChannel ();
};

void CSvIRCConnectToCommand::operator () ()
{
	// sv irc connect server port
	if (ArgCount() < 4)
		return;

	if (ArgGets(2) == "connect")
	{
		int port = 6667;
		if (ArgCount() >= 5)
			port = ArgGeti(4);
			
		CIRCClientServer *Server = QNew (TAG_GENERIC) CIRCClientServer;
		Server->Connect (ArgGets(3).c_str(), "Q2Serv", "Q2Serv", "", "Q2Serv", port);
		
		IRCServerList.push_back (Server);
	}
};

void SvCmd_Irc_Disconnect ()
{
	// sv irc disconnect server
	if (ArgCount() < 3)
		return;

	for (TIRCServers::iterator it = IRCServerList.begin(); it != IRCServerList.end(); ++it)
	{
		if (Q_stricmp((*it)->IRCServer.server.c_str(), ArgGets(3).c_str()) == 0)
		{
			IRCServerList.erase (it);
			break;
		}
	}
};

#endif