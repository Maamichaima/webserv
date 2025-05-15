#include "ServerManager.hpp"
#include "parsing/Server.hpp"
size_t ServerManager::numServer = 0;



ServerManager::ServerManager(){
    epollFd = epoll_create1(0);
    numServer++;
}
std::vector<Server>  ServerManager::get_servers(){
    return(servers);
}
std::map<int,client>  ServerManager::get_clients(){
    return(clients);
}

size_t  ServerManager::getNumServer(){
    return numServer;
}
void ServerManager::addServer(Server& server) {
    
    servers.push_back(server);
}

bool ServerManager::initializeAll() {
    for (size_t i = 0; i < servers.size() ; i++){
        if (!servers[i].initialize()) {
            std::cerr << "Failed to initialize a server" << std::endl;
            return false;
        }
        struct epoll_event event;
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = servers[i].getSocketFd();
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, servers[i].getSocketFd(), &event) < 0) {
            perror("epoll_ctl failed for client socket");
            close(servers[i].getSocketFd());
            return false;
        }
        std::cout << "New event added : " << servers[i].getSocketFd() << std::endl;
    }
    return true;
}

std::vector<int> ServerManager::getAllSocketFds() {
    std::vector<int> fds;
    for (size_t i = 0; i < servers.size() ; i++) {
        fds.push_back(servers[i].getSocketFd());
    }
    return fds;
}
Server* ServerManager::findServerByFd(int fd) {
    for (size_t i = 0; i < servers.size() ; i++) {
        if (servers[i].getSocketFd() == fd) {
            return &servers[i];
        }
    }
    return NULL;
}
void ServerManager::printAllServerInfo() {
    if(servers.empty()){
        std::cout << "No servers configured." << std::endl;
        return;
    }

    std::cout << "----- SERVER INFORMATION -----" << std::endl;

    for (size_t i = 0; i < servers.size(); i++) {

        Server& server = servers[i];
        std::cout << "Server #" << (i + 1) << ":" << std::endl;

        std::map<std::string, std::vector<std::string> > parameters = server.getParameters();
        std::cout <<  "  Parameters:" << std::endl;

        if (parameters.empty()) {
            std::cout << "    None" << std::endl;
        } else{
            print_map(parameters);
        }

        std::vector<location> locations = server.getLocations();
        std::cout << "  Locations:" << std::endl;
        if(locations.empty()){
            std::cout << "    None" << std::endl;
        }
        else {
            for (size_t j = 0; j < locations.size(); j++) {
               std::cout << "location #" << j << "  ==>path " << locations[j].path << std::endl;
               print_map(locations[j].infos);
            }
        
        }
        std::cout << std::endl;
        
    }

    std::cout << "----- END SERVER INFORMATION -----" << std::endl;
}

bool ServerManager::Add_new_event(int fd_socket){
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET | EPOLLOUT;
    event.data.fd = fd_socket;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd_socket, &event) < 0) {
        perror("epoll_ctl failed for client socket");
        close(fd_socket);
        return false;
    }
    std::cout << "New event added : " << fd_socket << std::endl;
    return true;
}

void    ServerManager::handle_cnx()
{

    char buffer[BUFFER_SIZE];
    std::vector<int> fds = getAllSocketFds();
    std::vector<int>::iterator it;
    int numEvents = epoll_wait(epollFd,events,MAX_EVENTS,30);
    if(numEvents < 0){
        std::cerr <<"epoll_wait failed" << std::endl; 
    }

for(int i = 0; i < numEvents; i++){
    
    int currentFd = events[i].data.fd; 
    it = std::find(fds.begin(),fds.end(),currentFd);
    if(it != fds.end()){ // it can be more than one socket in the server // 
        
        Server *server = findServerByFd(*it);
        int client_fd = accept(server->getSocketFd(),(server->getSocket().host_info->ai_addr),&server->getSocket().host_info->ai_addrlen);

        if (client_fd < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cerr << "accept failed" << std::endl;
            }
            continue;
        }
        clients[client_fd];
        clients[client_fd].server_fd = server->getSocketFd();
        std::cout << "connection accepted from client " <<std::endl;
        if(!Add_new_event(client_fd))
            continue;
           
    }
    else if(events[i].events & EPOLLIN || events[i].events & EPOLLOUT){

        bool closeConnection = false;
            
            ssize_t bytesRead = recv(currentFd, buffer, BUFFER_SIZE - 1,0);
            // std::cout << buffer << "\n";
           
            if (bytesRead < 0) {
                
                
                    perror("read failed");
                    closeConnection = true;
            }
            else if (bytesRead == 0){
                std::cout << "Client disconnected" << std::endl;
                closeConnection = true;
                break;
            }
            else{

                buffer[bytesRead] = '\0';
                clients[currentFd].setBuffer(buffer);
                clients[currentFd].parseRequest();
                // std::cout << "Received from client " << currentFd << ": " << buffer << std::endl;
                std::memset(buffer, 0, BUFFER_SIZE);
                // if (write(currentFd,  "buffer received \n", 18) < 0) {
                //     perror("write failed");
                //     closeConnection = true;
                //     break;
                // }
                // if(clients[currentFd].flag == 3)
                    //ghadir khdemteek 
            }
                
            // if (closeConnection) {
            //     close(currentFd);
            // }
    }
}
}