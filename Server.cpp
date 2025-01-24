#include "ft_irc.hpp"

int Server::_signal = 0;

Server::Server(int port, std::string password) : _port(port), _password(password)
{
    std::cout << "Server constructor called." << std::endl;
    serverName = "irc.myserver.com";
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
        throw std::runtime_error("Socket creation failed.");

    int buffer_size = 1;

    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &buffer_size, sizeof(buffer_size)) == -1)
    {
        close(_socket);
        throw std::runtime_error("setsockopt failed.");
    }

    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(_port);
    if (bind(_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        close(_socket);
        throw std::runtime_error("Binding failed.");
    }
    std::cout << "Binding successful. The server is now connected to port " << _port << "." << std::endl;
    if (listen(_socket, 2) == -1)
    {
        close(_socket);
        throw std::runtime_error("Listening failed.");
    }
    pollfd fds;
    fds.fd = _socket;
    fds.events = POLLIN;
    fds.revents = 0;
    _pollFds.push_back(fds);
}

Server::~Server()
{
	std::vector<pollfd>::iterator it = _pollFds.begin();
	while (it != _pollFds.end())
	{
		close(it->fd);
		it++;
	}
	std::cout << "Server Destructor Called" << std::endl;
	close(_socket);
}

void signalHandler(int signal)
{
     Server::updateSignal(signal);
}

void  Server::updateSignal(int signal) 
{
     _signal = signal;
}

void Server::Launch()
{
    signal(SIGINT, signalHandler);
    while (_signal == 0)
    {
        int retval = poll(&_pollFds[0], _pollFds.size(), -1);
        if (retval == -1)
        {
            close(_socket);
            std::cout << strerror(errno) << std::endl;
			throw std::runtime_error("poll Error");
        }
        if(_pollFds[0].revents & POLLIN)
            this->acceptClient();
        processUserEvents();
    }
}

void Server::acceptClient()
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int clientSocket = accept(_socket, (struct sockaddr*)&clientAddr, &clientLen);
	if (clientSocket == -1)
	{
        std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
		return ;
	}

	pollfd clientPollFd;
	clientPollFd.fd = clientSocket;
	clientPollFd.events = POLLIN;
	clientPollFd.revents = 0;
	_pollFds.push_back(clientPollFd);
	_clients[clientSocket] = Client(clientSocket);
    std::cout << "Client " << clientSocket << " connected." << std::endl;
    _clients[clientSocket].MsgToClient("Welcome to the server!");
}

int Server::receiveData(Client &client)
{
	char buffer[1024];
	std::memset(buffer, 0, sizeof(buffer));
	int bytesReceived = recv(client.getClientFd(), buffer, sizeof(buffer) - 1, 0);
	if (bytesReceived <= 0) 
    {
		if (bytesReceived == 0) 
        {
			std::cout << "Client " << client.getClientFd() << " has disconnected." << std::endl;
			RemoveChannels(_channels, client);
		}
        else 
			std::cerr << "Failed to receive data from client " << client.getClientFd() << ": " << strerror(errno) << std::endl;
	} 
    else 
    {
		buffer[bytesReceived] = '\0';
		client.appendBuffer(buffer);
	}
	return bytesReceived;
}


void	Server::login(Client &client, std::vector<std::string>	&str)
{
	if (str[0] == "CAP");
	else if (str[0] == "NICK")	Nick(_clients, client, str);
	else if (str[0] == "USER")  User(client, str);
	else if (str[0] == "PASS") 	Pass(*this , client, str);
	else if (str[0] == "QUIT")
	{
		Quit(_channels, _clients, client.getClientFd(), _pollFds);
		return ;
	}
	else
		client.MsgToClient("ERROR: You have to register first!");

	if (!client.getUsername().empty() && !client.getHostName().empty() && !client.getNickname().empty() 
		&& !client.getRealName().empty() && client.getPass() == true)
	{
		client.MsgToClient(RPL_WELCOME(serverName, client.getNickname(), client.getUsername(), client.getHostName()));
		client.setClient();
	}
}

void Server::routeCommand(Client &client, std::vector<std::string> &cmd)
{
	if (cmd[0] == "USER") 			client.MsgToClient(ERR_ALREADYREGISTRED(client.getNickname()));
    else if (cmd[0] == "NICK") 		Nick(_clients, client, cmd, _channels);
	else if (cmd[0] == "KICK") 		Kick(_channels, client, cmd);
    else if (cmd[0] == "JOIN") 		Join(_channels, client, cmd);
	else if (cmd[0] == "PASS") 		Pass(*this , client, cmd);
    else if (cmd[0] == "PRIVMSG") 	Privmsg(client, cmd, _channels, _clients);
	else if (cmd[0] == "QUIT")	 	Quit(_channels, _clients, client.getClientFd(), _pollFds);
	else if (cmd[0] == "PART")		Part(_channels, client, cmd);
	else if (cmd[0] == "MODE") 		Mode(_channels, client, cmd);
	else if (cmd[0] == "TOPIC")		Topic(_channels, client, cmd);
	else if (cmd[0] == "INVITE")	Invite(_channels, _clients ,client, cmd);
	else if (cmd[0] == "LIST") 		this->list(client);
	else if (cmd[0] == "WHO")		;
	else							client.MsgToClient("ERROR: Unknown Command!");
}

std::string	Server::getPassword() const {return (this->_password); }

void Server::list(Client &client)
{
	client.MsgToClient(RPL_LISTSTART(client.getNickname()));
	std::map<std::string, Channel>::iterator channel;
	for (channel = _channels.begin(); channel != _channels.end(); ++channel)
	{
		std::string mes = RPL_LIST(client.getNickname(), channel->second.getName(), channel->second.getSizeClient(), channel->second.getTopic());
		client.MsgToClient(mes);
	}
	client.MsgToClient(RPL_LISTEND(client.getNickname()));
}

void Server::processMessage(Client &client)
{
    std::string command;
    while (IsClient(_clients, client) && client.getBufferLine(command))
    {
        try
        {
            std::vector<std::string> str = split(command, ' ');
            if (str.empty())
            {
                client.MsgToClient("ERROR: Command cannot be empty.");
                continue;
            }
            if (!client.isRegistered())
                login(client, str);
            else
                routeCommand(client, str);
        }
        catch (const std::string &err)
        {
            client.MsgToClient("ERROR: " + err);
        }
    }
}

void Server::processUserEvents()
{
	for (size_t i = 1; i < _pollFds.size(); ++i) 
	{
		if (_pollFds[i].revents & POLLIN)
		{
			int bytesReceived = receiveData(_clients[_pollFds[i].fd]);
			if (bytesReceived <= 0)
			{
				Quit(_channels ,_clients, _pollFds[i].fd , _pollFds);
				continue ;
			}
			processMessage(_clients[_pollFds[i].fd]);
		}
	}
}
