#ifndef SERVER_HPP
#define SERVER_HPP

// ### LIBRARIES ###

//    FOR SERVER    //
#include <netinet/in.h>	//sockamddr_in
#include <sys/socket.h> //send
#include <poll.h>
#include <vector>	
#include <map>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cstring>	 	// memset strerror
#include <unistd.h>		// close
#include <stdexcept>	// runtime_error
#include <cerrno>   // errno
#include <csignal>  // signal
#include <string>
#include <cstddef> // size_t
#include "rpls_and_errs.hpp"

// ### CLASSES ###

class Client
{
    public:
        Client();
        Client(int fd);
		Client(const std::string& nickname, const std::string& username, int fd);
        int					getClientFd() const;
		const std::string&	getNickname() const;
		const std::string&	getUsername() const;
		const std::string&	getRealName() const;
		const std::string&	getBuffer() const;
		const std::string&	getHostName() const;
		const std::string&	getOldName() const;
		bool				getBufferLine(std::string &str);
		bool				getPass() const;
		std::string			getPrefixName();

		bool				isRegistered() const;

        void	            setOldNickName(const std::string& oldNickName);
		void	            setUsername(const std::string& username);
		void	            setRealName(const std::string& realname);
		void	            setHostName(const std::string& hostname);
		void	            setServerName(const std::string& servername);
		void	            setNickname(const std::string& nickname);
		void	            setPass(bool pass);
		void	            setClient(); 
		void	            MsgToClient(const std::string &message);
		void	            appendBuffer(const std::string& appendBuffer);
		bool	            operator==(const Client& other) const;

    protected:
		int _clientFd;
		std::string _oldNickName;
		std::string _nickname;	
		std::string _username;
		std::string _realname;
		std::string _hostname;
		std::string _servername;
		std::string _buffer;
		bool		_pass;
		bool		_registered;
};


typedef std::vector<Client *>::iterator it;

class Channel
{
	private:
		std::string 				_name;
		std::string 				_topic;
		std::string 				_password;
		std::map<char, bool>		_flags;
		std::vector<Client *>		_clients;
		std::vector<Client *>		_operator;
		size_t						_maxLimit;
		std::vector<std::string>	_invites;

	public:
		Channel();
		Channel(const std::string& name, const std::string &password);

		std::string				getName();
		std::vector<Client *>	&getClients();
		std::vector<Client *>	&getOperator();
		std::string 			getUsersNames();
		bool					getLimit() const;
		std::string				getTopic() const;
		Client					&getClient(std::string target);
		std::string				getSizeClient() const;
		std::string				getPassword() const;
		size_t countOperators(Channel &channel) const;

		bool	IsFlags(char c);
		bool	IsOperator(Client &client);
		bool 	IsOperator(std::string &op);
		bool	IsClient(Client &client);
		bool	IsClient(const std::string &client);
		bool	IsInvites(const std::string &invited);

		void	setPassword(std::string password);
		void	setInvite(std::string &invited);
		void	setLimit(size_t Limit);
		void	setFlags(char c, bool status);

		void	removeInvite(std::string &invited);
		void	ClientAdd(Client &newClient);
		void	ClientRemove(Client &removeClient);
		void	OperatorRemove(Client &removeClient);
		void	OperatorAdd(Client &newOperator);
		void	TopicAdd(std::string &topic);
		void	Brodcast(std::string message);
		void	Brodcast(std::string message, Client &client);
		std::string getFlags();
};

class Server
{
	public:
		Server(int port, std::string password);
		~Server();
		void	Launch();
		void	acceptClient();
		void	processUserEvents();
		void	processMessage(Client &client);
		void	routeCommand(Client &Client, std::vector<std::string> &cmd);
		void	login(Client &client, std::vector<std::string>	&str);
		void	list(Client &client);
		int		receiveData(Client &client);
		static void updateSignal(int signal);

		std::string	getPassword() const;
	private:
		std::vector<pollfd>				_pollFds;
		std::map<int, Client>			_clients;
		std::string						serverName;
		const int						_port;
		const std::string				_password;
		int								_socket;
		struct sockaddr_in				server_addr;
		std::map<std::string, Channel>	_channels;
		static int						_signal;
};



//		FUNCTIONS		//
const Client                getClientNameFd(const std::map<int, Client>& clients, const std::string& target);
bool	                    IsClient(const std::map<int, Client>& clients, const std::string &client);
bool                        IsClient(const std::map<int, Client>& clients, Client &client);
std::vector<std::string>	split(std::string str, char	c);
void                        RemoveChannels(std::map<std::string, Channel> &channels, Client &client);

void    Nick(std::map<int, Client>& clients, Client &client, std::vector<std::string> cmd);
void    Nick(std::map<int, Client>& clients, Client &client, std::vector<std::string> cmd, std::map<std::string, Channel> &channels);
void	Join(std::map<std::string, Channel> &channels, Client &client, std::vector<std::string> &cmd);
void	Privmsg(Client client, std::vector<std::string> &cmd, std::map<std::string, Channel> channels, std::map<int, Client>& clients);
void    Quit(std::map<std::string, Channel> &channels, std::map<int, Client>& clients, int clientFd, std::vector<pollfd>& pollFds);
void	User(Client &client, std::vector<std::string> cmd);
void    Part(std::map<std::string, Channel> &channels, Client &client,  std::vector<std::string> &cmd);
void	Pass(Server &server, Client &client, std::vector<std::string> cmd);
void    Mode(std::map<std::string, Channel> &channels, Client &client ,std::vector<std::string> cmd);
void    Topic(std::map<std::string, Channel> &channels, Client &client, std::vector<std::string> cmd);
void    Invite(std::map<std::string, Channel> &channels, std::map<int, Client> &clients, Client &client, std::vector<std::string> cmd);
void	Kick(std::map<std::string, Channel> &channels, Client &client, std::vector<std::string> &cmd);

#endif