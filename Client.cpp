#include "ft_irc.hpp"

Client::Client() : _clientFd(-1), _pass(false), _registered(false) {}

Client::Client(int fd) : _clientFd(fd), _nickname(""), _pass(false), _registered(false) {}

Client::Client(const std::string& nickname, const std::string& username, int fd)
    : _clientFd(fd), _nickname(nickname), _username(username), _pass(false), _registered(false) {}

const std::string& Client::getNickname() const { return _nickname; }
const std::string& Client::getUsername() const { return _username; }
const std::string& Client::getOldName() const { return _oldNickName; }
const std::string& Client::getRealName() const { return _realname; }
const std::string& Client::getHostName() const { return _hostname; }
const std::string& Client::getBuffer() const { return _buffer; }
bool Client::isRegistered() const { return _registered; }
int Client::getClientFd() const { return _clientFd; }
bool Client::getPass() const { return _pass; }

void Client::setOldNickName(const std::string& oldNickName) { _oldNickName = oldNickName; }
void Client::setNickname(const std::string& nickname) { _nickname = nickname; }
void Client::setUsername(const std::string& username) { _username = username; }
void Client::setRealName(const std::string& realname) { _realname = realname; }
void Client::setHostName(const std::string& hostname) { _hostname = hostname; }
void Client::setServerName(const std::string& servername) { _servername = servername; }
void Client::setClient() { _registered = true; }
void Client::setPass(bool pass) { _pass = pass; }

std::string Client::getPrefixName() {
    std::string prefix;
    std::string usname = _username.empty() ? "" : "!" + _username;
    std::string hostname = _hostname.empty() ? "" : "@" + _hostname;
    prefix = _nickname + usname + hostname;
    return prefix;
}

void Client::appendBuffer(const std::string& appendBuffer) {
    _buffer.append(appendBuffer);
}

bool Client::operator==(const Client& other) const {
    return _clientFd == other._clientFd;
}

bool Client::getBufferLine(std::string& str) {
    size_t find = _buffer.find("\r\n");
    if (find == std::string::npos || find > _buffer.size()) {
        return false;
    }
    str = _buffer.substr(0, find);
    _buffer.erase(0, find + 2);
    return true;
}

void Client::MsgToClient(const std::string& message)
{
    std::string buf = message + "\r\n";
    if (send(_clientFd, buf.c_str(), buf.size(), 0) < 0)
        throw std::runtime_error("Failed to send message");
}
