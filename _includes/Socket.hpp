#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <cstddef>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstdio>
#include <algorithm>
#include <sys/types.h>
#include <netdb.h>



#define BUFFER_SIZE 4024
#define MAX_EVENTS 64


class Socket{

    public:
        int                fd_socket;
        struct sockaddr_in host_addr;
        struct addrinfo   *host_info;

        Socket();
        int     getSocketFd();
        bool    initialize(std::string port, std::string add_ip);
        bool    create_Socket();
        bool    bind_Socket();
        bool    listen_socket();
        ~Socket();
};




#endif