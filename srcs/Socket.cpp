#include "../_includes/Socket.hpp"
#include <cstddef>

Socket::Socket() :fd_socket(-1) {
    host_info = NULL;
    memset(&host_addr, 0, sizeof(host_addr));
}

Socket::~Socket() {
	// std::cout << "========"<< fd_socket << "=========\n";
    // close(fd_socket);
}

int Socket::getSocketFd() {
    return(fd_socket);
}

bool    Socket::initialize(std::string port, std::string add_ip){
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(add_ip.c_str(), port.c_str(), &hints, &host_info);
    if (status != 0) 
        return false;
    return true;
}

bool Socket::create_Socket(){

    fd_socket = socket(host_info->ai_family ,host_info->ai_socktype, host_info->ai_protocol);
    if (fd_socket == -1) {
        return false;
    }
    int opt = 1;
    if (setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)) < 0) {
        close(fd_socket);
        fd_socket = -1;
        return false;
    }
    return true;
}

bool   Socket::bind_Socket(){
    
    if (bind(fd_socket, host_info->ai_addr, host_info->ai_addrlen) < 0) {
        close(fd_socket);
        fd_socket = -1;
        freeaddrinfo(host_info);
        return false;
    }
    freeaddrinfo(host_info);
    return true;
}

bool    Socket::listen_socket(){

    if (listen(fd_socket, SOMAXCONN) != 0) {
        return false;
    }
    return true;
}

