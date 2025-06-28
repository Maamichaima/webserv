#include "../_includes/ServerManager.hpp"
# include "methods/GetMethod.hpp"
#include <cstddef>
#include <ostream>

std::map<int, std::vector<Server *> > SocketToServers;

ServerManager::ServerManager(){
    epollFd = epoll_create(1024);
}
std::vector<Server>  ServerManager::get_servers(){
    return(servers);
}
std::map<int,client>  ServerManager::get_clients(){
    return(clients);
}


void ServerManager::addServer(Server& server) {
    
    servers.push_back(server);
}

bool ServerManager::initializeAll() {
    std::set<int> epollFds;
    for (size_t i = 0; i < servers.size() ; i++){
      
        if (!servers[i].initialize(this->servers, static_cast<int>(i))) 
            continue;
        std::map<std::string, Socket>& serverComb = servers[i].comb;
        for (std::map<std::string, Socket>::iterator it = serverComb.begin(); 
             it != serverComb.end(); ++it) {

            int fd = it->second.fd_socket;
            if (epollFds.find(fd) == epollFds.end())
            {    
                struct epoll_event event;
                event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP ;
                event.data.fd = fd;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) < 0) {
                    ServerLogger::serverError("epoll_ctl failed for server socket");
                    return false; 
                }
                epollFds.insert(fd);
            }
        }

    }
    if(epollFds.empty())
     {   
        cout << "no server exist " << endl;
        return false;
    }
    
    routeRequest();
    return true;
}

std::vector<int> ServerManager::getAllSocketFds() {
    std::set<int> unique_fds;
    
    for (size_t i = 0; i < servers.size(); i++) {
        std::map<std::string, Socket>& server_sockets = servers[i].comb;
        
        for (std::map<std::string, Socket>::iterator it = server_sockets.begin();
             it != server_sockets.end(); ++it) {
            
            int fd = it->second.getSocketFd(); 
            if (fd >= 0) { 
                unique_fds.insert(fd);
            }
        }
    }
    return std::vector<int>(unique_fds.begin(), unique_fds.end());
}

std::string ServerManager::findPort(int currentFd)
{
    for (size_t i = 0; i < servers.size(); i++) {
        std::map<std::string, Socket>& server_sockets = servers[i].comb;
        for (std::map<std::string, Socket>::iterator it = server_sockets.begin(); it != server_sockets.end(); ++it) {
            if (it->second.getSocketFd() == currentFd) {  // Assuming Socket has getFd() method
                return it->first;  // Return the port (key)
            }
        }   
    }
    return "";
}

bool ServerManager::Add_new_event(int fd_socket){
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLOUT |  EPOLLRDHUP | EPOLLHUP;
    event.data.fd = fd_socket;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd_socket, &event) < 0) {
        ServerLogger::serverError("epoll_ctl failed for client socket");
        close(fd_socket);
        return false;
    }
    return true;
}

void ServerManager::ClientDisconnected(int currentFd)
{

    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, currentFd, NULL) == -1) {
        ServerLogger::serverError("epoll_ctl: EPOLL_CTL_DEL");
    }
    ServerLogger::clientDisconnected();
    clients.erase(currentFd);
    close(currentFd);
}

void ServerManager::checkTimeOut() {
    time_t now = time(NULL);
    std::vector<int> timedOutClients;
    std::map<int, client>::iterator it = clients.begin(); 
    
    while(it != clients.end()) {
        int clientFd = it->first;
        client& cl = it->second;
        double elapsed = difftime(now, cl.lastActivityTime);
        ++it;
        if (elapsed >= TIMEOUT) {
            cl.data_rs.status_code = 408;
            cl.setDataResponse();
            std::string response = cl.buildResponse();
            if( send(clientFd, response.c_str(), response.size(), MSG_NOSIGNAL) == -1)
                std::cerr << "send failed" << std::endl;
            ClientDisconnected(clientFd);
        }
    }
    
}




