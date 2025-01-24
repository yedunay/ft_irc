#include "ft_irc.hpp"

void Part(std::map<std::string, Channel>& channels, Client& client, std::vector<std::string>& cmd)
{
    if (!(cmd.size() == 2 || cmd.size() == 3)) {
        client.MsgToClient(ERR_NEEDMOREPARAMS(client.getNickname(), cmd[0]));
        return;
    }
    
    std::string channelName = cmd[1];
    if (channels.find(channelName) == channels.end()) {
        client.MsgToClient(ERR_NOSUCHCHANNEL(client.getPrefixName(), channelName));
        return;
    }

    Channel& channel = channels[channelName];
    if (channel.IsClient(client)) {
        size_t operatorCount = channel.countOperators(channel);
        bool isClientOperator = channel.IsOperator(client);
        bool isLastClient = (channel.getSizeClient() == "1");

        if (isClientOperator && operatorCount == 1 && !isLastClient) {
            client.MsgToClient(ERR_CANNOTLEAVEASADMIN(client.getNickname(), channelName));
            return;
        }

        channel.Brodcast(RPL_PART(client.getPrefixName(), channelName));
        if (isClientOperator)
            channel.OperatorRemove(client);
        channel.ClientRemove(client);
        
        if (isLastClient)
            channels.erase(channelName);
    } else {
        client.MsgToClient(ERR_NOTONCHANNEL(client.getNickname(), channelName));
    }
}

void	User(Client &client, std::vector<std::string> cmd)
{
	if (cmd.size() != 5)
	{
		client.MsgToClient(ERR_NEEDMOREPARAMS(client.getNickname(), cmd[0]));
		return;
	}
	client.setUsername(cmd[1]);
	client.setHostName(cmd[2]);
	client.setServerName(cmd[3]);
	client.setRealName(cmd[4]);
}

void Pass(Server &server, Client &client, std::vector<std::string> cmd)
{
	if (cmd.size() != 2)
	{
		throw ERR_NEEDMOREPARAMS(client.getNickname(), "PASS");
		client.setPass(false);
		return ;
	}

	if (client.isRegistered())
	{
		throw (ERR_ALREADYREGISTRED(client.getNickname()));
		return ;
	}

	if (cmd[1] != server.getPassword())
	{
		client.setPass(false);
		throw ERR_PASSWDMISMATCH(client.getNickname());
		return ;
	}
	client.setPass(true);
}

void    Invite(std::map<std::string, Channel> &channels, std::map<int, Client> &clients, Client &client, std::vector<std::string> cmd)
{
	if (cmd.size() != 3)
		throw ERR_NEEDMOREPARAMS(client.getNickname(), cmd[0]);
	else if (cmd.size() == 3)
	{
		if (channels.find(cmd[2]) == channels.end())
			throw ERR_NOSUCHCHANNEL(client.getPrefixName(), cmd[2]);
		else if (IsClient(clients, cmd[1]) == false)
			throw (ERR_NOSUCHNICK(client.getNickname(), cmd[1]));
		else if (channels[cmd[2]].IsClient(client) == false)
			throw ERR_NOTONCHANNEL(client.getNickname(), cmd[2]);
		else if (channels[cmd[2]].IsOperator(client) == false)
			throw ERR_CHANOPRIVSNEEDED(client.getNickname(), cmd[2]);
		else
		{
				std::string in = RPL_INVITING(client.getNickname(), cmd[1], cmd[2]);
				channels[cmd[2]].setInvite(cmd[1]);
				client.MsgToClient(in);
		}
	}
}

std::vector<pollfd>::iterator Find(std::vector<pollfd>& pollFds, int clientFd)
{
	std::vector<pollfd>::iterator a;
	for (a = pollFds.begin(); a != pollFds.end(); a++)
	{
		if (a->fd == clientFd)
			return (a);
	}
	throw std::runtime_error("Fd Not Found");
}


void Quit(std::map<std::string, Channel> &channels, std::map<int, Client>& clients, int clientFd, std::vector<pollfd>& pollFds)
{
	try
	{
		Client &client = clients[clientFd];
		client.MsgToClient("QUIT : Good bye.");
		std::cout << clientFd << " Disconnected() :( " << std::endl;
		RemoveChannels(channels, client);
		clients.erase(clientFd);
		pollFds.erase(Find(pollFds, clientFd));
		close(clientFd);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

void Privmsg(Client client, std::vector<std::string> &cmd, std::map<std::string, Channel> channels, std::map<int, Client>& clients)
{
    std::string privMessage;
    if (cmd.size() == 1)
        throw ERR_NORECIPIENT(client.getNickname());
    else if (cmd[1].find(',') != std::string::npos)
        throw ERR_TOOMANYTARGETS(client.getNickname());
    else if (cmd.size() == 2)
        throw ERR_NOTEXTTOSEND(client.getNickname());
    else if (cmd[1][0] == '#' || cmd[1][0] == '&')
    {
        if (channels.find(cmd[1]) == channels.end())
            throw (ERR_NOTOPLEVEL(client.getNickname()));
        else
        {
            Channel& channel = channels[cmd[1]];
            if (!channel.IsClient(client.getNickname())) {
                client.MsgToClient(ERR_CANNOTSENDTOCHAN(client.getNickname(), cmd[1]));
                return;
            }
            privMessage = RPL_PRIVMSG(client.getPrefixName(), cmd[1], cmd[2]);
            channel.Brodcast(privMessage, client);
        }
    }
    else
    {
        try
        {
            Client targetClient = getClientNameFd(clients, cmd[1]);
            privMessage = RPL_PRIVMSG(client.getPrefixName(), targetClient.getNickname(), cmd[2]);
            targetClient.MsgToClient(privMessage);
        }
        catch (const std::exception& e)
        {
            client.MsgToClient(ERR_NOSUCHNICK(client.getNickname(), cmd[1]));
            return;
        }

    }
}