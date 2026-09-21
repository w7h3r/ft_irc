*This project has been created as part of the 42 curriculum by alermi, oozsipah, muokcan.*

# ft_irc — Internet Relay Chat Server

## Description

**ft_irc** is an IRC (Internet Relay Chat) server written in **C++98**. IRC is a text-based, real-time communication protocol in which clients connect to a server to exchange private messages and to talk in group channels.

The goal of the project is to build a working IRC server from scratch, using only low-level POSIX socket APIs, that a real IRC client can connect to and use as it would any official IRC server. Along the way, the project covers:

- TCP/IP socket programming (`socket`, `bind`, `listen`, `accept`, `send`, `recv`).
- Handling many clients at once in a **single process**, with **non-blocking I/O** and a **single `poll()` call**. No forking, and no threads.
- Parsing a text-based network protocol (IRC messages terminated by `\r\n`), including reassembling commands that arrive in several partial packets.
- Managing users, channels, operator privileges and channel modes.

Server-to-server communication is **not** implemented (it is out of the project's scope), and no IRC client is included. A standard existing client is used to connect.

### Features

- Connection authentication with a server password (`PASS`).
- Nickname and username registration (`NICK`, `USER`).
- Joining and leaving channels (`JOIN`, `PART`).
- Private messages between users and messages to channels (`PRIVMSG`). A channel message is forwarded to every other member of the channel.
- Channel operators and regular users.
- Operator commands: `KICK`, `INVITE`, `TOPIC`, `MODE`.
- Channel modes: `i`, `t`, `k`, `o`, `l`.
- Clean handling of disconnections (`QUIT`, or a connection closed by the client).

## Instructions

### Requirements

- A Unix-like system (Linux or macOS).
- A C++ compiler that supports the C++98 standard (`c++`).
- `make`.
- An IRC client for testing (our reference client is **hexchat**), and optionally `nc` (netcat).

### Compilation

From the root of the repository, run:

```bash
make
```

This produces the `ircserv` executable. The project is compiled with:

```bash
c++ -Wall -Wextra -Werror -std=c++98
```

The other Makefile rules are:

| Rule          | Effect                                         |
| :------------ | :--------------------------------------------- |
| `make` / `all`| Builds `ircserv`                               |
| `make clean`  | Removes object files                           |
| `make fclean` | Removes object files and the `ircserv` binary  |
| `make re`     | Runs `fclean`, then `all`                      |

### Execution

```bash
./ircserv <port> <password>
```

- `port`: the TCP port the server listens on (for example `6667`, the standard IRC port).
- `password`: the password every client must provide in order to connect.

For example:

```bash
./ircserv 6667 secret
```

### Connecting with the reference client (irssi)

```bash
irssi
```

Then, inside irssi:

```text
/connect 127.0.0.1 6667 secret mynick
/join #general
/msg #general Hello everyone!
/msg othernick Hi, this is a private message
```

### Connecting with netcat

`nc` can be used to send raw IRC commands. The `-C` flag makes it send `\r\n` line endings, as the IRC protocol requires.

```bash
nc -C 127.0.0.1 6667
PASS secret
NICK alice
USER alice 0 * :Alice
JOIN #general
PRIVMSG #general :Hello!
```

If registration succeeds, the server replies with the welcome message (`001 RPL_WELCOME`).

### Testing partial data

The server buffers incoming data per client and only processes a command once a complete line has arrived. To check this, connect with `nc -C 127.0.0.1 6667` and type a command in pieces, pressing `Ctrl+D` after each piece:

```text
com^Dman^Dd
```

The server receives `com`, then `man`, then `d\r\n`, and processes them as the single command `command`.

## Supported Commands

| Command   | Usage                                   | Description                                             |
| :-------- | :-------------------------------------- | :------------------------------------------------------ |
| `PASS`    | `PASS <password>`                       | Authenticates with the server password (sent first)     |
| `NICK`    | `NICK <nickname>`                       | Sets or changes the nickname                            |
| `USER`    | `USER <username> 0 * :<realname>`       | Sets the username and real name                         |
| `JOIN`    | `JOIN <#channel> [key]`                 | Joins a channel, creating it if it does not exist       |
| `PART`    | `PART <#channel> [:reason]`             | Leaves a channel                                        |
| `PRIVMSG` | `PRIVMSG <target> :<message>`           | Sends a message to a user or a channel                  |
| `KICK`    | `KICK <#channel> <nick> [:reason]`      | Ejects a user from a channel *(operator only)*          |
| `INVITE`  | `INVITE <nick> <#channel>`              | Invites a user to a channel                             |
| `TOPIC`   | `TOPIC <#channel> [:new topic]`         | Shows or changes the channel topic                      |
| `MODE`    | `MODE <#channel> <+/-modes> [params]`   | Changes the channel modes *(operator only)*             |
| `PING`    | `PING <token>`                          | Keep-alive check; the server answers with `PONG`        |
| `QUIT`    | `QUIT [:message]`                       | Disconnects from the server                             |

### Channel Modes

| Mode | Parameter  | Description                                                      |
| :--- | :--------- | :--------------------------------------------------------------- |
| `i`  | —          | Invite-only channel: users can only join if they were invited    |
| `t`  | —          | Only channel operators can change the topic                      |
| `k`  | `<key>`    | Sets or removes the channel key (password)                       |
| `o`  | `<nick>`   | Gives or takes channel operator privilege                        |
| `l`  | `<limit>`  | Sets or removes the maximum number of users in the channel       |

The first user to join a channel automatically becomes its operator.

## Technical Choices

- **Single event loop.** All file descriptors (the listening socket and every client socket) are watched by **one** `poll()` call. Accepting new connections, reading and writing all go through this loop. `recv()` and `send()` are only called on a descriptor after `poll()` has reported it ready.
- **Non-blocking sockets.** Every socket is switched to non-blocking mode with `fcntl(fd, F_SETFL, O_NONBLOCK)`, so no single client can block the server.
- **No `errno` after I/O.** The server never inspects `errno` after `recv()` or `send()` to decide what to do next. Decisions rely only on the return value and on the events reported by `poll()`.
- **Per-client input buffer.** Received bytes are appended to the client's buffer. Complete lines (ending in `\r\n`, with plain `\n` also accepted) are extracted and parsed one at a time, and any incomplete remainder stays in the buffer until more data arrives.
- **Per-client output buffer.** Replies are queued in an output buffer and sent when `poll()` reports the socket as writable (`POLLOUT`), which handles slow clients and partial sends.
- **Clean disconnection.** When a client disconnects, its socket is closed, it is removed from every channel it belonged to, and empty channels are deleted.
- **Replies** follow the numeric reply format of RFC 2812 (for example `001 RPL_WELCOME`, `433 ERR_NICKNAMEINUSE`, `482 ERR_CHANOPRIVSNEEDED`), so standard clients can interpret them.

## Project Structure

```text
.
├── Makefile
├── README.md
├── include/
│   ├── Server.hpp
│   ├── Client.hpp
│   ├── Channel.hpp
│   └── ...
└── src/
    ├── main.cpp
    ├── Server.cpp
    ├── Client.cpp
    ├── Channel.cpp
    ├── commands/
    │   └── ...
    └── ...
```

## Resources

### IRC protocol

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2810 — IRC: Architecture](https://datatracker.ietf.org/doc/html/rfc2810)
- [RFC 2811 — IRC: Channel Management](https://datatracker.ietf.org/doc/html/rfc2811)
- [RFC 2812 — IRC: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/)
- [irssi documentation](https://irssi.org/documentation/)

### Network programming

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- Linux man pages: `socket(2)`, `bind(2)`, `listen(2)`, `accept(2)`, `recv(2)`, `send(2)`, `poll(2)`, `fcntl(2)`, `setsockopt(2)`, `close(2)`

### AI Usage

AI tools were used as a supporting resource during the project, for the following tasks:

- **Documentation:** organizing, structuring and proofreading this `README.md`.
- **Learning concepts:** explanations of socket programming (`socket()`, `bind()`, `listen()`, `accept()`), of non-blocking I/O with `poll()`, and of the IRC message format described in the RFCs.
- **Debugging help:** explanations of common socket programming errors (for example `Address already in use` and `SO_REUSEADDR`).

AI was not used as a replacement for understanding or implementing the project's core requirements. The server architecture, the command handling and the implementation decisions were written, tested and reviewed by the project authors, who can explain every part of the code.

## Authors

- **alermi**
- **oozsipah**
- **muokcan**
