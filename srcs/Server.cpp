#include "../_includes/Server.hpp"

int Server::index = 0;

Server::Server() {
    index++;
    ip_address = "127.0.0.1";
}
    
Server::~Server(){}

void Server::set_IpAddress(std::string ip){
    ip_address = ip;
}

void    Server::set_Port(std::string port){
   this->port.push_back(port);
}

bool    Server::createLocation(Tokenizer& tokenizer) {
    std::map<std::string, std::vector<std::string> > locationParams;
    
    tokenizer.advance(); 
    std::string path = tokenizer.peek();
    tokenizer.advance(); 
    tokenizer.advance(); 
    
    location loc;
    if(!loc.validParameter( tokenizer))
        return false;
    
    tokenizer.advance(); // Skip '}'
    
    // Create and store the location
    loc.path = path;
    locations[path] = loc;
    
    return true;
}

bool Server::createParam(Tokenizer& tokenizer) {
    std::string key = tokenizer.peek();
    std::map<std::string, std::vector<std::string> >::iterator it;
    if (key == "listen" || key == "server_name" || key == "client_max_body_size" || key == "error_page" ) {
        tokenizer.advance();
        std::vector<std::string> newValues = peekValues(tokenizer);
        it = params.find(key);
        if(it != params.end()) //if the parameter already existe 
        {
            it->second.insert(it->second.end(), newValues.begin(), newValues.end());
        }   
        else{

            params.insert(std::make_pair(key, newValues));//insert 
        }
        if (!param_Syntaxe(key,params[key],*this))
            return false;

        tokenizer.advance();
    } else {
        std::cout << key << " not a required parameter" << std::endl;
        tokenizer.advance();
        
        while (tokenizer.peek() != ";") {
            tokenizer.advance();
        }
        
        tokenizer.advance(); 
        return false;
    }
    
    std::cout << std::endl;
    return true;
}

    



bool Server::createServer(Tokenizer& tokenizer) {
    while (tokenizer.hasMore() && tokenizer.peek() != "}") {
        if (tokenizer.peek() == "location") {
            if(!createLocation(tokenizer))
                return false;

        } else {
            if(!createParam(tokenizer))
                return false;
        }
    }
    if (!tokenizer.hasMore()) {
        return false;
    }
    
    return true;
}


std::string Server::getPort() {
    if(!port.empty())
        return(port[0]);
    return("8080");
}

std::string Server::getIpAddress(){

    return(ip_address);
}

bool Server::initialize() {
   
    socket.initialize(getPort(),getIpAddress());
    
    if (!socket.create_Socket()) {
        return false;
    }
    
    if (!socket.bind_Socket()) {
        return false;
    }
    
    if (!socket.listen_socket()) {
        return false;
    }
    
    std::cout << "Server initialized on port " << getPort() << std::endl;
    return true;
}

int Server::getSocketFd()  {
    return socket.fd_socket;
}

std::map<std::string, std::vector<std::string> >& Server::getParameters()  {
    return params;
}

location & Server::getLocations(std::string key)  {
    return locations[key];
}

Socket & Server::getSocket(){
    return socket;
}

  
  void Server::printLocations() {
    std::cout << "Server Locations:" << std::endl;
    
    if (locations.empty()) {
        std::cout << "  No locations defined." << std::endl;
        return;
    }
    
    for (std::map<std::string, location>::const_iterator it = locations.begin(); 
         it != locations.end(); ++it) {
        
        std::cout << "Location key: " << it->first << std::endl;
    
        // Print infos map
        std::cout << "  Infos:" << std::endl;
        if (it->second.infos.empty()) {
            std::cout << "    No infos defined." << std::endl;
        } else {
            for (std::map<std::string, std::vector<std::string> >::const_iterator infoIt = it->second.infos.begin();
                 infoIt != it->second.infos.end(); ++infoIt) {
                
                std::cout << "    " << infoIt->first << ": ";
                
                // Print the vector of strings
                std::cout << "[";
                for (size_t i = 0; i < infoIt->second.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << infoIt->second[i];
                }
                std::cout << "]" << std::endl;
            }
        }
        std::cout << std::endl;
    }
}












































// int Server::acceptConnection(){
//     Epoll_events cnx = socket.event;
//     char buffer[BUFFER_SIZE];
//     int numEvents = epoll_wait(cnx.get_epollFd(),cnx.events,MAX_EVENTS,-1);
//     if(numEvents < 0){
//         std::cerr <<"epoll_wait failed" << std::endl; 
//     }

//     for(int i = 0; i < numEvents; i++){
//         int currentFd = cnx.events[i].data.fd;

//         if(currentFd == socket.getSocketFd()){ // it can be more than one socket in the server
//                 int client_fd = accept(socket.getSocketFd(),(struct sockaddr *)&(socket.host_addr),(socklen_t *)&socket.host_addrlen);
//                 if (client_fd < 0) {
//                     if (errno != EAGAIN && errno != EWOULDBLOCK) {
//                         std::cerr << "accept failed" << std::endl;
//                     }
//                     continue;
//                 }
//                 std::cout << "connection accepted from client " <<std::endl;
           
//                 if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
//                     // perror("fcntl F_SETFL O_NONBLOCK");
//                     close(client_fd);
//                     continue;
//                 }
//                 socket.event.Add_new_event(client_fd);
//         }
//         else if(cnx.events[i].events & EPOLLIN){

//             bool closeConnection = false;
        
//                 ssize_t bytesRead = recv(currentFd, buffer, BUFFER_SIZE - 1,0);
//                 if (bytesRead < 0) {
                    
//                     if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                         // No data available, not an error
//                         std::cout << "No data available on socket " << currentFd << std::endl;
//                     } else {
//                         perror("read failed");
//                         closeConnection = true;
//                     }
//                 }
//                 else if (bytesRead == 0){
//                     std::cout << "Client disconnected" << std::endl;
//                     closeConnection = true;
//                     break;
//                 }
//                 else{

//                     buffer[bytesRead] = '\0';
//                     std::cout << "Received from client " << currentFd << ": " << buffer << std::endl;
//                     if (write(currentFd,  "buffer received \n", 18) < 0) {
//                         perror("write failed");
//                         closeConnection = true;
//                         break;
//                     }
//                 }

//                 if (closeConnection) {
//                     close(currentFd);
//                 }
//         }
//     }

//     return 0;
// }
