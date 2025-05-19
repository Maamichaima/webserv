#include "../_includes/ServerManager.hpp"
# include "methods/GetMethod.hpp"

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
        event.events = EPOLLIN  | EPOLLET;
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
            std::cout << "    Port " << server.getPort()<< std::endl;
            std::cout << "    IP_address "<< server.getIpAddress() << std::endl;
            print_map(parameters);
        }
        server.printLocations();  
        std::cout << std::endl;
        
    }
    std::cout << "----- END SERVER INFORMATION -----" << std::endl;
}

// khass rihab txouf fin dirha
void printLocations(const Server& server) {
    for (std::map<std::string, location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
        std::cout << "Location path: " << it->second.getPath() << std::endl;

        const std::map<std::string, std::vector<std::string> >& infos = it->second.infos;
        for (std::map<std::string, std::vector<std::string> >::const_iterator info_it = infos.begin(); info_it != infos.end(); ++info_it) {
            std::cout << "  " << info_it->first << " : ";
            for (size_t i = 0; i < info_it->second.size(); ++i) {
                std::cout << info_it->second[i];
                if (i < info_it->second.size() - 1)
                    std::cout << ", ";
            }
            std::cout << std::endl;
        }
    }
}

bool ServerManager::Add_new_event(int fd_socket){
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLOUT;
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
            std::cout << client_fd << "\n";

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
        else if(events[i].events & EPOLLIN){

            bool closeConnection = false;
                
            ssize_t bytesRead = recv(currentFd, buffer, BUFFER_SIZE - 1,0);
            // std::cout << buffer << "\n";
        
            if (bytesRead < 0) {
                    perror("read failed");
                    closeConnection = true;
            }
            else if (bytesRead == 0){
                std::cout << "Client disconnected" << std::endl;
                if (epoll_ctl(epollFd, EPOLL_CTL_DEL, currentFd, nullptr) == -1) {
                    perror("epoll_ctl: EPOLL_CTL_DEL");
                // Handle error
                }
                closeConnection = true;
                break;
            }
            else{

                buffer[bytesRead] = '\0';
                // std::cout << buffer << std::endl;
                clients[currentFd].setBuffer(buffer);
                // this->servers[0].locations["/api"]; ??? !!!
                // std::cout << "Received from client " << currentFd << ": " << buffer << std::endl;
                std::memset(buffer, 0, BUFFER_SIZE);
                // string responce = handleGetRequest(clients[currentFd].data_rq, "root");
                
                // if (write(currentFd,  "buffer received \n", 18) < 0) {
                //     perror("write failed");
                //     closeConnection = true;
                //     break;
                // }
                // if(clients[currentFd].flag == 3)
                    //ghadir khdemteek  GET / gerreg
            }
                
            // if (closeConnection) {
            //     close(currentFd);
            // }
        }
        if (!clients[currentFd].checkRequestProgress())
            clients[currentFd].parseRequest();
        else if (events[i].events & EPOLLOUT)
        {
            dprintf(2, "salammmmmmmmmmmmmmmmmmmmmmmmmmm\n");
            //send responde
            // //std::cout << "sending........\n";
            std::string response;
            location* loc = getClosestLocation(servers[0], "/");
            if (loc) {
                std::cout << "Best location path: " << loc->getPath() << std::endl << endl;
                std::cout << "result: " << loc->getInfos("root")->at(0) << std::endl << std::endl;
                response = handleGetRequest(clients[currentFd].data_rq, loc->getInfos("root")->at(0));
                // std::string response = handleGetRequest(clients[currentFd].data_rq, "www");
            }

            cout << "response: "<< response << endl;
            // send(currentFd, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 11\r\n\r\nHello Amine",77, 0);
            cout << "size : "<< response.size() << endl;
            printf("%zu\n",strlen(response.c_str()));
            send(currentFd, response.c_str(), strlen(response.c_str()), 0);
        }
    }
}

// GET /tmp/index.html HTTP
