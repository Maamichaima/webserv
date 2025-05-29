#include "../_includes/ServerManager.hpp"
# include "methods/GetMethod.hpp"
#include <cstddef>
#include <ostream>

size_t ServerManager::numServer = 0;
int flll = 1;
std::map<int, std::vector<Server *>> SocketToServers;

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

bool check_multiple_port(std::vector<Server> servers ,int i){

    std::map<std::string,Socket>::iterator pt1 = servers[i].comb.begin();
    std::vector<Server>::iterator  it = servers.begin();
    while(it != servers.end()){

        std::map<std::string,Socket>::iterator pt2 = it->comb.begin();
        while(pt2 != it->comb.end())
        {
            while (pt1 != servers[i].comb.end())
            {
               if(pt1->first == pt2->first && pt1->second.fd_socket != -1 )
                    return true; // port already bind to socket
                ++pt1;
            }
            ++pt2;
            
        }
        ++it;
    }
    
    return false;
}


bool ServerManager::initializeAll() {
    std::set<int> epollFds;
    for (size_t i = 0; i < servers.size() ; i++){
      
        if (!servers[i].initialize(this->servers, static_cast<int>(i))) {
            std::cerr << "Failed to initialize a server" << std::endl;
            return false;
        }
        std::map<std::string, Socket>& serverComb = servers[i].comb;
        for (std::map<std::string, Socket>::iterator it = serverComb.begin(); 
             it != serverComb.end(); ++it) {

            int fd = it->second.fd_socket;
               // std::cout << "fd socket " << fd << endl;
            if (epollFds.find(fd) == epollFds.end())
            {    
                struct epoll_event event;
                event.events = EPOLLIN ;
                event.data.fd = fd;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) < 0) {
                    perror("epoll_ctl failed for client socket");
                    return false; 
                }
                epollFds.insert(fd);
                std::cout << "New event added : " << fd << " for port " << it->first << " to epoll" << std::endl;
            }
        }

    }
    routeRequest();
    return true;
}

// std::vector<int> ServerManager::getAllSocketFds() {
//     std::vector<int> fds;
//     for (size_t i = 0; i < servers.size() ; i++) {
//         fds.push_back(servers[i].getSocketFd());
//     }
//     return fds;
// }
// Server* ServerManager::findServerByFd(int fd) {
//     for (size_t i = 0; i < servers.size() ; i++) {
//         if (servers[i].getSocketFd() == fd) {
//             return &servers[i];
//         }
//     }
//     return NULL;
// }

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
            std::cout << "    Port " << server.getPort()[0] << std::endl;
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


Socket* ServerManager::findSocketByFd(int fd) {
    for (size_t i = 0; i < servers.size(); i++) {
        for (std::map<std::string, Socket>::iterator it = servers[i].comb.begin(); 
             it != servers[i].comb.end(); ++it) {
            
            if (it->second.fd_socket == fd) {
                return &(it->second);  
            }
        }
    }
    return NULL;
}

std::map<int, Socket*> ServerManager::getFdToSocketMap() {
    std::map<int, Socket*> fdToSocketMap;
    
    for (size_t i = 0; i < servers.size(); i++) {
        for (std::map<std::string, Socket>::iterator it = servers[i].comb.begin(); 
             it != servers[i].comb.end(); ++it) {
            int fd = it->second.fd_socket;
            if (fd != -1 && fdToSocketMap.find(fd) == fdToSocketMap.end()) {
                fdToSocketMap[fd] = &(it->second); 
            }
        }
    }
    
    return fdToSocketMap;
}






void    ServerManager::handle_cnx()
{
    char buffer[BUFFER_SIZE];
    std::map<int, Socket*> SocketMap = getFdToSocketMap();
    std::vector<int>::iterator it;
    int numEvents = epoll_wait(epollFd,events,MAX_EVENTS,30);
    if(numEvents < 0){
        std::cerr <<"epoll_wait failed" << std::endl; 
    }

	// std::cout << "=======" << numEvents << "=======\n";

    for(int i = 0; i < numEvents; i++){

        bool closeConnection = false;
        int currentFd = events[i].data.fd; 
       
        if(SocketMap.find(currentFd) != SocketMap.end()){ 
            int client_fd = accept(currentFd,NULL,NULL);
            std::cout << "client fd " << client_fd << "\n";

            if (client_fd < 0) {
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    std::cerr << "accept failed" << std::endl;
                }
                continue;
            }

            clients[client_fd].server_fd = currentFd;
            
            std::cout << "connection accepted from client "  <<std::endl;
            if(!Add_new_event(client_fd))
                continue;
        }
        else if(events[i].events & EPOLLIN){

            // cout << "recieved buffer from client" << endl;
            ssize_t bytesRead = recv(currentFd, buffer, BUFFER_SIZE ,0);
            if(bytesRead <= 0)
            {
                if (bytesRead == 0){
                    std::cout << "Client disconnected  " << currentFd <<std::endl;
                    
                }
                else {
                    std::cout << "read failed  " <<  currentFd <<std::endl;
                }
                clients[currentFd].closeConnection = true;
            }
            else {
                // buffer[bytesRead] = '\0';
	            clients[currentFd].buffer.append(buffer, bytesRead);
                std::memset(buffer, 0, BUFFER_SIZE);               
                
            }
            
        } 
        if (!clients[currentFd].checkRequestProgress())
        {
            clients[currentFd].parseRequest();
        }
        else if (events[i].events & EPOLLOUT)
        {
            // dprintf(2, "salammmmmmmmmmmmmmmmmmmmmmmmmmm\n");
            // std::string response;
            // location* loc = getClosestLocation(servers[0], "/");
            // // loc->printInfos();
            // if (loc) {
            //     // std::cout << "Best location path: " << loc->getPath() << std::endl << endl;
            //     response = handleGetRequest(clients[currentFd].data_rq, loc, servers);
            //     // std::string response = handleGetRequest(clients[currentFd].data_rq, "www");
            // }
            // std::string response = clients[currentFd].buildResponse();//"HTTP/1.1 200 ok\r\nContent-Length: 17\r\nConnection: close\r\n\r\nUpload succeeded.\n";
            // std::cout  <<  "=========" << response << "=========" ;
			// clients[currentFd].closeConnection = true;
            // send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);

			clients[currentFd].handleResponse(currentFd);
			clients[currentFd].closeConnection = true;
        }
        if(clients[currentFd].closeConnection)
        {
            if (epoll_ctl(epollFd, EPOLL_CTL_DEL, currentFd, NULL) == -1) {
                perror("epoll_ctl: EPOLL_CTL_DEL");
            }
            clients.erase(currentFd);
            close(currentFd);
            std::cout << "client removed \n";
        }
    }
}



Server     *chooseServer(std::vector<Server*> &routeServer,std::string host)
{
    if(routeServer.empty())
    {
        std::cerr << "Error: No servers available for routing" << std::endl;
        return NULL;
    }
    if(routeServer.size() == 1)
        return routeServer[0];
    std::vector<Server*>::iterator it ;
    for( it = routeServer.begin(); it != routeServer.end(); ++it){
        Server *server = *it;
        if(!server->params["server_name"].empty() &&  server->params["server_name"][0] == host)
        {       //std::cout << "host " << host << " server name "<< server->params["server_name"][0] << std::endl;
                return server;
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

