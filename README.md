# FT_IRC

## What is a socket?
Socket Programming is a method to connect two nodes over a network to establish a means of communication between those two nodes. A node represents a computer or a physical device with an internet connection. A socket is the endpoint used for connecting to a node. The signals required to implement the connection between two nodes are sent and received using the sockets on each node respectively.

- The nodes are divided into two types, server node and client node.
- The client node sends the connection signal and the server node receives the connection signal sent by the client node.
- The connection between a server and client node is established using the socket over the transport layer of the internet.
- After a connection has been established, the client and server nodes can share information between them using the read and write commands.
- After sharing of information is done, the nodes terminate the connection.


### There are 2 programs that should be made
![image](https://github.com/0bada1/ft_IRC/assets/95438648/37c49f36-5cc8-404c-925c-4c803208159d)
Note that using poll is more efficient than a simple read/write method. Read and write takes up a lot of resources, so using poll is required.

## 1) Building The Server
### A. Socket Creation
### B. Setsockopt
### C. Bind
### D. Listen
### E. Accept

## 2) Building The Client
### A. Socket Connection
### B. Connect

------------------------------------------------------------------------------------------------------

### All functions in the subject
| Function | Description| Parameters
| -------- | -------- | ------- |
| int socket(int domain, int type, int protocol); | Provides a method of communication between processes on the same device (or different devices over a network) | domain: Specifies the communication domain, such as AF_INET for IPv4 or AF_INET6 for IPv6. <br> type: Specifies the socket type, such as SOCK_STREAM for stream (TCP) sockets or SOCK_DGRAM for datagram (UDP) sockets. <br> protocol: Specifies the protocol to be used, usually set to 0 to let the system choose the default protocol for the specified socket type. |
| int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen); | Used to set options on a socket |  |
| int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen); | Used to retrieve the local endpoint(IP address and port) to which a socket is bound |  |
| struct protoent *getprotobyname(const char *name); | Used to retrieve protocol information like the protocol number based on the protocol name |  |
| struct hostent *gethostbyname(const char *name); | Used to obtain the IP address of a host(computer) based on its name |  |
| int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res); | Used for translating hostnames and service names into socket addresses |  |
| void freeaddrinfo(struct addrinfo *res); | Used to free memory allocated by the getaddrinfo() function |  |
| int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); | Used to associate a socket with a specific local address and port. It is a crucial step in the process of setting up a server, where the server socket needs to be assigned a unique address and port number on the local machine | sockfd: The socket file descriptor that will be bound to the specified address. <br>  addr: A pointer to a struct sockaddr structure containing the local address information. <br> addrlen: The size of the addr structure |
| int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen); | Used to establish a connection to a remote server from a client socket. It is a key step in the process of initiating a network connection in a client-server model. The connect() function is typically used with sockets of type SOCK_STREAM (e.g., TCP sockets) | |
| int listen(int sockfd, int backlog); | Used in server-side socket programming to indicate that a socket should be used for accepting incoming connections. After a socket is created using socket() and bound to a local address using bind(), the listen() function is called to set the socket into a passive mode, allowing it to accept incoming connection request | |
| int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen); | Used in server-side socket programming to accept an incoming connection request and create a new socket for communication with the client. It is typically called after the server socket has been set to listen for incoming connections using the listen() function | |
| uint16_t htons(uint16_t hostshort); | Used to convert a 16-bit unsigned integer (represented in host byte order) to network byte order (big-endian). The term "byte order" refers to the way in which multibyte data is represented in computer memory, and it can be either little-endian or big-endian |
| uint32_t htonl(uint32_t hostlong); | Used to convert a 32-bit unsigned integer (represented in host byte order) to network byte order (big-endian). Like htons() for 16-bit integers, htonl() is used in network programming to ensure consistent byte order when exchanging data between systems with potentially different native byte orders |
| uint16_t ntohs(uint16_t netshort); | Used to convert a 16-bit unsigned integer from network byte order (big-endian) to host byte order. This function is part of the Berkeley Sockets API and is commonly used in network programming when dealing with port numbers |
| uint32_t ntohl(uint32_t netlong); | Used to convert a 32-bit unsigned integer from network byte order (big-endian) to host byte order. Similar to ntohs() for 16-bit integers, ntohl() is part of the Berkeley Sockets API and is commonly used in network programming to handle 32-bit values in network packet headers |
| in_addr_t inet_addr(const char *cp); | Used to convert a string containing an IPv4 address in dotted-decimal format to a 32-bit binary representation in network byte order (big-endian). This function is part of the Berkeley Sockets API and is commonly used in network programming when dealing with IPv4 addresses |
| char *inet_ntoa(struct in_addr in); | Used to convert a 32-bit binary representation of an IPv4 address in network byte order to a string in dotted-decimal format. This function is part of the Berkeley Sockets API and is commonly used in network programming when dealing with IPv4 addresses |
| ssize_t send(int sockfd, const void *buf, size_t len, int flags); | Used to transmit data over a connected socket. It is part of the Berkeley Sockets API and is commonly used in network programming to send data from one endpoint (socket) to another in a TCP (Transmission Control Protocol) or other stream-oriented communication |
| ssize_t recv(int sockfd, void *buf, size_t len, int flags); | Used to receive data from a connected socket. It is part of the Berkeley Sockets API and is commonly used in network programming to receive data from one endpoint (socket) to another in a TCP (Transmission Control Protocol) or other stream-oriented communication |
| sighandler_t signal(int signum, sighandler_t handler); | Used to set a signal handler function for a specific signal. Signals are software interrupts that can be sent to a running process to notify it about specific events, such as user interrupts (Ctrl+C), segmentation faults, or other exceptional conditions |
| int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact); | Used to set or examine the action that the program will take when a specific signal is received. This function is part of the signal handling mechanism provided by the operating system, allowing a program to define custom actions or behaviors in response to signals |
| off_t lseek(int fd, off_t offset, int whence); | Used for positioning the file pointer within a file. It allows you to move the read/write pointer to a specified position in a file |
| int fstat(int fileDescriptor, struct stat *buf); | Used to retrieve information about an open file, including its size, permissions, and other file-related attributes |
| int fcntl(int fd, int cmd, ... /* arg */ ); | Used to perform various operations on open file descriptors. It provides a flexible and extensible interface for controlling various aspects of file descriptors, such as file status flags, file descriptor duplication, and more |
| int poll(struct pollfd fds[], nfds_t nfds, int timeout); | Used for monitoring multiple file descriptors to see if they are ready for I/O operations. It allows a program to efficiently wait for events on multiple file descriptors, such as incoming data on a socket or a change in the status of a file |
| int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout); | Used for monitoring multiple file descriptors to see if they are ready for I/O operations. Similar to poll(), select() allows a program to efficiently wait for events on multiple file descriptors, such as incoming data on a socket or a change in the status of a file |
| int kqueue(void); | Used for multiplexing I/O events. It allows a program to efficiently monitor multiple file descriptors for various events, such as data readiness, connection establishment, or disconnection |
| epoll has multiple versions | System call in Linux for monitoring multiple file descriptors to see if they are ready for I/O operations. It is designed to be more scalable and efficient than traditional mechanisms such as select() and poll(). epoll() is commonly used for high-performance event-driven programming |


## Resources
- https://www.geeksforgeeks.org/socket-programming-cc/ (Socket Programming)
- https://www.scaler.com/topics/socket-programming-in-c/ (Socket Programming)


### Created by: [Abdul Aziz](https://www.github.com/Ael-masr), Mutasem, [Obada Outabachi](https://www.github.com/0bada1)
