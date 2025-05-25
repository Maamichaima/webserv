#include "../_includes/ServerManager.hpp"
# include "methods/GetMethod.hpp"
#include <cstddef>

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
        // if(check_multiple_port(this->servers,i))
        //     continue;

        if (!servers[i].initialize(this->servers, static_cast<int>(i))) {
            std::cerr << "Failed to initialize a server" << std::endl;
            return false;
        }
        std::map<std::string, Socket>& serverComb = servers[i].comb;
        for (std::map<std::string, Socket>::iterator it = serverComb.begin(); 
             it != serverComb.end(); ++it) {

            int fd = it->second.fd_socket;
                std::cout << "fd socket " << fd << endl;
            if (epollFds.find(fd) == epollFds.end())
            {    
                struct epoll_event event;
                event.events = EPOLLIN  | EPOLLET;
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
    for(int i = 0; i < numEvents; i++){
        
        bool closeConnection = false;
        int currentFd = events[i].data.fd; 
        //std::cout << "==========================================" << "current fd " << currentFd << "=======================================" << std::endl;
        if(SocketMap.find(currentFd) != SocketMap.end()){ 
            
            // Socket *socket = SocketMap[currentFd];
            int client_fd = accept(currentFd,NULL,NULL);
            //std::cout << client_fd << "\n";

            if (client_fd < 0) {
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    std::cerr << "accept failed" << std::endl;
                }
                continue;
            }
            clients[client_fd];
            clients[client_fd].server_fd = currentFd;
            std::cout << "connection accepted from client "  <<std::endl;
            if(!Add_new_event(client_fd))
                continue;
           
        }
        else if(events[i].events & EPOLLIN){
            ssize_t bytesRead = recv(currentFd, buffer, BUFFER_SIZE - 1,0);
            // std::cout << buffer << "\n";
            if(bytesRead <= 0)
            {
                if (bytesRead == 0){
                    std::cout << "Client disconnected  " << currentFd <<std::endl;
                    
                }
                else {
                    std::cout << "read failed  " <<  currentFd <<std::endl;
                }
                closeConnection = true;
            }
            else{

                buffer[bytesRead] = '\0';
                // std::cout << buffer << std::endl;
	            clients[currentFd].buffer.append(buffer, bytesRead);

                // clients[currentFd].setBuffer(buffer, bytesRead);
                // this->servers[0].locations["/api"]; ??? !!!
                // std::cout << "Received from client " << currentFd << ": " << buffer << std::endl;
                std::memset(buffer, 0, BUFFER_SIZE);
                
               
            }
                
            
        }
        if (!clients[currentFd].checkRequestProgress())
        {
            clients[currentFd].myServer = servers[0];
            clients[currentFd].parseRequest();
        }
        else if (events[i].events & EPOLLOUT)
        {
            // dprintf(2, "salammmmmmmmmmmmmmmmmmmmmmmmmmm\n");
            // //send responde
            // // //std::cout << "sending........\n";
            // std::string response;
            // location* loc = getClosestLocation(servers[0], "/");
            // if (loc) {
            //     std::cout << "Best location path: " << loc->getPath() << std::endl << endl;
            //     std::cout << "result: " << loc->getInfos("root")->at(0) << std::endl << std::endl;
            //     response = handleGetRequest(clients[currentFd].data_rq, loc->getInfos("root")->at(0));
            //     // std::string response = handleGetRequest(clients[currentFd].data_rq, "www");
            // }

            // cout << "response: "<< response << endl;
            // cout << "size : "<< response.size() << endl;
            // printf("%zu\n",strlen(response.c_str()));
            // send(currentFd, response.c_str(), strlen(response.c_str()), 0);
            send(currentFd, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 11\r\n\r\nHello rihab",77, 0);
            closeConnection = true;
            
            // std::string response = "HTTP/1.1 200 ok\r\nContent-Length: 17\r\nConnection: close\r\n\r\nUpload succeeded.\n";
            // send(currentFd, response.c_str(), response.size(), 0);
        }
        if(closeConnection)
        {
            std::cout << "client removed \n";
            if (epoll_ctl(epollFd, EPOLL_CTL_DEL, currentFd, nullptr) == -1) {
                perror("epoll_ctl: EPOLL_CTL_DEL");
            }
            
            clients.erase(currentFd);
            close(currentFd);
        }
    }
}

// GET /tmp/index.html HTTP

Server*     chooseServer(std::vector<Server> routeServer,std::string host)
{
    std::vector<Server>::iterator it ;
    for( it = routeServer.begin(); it != routeServer.end(); ++it){

        if(!it->params["server_name"].empty() &&  it->params["server_name"][0] == host)
            return &(*it);
    }
    return (NULL);
}

Server    *ServerManager::routeRequest(client cl ,std::string host)
{

        int fd = cl.server_fd;
        std::vector<Server> routeServer;
        std::vector<Server>::iterator it;
        for( it = servers.begin(); it != servers.end(); ++it)
        {
            for(std::map<std::string ,Socket>::iterator tt = it->comb.begin();
                 tt !=  it->comb.end(); ++tt)
            {
                if(fd == tt->second.fd_socket)
                    routeServer.push_back(*it);
            }
        }

        if(routeServer.size() > 1)
            return (chooseServer(routeServer,host));

        return(&(*it));
    
}