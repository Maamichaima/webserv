#include "Socket.hpp"

void update_Socket(int fd_socket)
{
    if (fcntl(fd_socket, F_SETFL, O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL O_NONBLOCK");
       // exit(EXIT_FAILURE);
    }
    
}

int main()
{
    
    Socket socket;
    int new_socket;
    char buffer[BUFFER_SIZE];
    char resp[] = "HTTP/1.0 200 OK\r\n"
                  "Server: webserver-c\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "<html>hello, world</html>\r\n";
    socket.initialize();
    socket.create_Socket();
    socket.bind_Socket();
    socket.listen_socket();
    socket.update_Socket();
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    //create epoll instance 
    int epollFd = epoll_create1(0);
    if (epollFd == -1) {
        std::cerr << "Failed to create epoll instance." << std::endl;
        close(socket.fd_socket);
        return 1;
    }

    //add server socket to epoll
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = socket.fd_socket;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, socket.fd_socket, &event) < 0) {
        perror("epoll_ctl failed for server socket");
        //exit(EXIT_FAILURE);
    }
    struct epoll_event events[MAX_EVENTS];




    while(1)
    {
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (numEvents < 0) {
            perror("epoll_wait failed");
            //exit(EXIT_FAILURE);
        }

        

        for(int i = 0; i < numEvents; i++){
            int currentFd = events[i].data.fd;

            if(currentFd == socket.fd_socket){
                while(1){
                    new_socket = accept(socket.fd_socket, (struct sockaddr *)&socket.host_addr,(socklen_t *)&socket.host_addrlen);
                    if (new_socket < 0) {
                        perror("accept failed");
                        break;
                    }
                    std::cout << "connection accepted from client " <<std::endl;


                    update_Socket(new_socket);

                    struct epoll_event clientEvent;
                    clientEvent.events = EPOLLIN | EPOLLET;  
                    clientEvent.data.fd = new_socket;
                    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, new_socket, &clientEvent) < 0) {
                        perror("epoll_ctl failed for client socket");
                        close(new_socket);
                    } else {
                        std::cout << "New client connected: " << new_socket << std::endl;
                    }
                }
            }
            else if(events[i].events & EPOLLIN){
                
                bool closeConnection = false;
                while(1){

                    ssize_t bytesRead = read(currentFd, buffer, BUFFER_SIZE - 1);
                    if (bytesRead < 0) {
                        perror("read failed");
                        closeConnection = true;
                        break;
                    }
                    else if (bytesRead == 0){
                        closeConnection = true;
                        break;
                    }
                    else{

                        buffer[bytesRead] = '\0';
                        std::cout << "Received from client " << currentFd << ": " << buffer << std::endl;
                        if (write(currentFd,  resp, strlen(resp)) < 0) {
                            perror("write failed");
                            closeConnection = true;
                            break;
                        }
                    }

                }  
            }
        }





















        
        //get client infos(ip addr / port)
        // int sockn = getsockname(new_socket, (struct sockaddr *)&client_addr,
        //                           (socklen_t *)&client_addrlen);
        // if (sockn < 0) {
        //     perror("webserver (getsockname)");
        //     continue;
        // }
        // std::cout << "IP address : " << inet_ntoa(client_addr.sin_addr) << "  Port : " << ntohs(client_addr.sin_port) << std::endl;
        
        // //read data from client 
        // int valread = read(new_socket, buffer, BUFFER_SIZE);
        // if (valread < 0) {
        //     perror("webserver (read)");
        //     continue;
        // }
        // else
        //     std::cout << "send data ====> " << buffer <<  std::endl;



        // // write data to client 
        // int valwrite = write(new_socket, resp, strlen(resp));
        // if (valwrite < 0) {
        //     perror("webserver (write)");
        //     continue;
        // }
     
    }
    


    
   




  // Close socket
  close(new_socket);
  close(socket.fd_socket);
  return 0;

    

}

