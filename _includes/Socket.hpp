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

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_EVENTS 64

class Epoll_events{
    
    int epollFd;
    
    public:
        struct epoll_event events[MAX_EVENTS];
        int get_epollFd(){
            return epollFd;
        }
        // struct epoll_event& get_events(){
        //     return *events;
        // }
        bool create_fd_epoll(){

            epollFd = epoll_create1(0);
             if (epollFd == -1) {
                std::cerr << "Failed to create epoll instance." << std::endl;
                return false;
            }
            return true;
        }
        bool Add_new_event(int fd_socket){
            struct epoll_event event;
            event.events = EPOLLIN | EPOLLET;
            event.data.fd = fd_socket;
            if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd_socket, &event) < 0) {
                perror("epoll_ctl failed for client socket");
                close(fd_socket);
                return false;
            }
            std::cout << "New event added : " << fd_socket << std::endl;
            return true;
        }
        
};

class Socket{

    public:


    int fd_socket; //getters setters
    struct sockaddr_in host_addr;
    struct addrinfo *host_info;
    int host_addrlen;
    Epoll_events event;

    Socket() :fd_socket(-1), host_addrlen(0) {
        host_info = NULL;
        memset(&host_addr, 0, sizeof(host_addr));
    }

    ~Socket() {
        if (fd_socket != -1) {
            close(fd_socket);
        }
    }
    int getSocketFd(){
        return(fd_socket);
    }
    void    initialize(std::string port, std::string add_ip){
        struct addrinfo hints;
        std::memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;  
        hints.ai_socktype = SOCK_STREAM;      
        hints.ai_flags = AI_PASSIVE;

    
        int status = getaddrinfo(add_ip.c_str(), port.c_str(), &hints, &host_info);
        if (status != 0) {
            std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
            exit(EXIT_FAILURE);
        }

    }

    bool create_Socket(){

        fd_socket = socket(host_info->ai_family ,host_info->ai_socktype, host_info->ai_protocol);
        if (fd_socket == -1) {
            perror("socket failed");
            return false;
        }
        int opt = 1;
        if (setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            perror("setsockopt failed");
            close(fd_socket);
            fd_socket = -1;
            return false;
        }

        std::cout << "socket created successfully\n" <<std::endl;
        return true;
    }

    bool   bind_Socket(){
           
        if (bind(fd_socket, host_info->ai_addr, host_info->ai_addrlen) < 0) {
            perror("bind failed");
            close(fd_socket);
            return false;
        }
        std::cout << "socket successfully bound to address\n" << std::endl;
        return true;
    }

    bool    listen_socket(){

        if (listen(fd_socket, SOMAXCONN) != 0) {
            perror("webserver (listen)");
            return false;
        }
        std::cout << "server listening for connections\n"<<std::endl;
        return true;
    }

    bool set_nonblocking()
    {
        if (fcntl(fd_socket, F_SETFL, O_NONBLOCK) == -1) {
            // perror("fcntl F_SETFL O_NONBLOCK");
            return false;
        }
        return true;
    }

};


// setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

#endif