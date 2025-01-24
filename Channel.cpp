#include "ft_irc.hpp"

Channel::Channel() {}

Channel::Channel(const std::string &name, const std::string &password)
{
    _password = password;
    _name = name;
    _maxLimit = 0;
    _flags['i'] = false;
    _flags['t'] = false;
    _flags['k'] = !password.empty();
    _flags['l'] = false;
}

// Getters
std::string Channel::getName() { return _name; }
std::vector<Client *> &Channel::getClients() { return _clients; }
std::vector<Client *> &Channel::getOperator() { return _operator; }
std::string Channel::getTopic() const { return _topic; }
bool Channel::getLimit() const { return (_maxLimit <= _clients.size()); }
std::string Channel::getSizeClient() const 
{
    std::ostringstream oss; 
    oss << _clients.size(); 
    return oss.str();
}
std::string Channel::getPassword() const { return _password; }

// Setters
void Channel::setPassword(std::string password) { _password = password; }
void Channel::setLimit(size_t limit) { _maxLimit = limit; }
void Channel::setInvite(std::string &invited) { _invites.push_back(invited); }

// Adders
void Channel::TopicAdd(std::string &topic) { _topic = topic; }
void Channel::ClientAdd(Client &newClient) { _clients.push_back(&newClient); }
void Channel::OperatorAdd(Client &newOperator) { _operator.push_back(&newOperator); }

void Channel::ClientRemove(Client &removeClient)
{
    it user = find(_clients.begin(), _clients.end(), &removeClient);
    if (user != _clients.end())
    {
        _clients.erase(user);
        return;
    }
    throw std::runtime_error("Channel Error: Client Not Found");
}

bool Channel::IsFlags(char flag)
{
    if (_flags.find(flag) != _flags.end())
        return _flags[flag];
    return false;
}

void Channel::setFlags(char flag, bool status)
{
    if (_flags.find(flag) != _flags.end())
        _flags[flag] = status;
    else
        throw std::runtime_error("Channel Error: Invalid Flag");
}

void Channel::OperatorRemove(Client &removeClient)
{
    it user = find(_operator.begin(), _operator.end(), &removeClient);
    if (user != _operator.end())
        _operator.erase(user);
    else
        throw std::runtime_error("Channel Error: Operator Not Found");
}

void Channel::removeInvite(std::string &invited)
{
    std::vector<std::string>::iterator it = find(_invites.begin(), _invites.end(), invited);
    if (it != _invites.end())
        _invites.erase(it);
    else
        throw std::runtime_error("Channel Error: Invited Not Found");
}

std::string Channel::getUsersNames()
{
    std::string usersNames;
    for (it user = _clients.begin(); user != _clients.end(); ++user)
    {
        if (this->IsOperator(*(*user)))
            usersNames += "@" + (*user)->getNickname() + " ";
        else
            usersNames += (*user)->getNickname() + " ";
    }

    if (!usersNames.empty() && usersNames[usersNames.size() - 1] == ' ')
        usersNames.resize(usersNames.size() - 1);
    return usersNames;
}


bool Channel::IsOperator(Client &client)
{
    it operatorClient = find(_operator.begin(), _operator.end(), &client);
    return operatorClient != _operator.end();
}

bool Channel::IsOperator(std::string &nickname)
{
    for (size_t i = 0; i < _operator.size(); ++i)
    {
        if (_operator[i]->getNickname() == nickname)
            return true;
    }
    return false;
}

bool Channel::IsClient(Client &client)
{
    it user = find(_clients.begin(), _clients.end(), &client);
    return user != _clients.end();
}

bool Channel::IsClient(const std::string &nickname)
{
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i]->getNickname() == nickname)
            return true;
    }
    return false;
}

bool Channel::IsInvites(const std::string &invited)
{
    std::vector<std::string>::const_iterator it = find(_invites.begin(), _invites.end(), invited);
    return it != _invites.end();
}

Client &Channel::getClient(std::string target)
{
    for (it user = _clients.begin(); user != _clients.end(); ++user)
    {
        if ((*user)->getNickname() == target)
            return *(*user);
    }
    throw std::runtime_error("Channel Error: Client Not Found");
}

void Channel::Brodcast(std::string message)
{
    for (it user = _clients.begin(); user != _clients.end(); ++user)
        (*user)->MsgToClient(message);
}

void Channel::Brodcast(std::string message, Client &client)
{
    for (it user = _clients.begin(); user != _clients.end(); ++user)
    {
        if ((*user)->getNickname() != client.getNickname())
            (*user)->MsgToClient(message);
    }
}

std::string Channel::getFlags()
{
    std::string flags;
    for (std::map<char, bool>::iterator it = _flags.begin(); it != _flags.end(); ++it)
    {
        if (it->second)
        {
            flags += it->first;
            flags += " ";
        }
    }
    return flags;
}