void    ServerManager::RunServer()
{
    while(1)
    {
        char buffer[BUFFER_SIZE];
        int client_fd;
        std::vector<int> fds = getAllSocketFds();
        checkTimeOut();
        int numEvents = epoll_wait(epollFd,events,MAX_EVENTS,30);
        if(numEvents < 0){
            ServerLogger::serverError("epoll_wait failed");
        }
       
        for(int i = 0; i < numEvents; i++){
            int currentFd = events[i].data.fd; 
            if(find(fds.begin(),fds.end(),currentFd) != fds.end()){ 
                client_fd = accept(currentFd,NULL,NULL);
                if (client_fd < 0) 
                    continue;
                clients[client_fd].server_fd = currentFd;
                ServerLogger::clientConnected(findPort(currentFd)); // find server port 
                Add_new_event(client_fd);
                continue ;
            }
            else if(events[i].events & (EPOLLRDHUP | EPOLLHUP))
            {
                clients[currentFd].closeConnection = true;
            }
            else if(events[i].events & EPOLLIN){
                ssize_t bytesRead = recv(currentFd, buffer, BUFFER_SIZE ,0);
                clients[currentFd].lastActivityTime = time(NULL);
                
                if(bytesRead <= 0)
                {      
                    clients[currentFd].closeConnection = true;
                }
                else {
                    clients[currentFd].buffer.append(buffer, bytesRead);
                    std::memset(buffer, 0, BUFFER_SIZE);   
                }
            } 
            if (!clients[currentFd].checkRequestProgress())
                clients[currentFd].parseRequest();
            else if (events[i].events & EPOLLOUT) 
            {
                clients[currentFd].handleResponse(currentFd);
                clients[currentFd].closeConnection = true;
            }
            if(clients[currentFd].closeConnection)
                ClientDisconnected(currentFd);
               
        }
    }
}



Server     *chooseServer(std::vector<Server*> &routeServer,std::string host)
{
    // if(routeServer.empty())
    // {
    //     //std::cerr << "Error: No servers available for routing" << std::endl;
    //     return NULL;
    // }
    if(routeServer.size() == 1)
    {
        return routeServer[0];
    }
    std::vector<Server*>::iterator it ;
    for( it = routeServer.begin(); it != routeServer.end(); ++it){
        Server *server = *it;
        for(size_t i = 0; i < server->serverNames.size(); i++) {
            if(server->serverNames[i] == host) {
                return server;
            }
        }
    }
   
    return(routeServer[0]); // check empty vector 
}

void   ServerManager::routeRequest()
{
    
    for(std::vector<Server>::iterator serverIt = servers.begin(); 
        serverIt != servers.end(); ++serverIt)
    {
        for(std::map<std::string, Socket>::iterator combIt = serverIt->comb.begin();
            combIt != serverIt->comb.end(); ++combIt)
        {
            int fd = combIt->second.fd_socket;
            SocketToServers[fd].push_back(&(*serverIt));
        }
    }
    // std::cout << "routeServer contains " << SocketToServers[4].size() << " servers:" << std::endl;
    // for(size_t i = 0; i < SocketToServers[4].size(); ++i)
    // {
    //     std::cout << "Server[" << i << "]: " << SocketToServers[4][i] << std::endl;
    //     // If Server class has specific members you want to print, access them like:
    //     // std::cout << "Server[" << i << "] - Name: " << routeServer[i]->name << std::endl;
    // }
 
}



void ServerManager::printAllServerInfo() {

    if(servers.empty()){
        std::cout << "No servers configured." << std::endl;
        return;
    }
    std::cout << "----- SERVER INFORMATION -----" << std::endl;
    for (size_t i = 0; i < servers.size(); i++) {

        Server& server = servers[i];
        std::cout << "\n  SERVER #" << (i + 1) << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        
        // IP Address
        if (!server.ip_address.empty()) {
            std::cout << "   IP Address: " << server.ip_address << std::endl;
        } else {
            std::cout << "   IP Address: Not configured" << std::endl;
        }
        if (!server.port.empty()) {
            std::cout << "   Ports: ";
            for (size_t j = 0; j < server.port.size(); j++) {
                std::cout << server.port[j];
                if (j < server.port.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "   Ports: None configured" << std::endl;
        }

        if (!server.serverNames.empty()) {
            std::cout << "   Server Names: ";
            for (size_t j = 0; j < server.serverNames.size(); j++) {
                std::cout << server.serverNames[j];
                if (j < server.serverNames.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "   Server Names: None configured" << std::endl;
        }

        if (server.MaxBodySize != 0) {
            std::cout << "   Max Body Size: " << server.MaxBodySize << std::endl;
        } else {
            std::cout << "   Max Body Size: Default" << std::endl;
        }

        if (!server.comb.empty()) {
            std::cout << "\n SOCKET COMBINATIONS:" << std::endl;
            for (std::map<std::string, Socket>::const_iterator it = server.comb.begin(); 
                 it != server.comb.end(); ++it) {
                std::cout << "   " << it->first << " -> Socket configured" << std::endl;
            }
        }
        if (!server.errorPages.empty()) {
            std::cout << "\n ERROR PAGES:" << std::endl;
            for (std::map<std::string, std::string>::const_iterator it = server.errorPages.begin(); 
                 it != server.errorPages.end(); ++it) {
                std::cout << "   Error " << it->first << " -> " << it->second << std::endl;
            }
        } else {
            std::cout << "\n ERROR PAGES: None configured" << std::endl;
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