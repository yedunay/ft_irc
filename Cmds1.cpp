#include "ft_irc.hpp"

bool ValidChannelName(const std::string& channelName)
{
    if (channelName.length() > 200)
        return false;
	else if (channelName.length() == 1)
		return false;
    else if (channelName.empty() || (channelName[0] != '&' && channelName[0] != '#'))
	{
		return (false);
	}
	for (unsigned int i = 0; i < channelName.length(); ++i)
	{
        char c = channelName[i];
        if (c == ' ' || c == 7 || c == ',')
            return false;
    }
    return true;
}

void Join(std::map<std::string, Channel> &channels, Client &client, std::vector<std::string> &cmd)
{
    if (cmd.size() > 3 || cmd.size() == 1)
        throw ERR_NEEDMOREPARAMS(client.getNickname(), cmd[0]);
    else if (ValidChannelName(cmd[1]) == false)
        throw ERR_BADCHANMASK(client.getNickname(), cmd[1]);
    std::string password = cmd.size() == 3 ? cmd[2] : "";
    if (channels.find(cmd[1]) != channels.end())
    {
        Channel& channel = channels[cmd[1]];
        if (channel.IsClient(client))
        {
            client.MsgToClient(ERR_USERONCHANNEL(client.getNickname(), channel.getName()));
            return;
        }
        if (channel.IsFlags('k') && channel.getPassword() != password)
            throw ERR_BADCHANNELKEY(client.getNickname(), channel.getName());
        else if (channel.IsFlags('l') && channel.getLimit() == true)
            throw ERR_CHANNELISFULL(client.getNickname(), cmd[1]);
        else if (channel.IsFlags('i') == true && channel.IsInvites(client.getNickname()) == false)
            throw ERR_INVITEONLYCHAN(client.getNickname(), cmd[1]);

        channel.ClientAdd(client);
        channel.Brodcast(RPL_JOIN(client.getPrefixName(), cmd[1]));
        channel.Brodcast(RPL_NAMREPLY(client.getPrefixName(), cmd[1], channel.getUsersNames()));
        channel.Brodcast(RPL_ENDOFNAMES(client.getPrefixName(), cmd[1]));
    }
    else
    {
        std::string joinMessage = RPL_JOIN(client.getPrefixName(), cmd[1]);
        Channel channel(cmd[1], password);

        channel.ClientAdd(client);
        channel.OperatorAdd(client);
        channels[cmd[1]] = channel;
        channels[cmd[1]].Brodcast(joinMessage);
        client.MsgToClient(RPL_NAMREPLY(client.getPrefixName(), cmd[1], channels[cmd[1]].getUsersNames()));
        client.MsgToClient(RPL_ENDOFNAMES(client.getPrefixName(), cmd[1]));
        client.MsgToClient(RPL_MODE(client.getPrefixName(), cmd[1], "+o ", client.getNickname()));
    }
}

void    Topic(std::map<std::string, Channel> &channels, Client &client, std::vector<std::string> cmd)
{
    if (cmd.size() == 1)
        throw ERR_NEEDMOREPARAMS(client.getNickname(), "TOPIC");
    else if (channels[cmd[1]].IsClient(client) == false)
        throw ERR_NOTONCHANNEL(client.getNickname(), channels[cmd[1]].getName());
    else if (channels[cmd[1]].IsFlags('t') == false)
    {
        std::string topic;
        if (cmd.size()  == 3)
        {
            topic = RPL_TOPIC(client.getNickname(), channels[cmd[1]].getName(), cmd[2]);
            channels[cmd[1]].TopicAdd(cmd[2]);
        }
        else if (!channels[cmd[1]].getTopic().empty())
            topic = RPL_TOPIC(client.getNickname(), channels[cmd[1]].getName(), channels[cmd[1]].getTopic());
        else
        {
            client.MsgToClient(RPL_NOTOPIC(client.getNickname(), channels[cmd[1]].getName()));
            return;
        }
        channels[cmd[1]].Brodcast(topic);
    }
    else
    {
        if (channels[cmd[1]].IsOperator(client))
        {
            std::string topic;
            if (cmd.size()  == 3)
            {
                topic = RPL_TOPIC(client.getNickname(), channels[cmd[1]].getName(), cmd[2]);
                channels[cmd[1]].TopicAdd(cmd[2]);
            }
            else if (!channels[cmd[1]].getTopic().empty())
                topic = RPL_TOPIC(client.getNickname(), channels[cmd[1]].getName(), channels[cmd[1]].getTopic());
            else
                client.MsgToClient(RPL_NOTOPIC(client.getNickname(), channels[cmd[1]].getName()));

            if (topic.empty() == false)
                channels[cmd[1]].Brodcast(topic);
        }
        else
            throw ERR_CHANOPRIVSNEEDED(client.getNickname(), channels[cmd[1]].getName());
    }
}

