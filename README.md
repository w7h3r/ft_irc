 > *This project has been created as part of the 42 curriculum by alermi, oozsipah, muokcan.*


## Description

This project is an introduction to socket programming and network communication using the C programming language. 


The main goal of the project is to understand how two independent processes can communicate with each other over a network by using sockets. A socket acts as an endpoint of a two-way communication channel and allows a client and a server to exchange data.


The project focuses on the fundamentals of the client-server architecture and the main system calls used to establish and manage network connections.


A typical TCP socket communication follows these steps:


```text

                 TCP CONNECTION


        +-------------------+

        |       SERVER      |

        |                   |

        | socket()          |

        | bind()            |

        | listen()          |

        | accept()          |

        +---------+---------+

                  |

                  | Connection

                  |

        +---------v---------+

        |       CLIENT      |

        |                   |

        | socket()          |

        | connect()         |

        | send()/recv()     |

        +-------------------+

```


## Socket Types

Sockets can be used with different communication protocols. The two main types are:


### Stream Sockets — TCP

TCP sockets provide:

* Connection-oriented communication.

* Reliable data transmission.

* Ordered delivery of data.

* Error detection and retransmission mechanisms.


*TCP is commonly used by applications such as web servers, file transfers and many client-server applications.*


### Datagram Sockets — UDP

UDP sockets provide:

* Connectionless communication.

* Lower communication overhead.

* No guarantee of delivery.

* No guarantee that packets arrive in order.


*UDP is commonly used when speed is more important than guaranteed delivery, such as some real-time communication and streaming applications.*


## Main Socket Functions


| Function | Purpose |

| :--- | :--- |

| `socket()` | Creates a new socket |

| `setsockopt()` | Configures socket options |

| `bind()` | Associates a socket with an address and port |

| `listen()` | Places a server socket into listening mode |

| `accept()` | Accepts an incoming client connection |

| `connect()` | Connects a client socket to a server |

| `send()` | Sends data through a socket |

| `recv()` | Receives data from a socket |

| `read()` | Reads data from a socket |

| `write()` | Writes data to a socket |

| `close()` | Closes a socket and releases resources |


## Client-Server Model

The project is based on the client-server communication model.


The server creates a listening socket and waits for clients. Once a client connects, the server can receive and send information through the connected socket. The client creates a socket and initiates the connection to the server using the server's IP address and port number.


For example:

`127.0.0.1:8080`


Where:

* `127.0.0.1` represents the local machine.

* `8080` represents the network port.


Once the connection is established, both processes can exchange messages.


## Technical Concepts

This project provides practical experience with several important networking concepts:


* IPv4 addressing with `AF_INET`.

* TCP communication with `SOCK_STREAM`.

* Socket file descriptors.

* Network byte order and `htons()`.

* `struct sockaddr_in`.

* Server-side socket initialization.

* Client-side socket connection.

* Blocking socket behavior.

* Sending and receiving data.

* Connection management.

* Error handling.

* Resource management.


## Instructions


### Requirements

The project is intended for a Unix/Linux environment and requires:

* A C compiler such as `gcc` or `cc`.

* POSIX-compatible socket APIs.

* Standard Unix system calls and networking headers.


The socket API used by the project includes:

```c

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <unistd.h>

```


"### Compilation\n"

"If the project provides a Makefile, compile it from the root directory with:\n"

```bash

make

```


If the source files are compiled directly, a typical compilation command is:

```bash

gcc server.c -o server

gcc client.c -o client

```


For C++ implementations, the equivalent command can be:

```bash

c++ server.cpp -o server

c++ client.cpp -o client

```

*The exact compilation command should follow the project's provided Makefile and source-file structure.*


### Execution

Start the server first:

```bash

./server

```

Then, from another terminal, start the client:

```bash

./client

```


The server must be running and listening on the configured port before the client attempts to connect. For local testing, the server can use `127.0.0.1` with a configured port such as `8080`.


### Example Communication

A simple client/server exchange can look like:

```text

Client: Hello from client

Server: Hello from server

```

The server waits for a connection, accepts the client, receives its message and can respond. The client connects to the server, sends its message and waits for the response.


## Common Issues


### Connection Failed

Make sure:

* The server is running.

* The client uses the correct IP address.

* The client uses the correct port.

* No firewall or network configuration is blocking the connection.


### Address Already in Use

A port may still be associated with another process or a recently closed connection. Using `setsockopt()` with appropriate socket options such as `SO_REUSEADDR` can help when restarting a server.


### Blocking Calls

By default, sockets are generally blocking. Calls such as:

```c

accept();

recv();

read();

```

may wait until a connection or data becomes available. This behavior should be taken into account when designing the communication flow.


### Resource Management

Every socket created by `socket()` should eventually be closed with:

```c

close(sockfd);

```

Properly closing file descriptors prevents resource leaks and keeps the program's network resources under control.


## Project Structure

The exact structure depends on the implementation, but a typical project can be organized as follows:

```text


├── README.md

├── Makefile

├── include

│   └── ...

├── src/

│   ├── ...

│   └── ...

└── ...

```


## Resources

The following resources were used to understand the concepts behind socket programming and the APIs used in the project:

* **Linux man pages**: `man socket`, `man bind`, `man listen`, `man accept`, `man connect`, `man send`, `man recv`, `man close`

* **Beej's Guide to Network Programming**: A practical introduction to network programming and sockets in C.

* **Linux Programmer's Manual**: Useful for understanding POSIX system calls, file descriptors and networking APIs.

* **GNU C Library documentation**: Reference material for C programming and standard library functionality.

* **POSIX Socket API documentation**: Reference for the standard socket functions and structures used by Unix/Linux systems.


## AI Usage\n"

AI tools were used as a supporting resource during the project. The main uses of AI were:

* Helping organize and structure the `README.md`.

* Explaining socket programming concepts such as `socket()`, `bind()`, `listen()`, `accept()` and `connect()`.

* Clarifying the difference between TCP/stream sockets and UDP/datagram sockets.

* Helping identify and explain common socket programming errors.

* Improving the readability and organization of the project's documentation.

* Reviewing explanations and terminology used in the documentation.


AI was not used as a replacement for understanding or implementing the project's core requirements. The project code, architecture and implementation decisions were developed and reviewed by the project authors.*


## Learning Objectives\n"

Through this project, we aim to develop a practical understanding of:

* How network communication works at the application level.

* How client-server architectures are implemented.

* How sockets are created and managed.

* How TCP connections are established.

* How processes exchange data over sockets.

* How IP addresses and ports identify communication endpoints.

* How to handle network-related errors.

* How to correctly manage system resources.


## Authors

* **alermi**

* **oozsipah**

* **muokcan** 
