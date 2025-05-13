#ifndef SOCKET_HPP
#define SOCKET_HPP
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include "parsing/tokenizer.hpp"

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_EVENTS 64
class Socket{

    public:


    int fd_socket; //getters setters
    struct sockaddr_in host_addr;
    int host_addrlen;


    Socket(){}
    void    initialize(){

        memset(&host_addr, 0, sizeof(host_addr));
        host_addr.sin_family = AF_INET;
        host_addr.sin_addr.s_addr = htonl(INADDR_ANY);//getaddrinfo()
        host_addr.sin_port = htons(PORT); // convert from host byte order to network bytes order 
        host_addrlen = sizeof(host_addr);
    }
    int create_Socket(){

        fd_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_socket == 0) {
            perror("socket failed");
            // exit(EXIT_FAILURE);
        }
        std::cout << "socket created successfully\n" <<std::endl;
        return fd_socket;
    }
    void    bind_Socket(){
           
      // on success it returs 0
        if (bind(fd_socket, (struct sockaddr *)&host_addr, host_addrlen) < 0) {
          perror("bind failed");
        //   exit(EXIT_FAILURE);
        }
        std::cout << "socket successfully bound to address\n" << std::endl;
    }

    void    listen_socket(){

        if (listen(fd_socket, SOMAXCONN) != 0) {
            perror("webserver (listen)");
            // exit(EXIT_FAILURE);
        }
        std::cout << "server listening for connections\n"<<std::endl;
    }

    void update_Socket()
    {
        if (fcntl(fd_socket, F_SETFL, O_NONBLOCK) == -1) {
            perror("fcntl F_SETFL O_NONBLOCK");
            //exit(EXIT_FAILURE);
        }
        
    }
};


// setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

#endif