
#include "../_includes/Socket.hpp"

Socket::Socket() :fd_socket(-1), host_addrlen(0) {
    host_info = NULL;
    memset(&host_addr, 0, sizeof(host_addr));
}

Socket::~Socket() {
    if (fd_socket != -1) {
        close(fd_socket);
    }
}
int Socket::getSocketFd(){
    return(fd_socket);
}
void    Socket::initialize(std::string port, std::string add_ip){
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

bool Socket::create_Socket(){

    fd_socket = socket(host_info->ai_family ,host_info->ai_socktype, host_info->ai_protocol);
    if (fd_socket == -1) {
        perror("socket failed");
        return false;
    }
    int opt = 1;
    if (setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(fd_socket);
        fd_socket = -1;
        return false;
    }
    if (setsockopt(fd_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEPORT failed");
        close(fd_socket);
        fd_socket = -1;
        return false;
    }
    std::cout << "socket created successfully\n" <<std::endl;
    return true;
}

bool   Socket::bind_Socket(){
       
    if (bind(fd_socket, host_info->ai_addr, host_info->ai_addrlen) < 0) {
        perror("bind failed");
        close(fd_socket);
        return false;
    }
    std::cout << "socket successfully bound to address\n" << std::endl;
    return true;
}

bool    Socket::listen_socket(){

    if (listen(fd_socket, SOMAXCONN) != 0) {
        perror("webserver (listen)");
        return false;
    }
    std::cout << "server listening for connections\n"<<std::endl;
    return true;
}

bool Socket::set_nonblocking()
{
    if (fcntl(fd_socket, F_SETFL, O_NONBLOCK) == -1) {
        // perror("fcntl F_SETFL O_NONBLOCK");
        return false;
    }
    return true;
}
