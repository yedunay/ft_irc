
# **ft_irc**

**ft_irc** is an IRC (Internet Relay Chat) server implementation built in C++ in compliance with the **RFC 1459** standard. This project demonstrates the principles of server-client communication, multi-user real-time messaging, and protocol implementation.

---

## 📋 **Project Objective**  
The purpose of this project is to implement an IRC server that:  
- Handles multiple clients.  
- Manages channels.  
- Allows real-time messaging.  

This showcases a strong understanding of network protocols and C++ development skills.  

---

## ✨ **Key Features**  

### **Channel Management**  
- Create, delete, and manage channels.  
- Assign operator privileges to specific users.  
- Set and update channel topics.  

### **User Management**  
- Authenticate and register users on the server.  
- Allow users to join or leave channels (`JOIN`, `PART`).  
- Invite users to channels (`INVITE`).  

### **Messaging**  
- Send messages between users or channels (`PRIVMSG`).  
- Broadcast messages to all clients in a channel.  

### **Authorization**  
- Require passwords for server login (`PASS`).  
- Implement permission checks for restricted actions (e.g., operator-only features).  

---

## 🛠 **Technical Details**  

- **Programming Language**: C++ (C++98 standard).  
- **Networking**:  
  - Built using **Berkeley Sockets** for server-client communication.  
  - Supports multiple clients using the `select()` system call.  
- **Modular Design**:  
  - Separate classes for channels and clients: `Channel`, `Client`.  
  - Core server logic handled by the `Server` class.  
- **RFC 1459 Compliance**:  
  - Fully implements commands and error messages as specified in the standard.  

---

## 📜 **Installation and Usage**  

### **Clone the repository**  
```bash
git clone https://github.com/your_username/ft_irc.git  
cd ft_irc  
```

### **Compile the project**  
```bash
make  
```

### **Run the server**  
```bash
./ircserv <port> <password>  
```
- **port**: The port number where the server will listen.  
- **password**: The password required for clients to connect.  

### **Connect an IRC client**  
Use an IRC client (e.g., **HexChat**, **WeeChat**) to connect to the server.  

---

## 📖 **Supported Commands**

| **Command**  | **Description**                                       | **Example Usage**                 |
|--------------|-------------------------------------------------------|-----------------------------------|
| `PASS`       | Verifies the password before connecting to server.    | `/pass <password>`               |
| `NICK`       | Assigns a nickname to a user.                         | `/nick <nickname>`               |
| `USER`       | Defines the user connecting to the server.            | `/user <username>`               |
| `JOIN`       | Allows a user to join a channel.                      | `/join <channel>`                |
| `PART`       | Allows a user to leave a channel.                     | `/part <channel>`                |
| `PRIVMSG`    | Sends a private message to a user or channel.         | `/privmsg <target> <message>`    |
| `INVITE`     | Invites a user to join a channel.                     | `/invite <nickname> <channel>`   |
| `TOPIC`      | Sets or displays the topic of a channel.              | `/topic <channel> [new_topic]`   |

---

## 🚀 **Development and Testing**

- **Testing Environment**: The project was tested on **Linux** systems.  
- **Code Standard**: The project adheres to the **C++98** standard with a modular and maintainable structure.  
- **Error Handling**: All errors are reported according to the **RFC 1459** format.  

---

## 💡 **Key Learnings**

- Implementation of **socket programming** and **network communication**.  
- Development of **real-time communication systems** and protocol compliance.  
- Application of **object-oriented programming** principles in **C++98**.  

---