static bool ValidNickname(const std::string& nickname) {
    if (nickname.length() < 1 || nickname.length() > 9)
        return false;

    if (!isalpha(nickname[0]))
        return false;

    for (size_t i = 0; i < nickname.length(); ++i)
    {
        char c = nickname[i];
        if (!isalnum(c) && c != '-' && c != '[' && c != ']' && c != '\\' && c != '^' && c != '{' && c != '}' && c != '_')
            return false;
    }
    return true;
}

void Nick(std::map<int, Client>& clients, Client &client, std::vector<std::string> cmd, std::map<std::string, Channel> &channels)
{
	if (cmd.size() == 1)
		throw ERR_NONICKNAMEGIVEN(client.getNickname());
	else if (cmd.size() != 2)
		throw ERR_NEEDMOREPARAMS(client.getNickname(), cmd[0]);
	else if (IsClient(clients, cmd[1]) == true)
		throw ERR_NICKNAMEINUSE(cmd[1]);
	else if (ValidNickname(cmd[1]) == false)
		throw ERR_ERRONEUSNICKNAME(cmd[1]);
	client.MsgToClient(RPL_NICK(client.getPrefixName(), cmd[1]));
	client.setNickname(cmd[1]);
    for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); it++)
    {
        if (it->second.IsClient(client))
            it->second.Brodcast(RPL_NAMREPLY(client.getPrefixName(), it->second.getName(), it->second.getUsersNames()), client);
			it->second.Brodcast(RPL_ENDOFNAMES(client.getPrefixName(), it->second.getName()), client);
    }
	return ;
}


void Nick(std::map<int, Client>& clients, Client &client, std::vector<std::string> cmd)
{
	if (cmd.size() == 1)
		throw ERR_NONICKNAMEGIVEN(client.getNickname());
	else if (cmd.size() != 2)
		throw ERR_NEEDMOREPARAMS(client.getNickname(), cmd[0]);
	else if (IsClient(clients, cmd[1]) == true)
		throw ERR_NICKNAMEINUSE(cmd[1]);
	else if (ValidNickname(cmd[1]) == false)
		throw ERR_ERRONEUSNICKNAME(cmd[1]);
	else if (client.isRegistered() == true)
    {
		client.MsgToClient(RPL_NICK(client.getPrefixName(), cmd[1]));
    }
	client.setNickname(cmd[1]);
}

void	Kick(std::map<std::string, Channel> &channels, Client &client, std::vector<std::string> &cmd)
{
	if (cmd.size() <= 2 || cmd.size() > 4)
		throw ERR_NEEDMOREPARAMS(client.getUsername(), cmd[0]);
	std::vector<std::string>	users = split(cmd[2], ',');
	std::vector<std::string>	channelsName = split(cmd[1], ',');
	if (users.size() != 1 || channelsName.size() != 1)
		throw ERR_NEEDMOREPARAMS(client.getUsername(), cmd[0]);
	else if (channels.find(channelsName[0]) == channels.end())
		throw ERR_NOSUCHCHANNEL(client.getUsername(), channelsName[0]);
	else if (channels[channelsName[0]].IsClient(client) == false)
		throw ERR_NOTONCHANNEL(client.getUsername(), channelsName[0]);
	else if (channels[channelsName[0]].IsOperator(client) == false)
		throw ERR_CHANOPRIVSNEEDED(client.getUsername(), channelsName[0]);
	else if (channels[channelsName[0]].IsClient(users[0]) == false)
		throw ERR_NOTONCHANNEL(client.getUsername(), channelsName[0]);
	else
	{
		if (cmd[2] != client.getNickname())
		{
			std::string reason = cmd.size() != 4 ? "" : cmd[3];
			std::string message = RPL_KICK(client.getNickname(), client.getUsername(), client.getHostName(),
										channels[channelsName[0]].getName(),
										channels[channelsName[0]].getClient(users[0]).getNickname(),
										reason);
			channels[channelsName[0]].Brodcast(message);
			channels[channelsName[0]].ClientRemove(channels[channelsName[0]].getClient(users[0]));

		}
		else
		{
			std::string m = "You cannot assign this to yourself.";
			client.MsgToClient(m);
		}
	}
}