#include "../_includes/Server.hpp"
#include <cstddef>



Server::Server() {
    ip_address = "";
    std::string MaxBodySize ="";
    for (std::vector<std::string>::iterator it = port.begin(); it != port.end(); ++it) {
        comb[*it] = Socket();
    }

}
    
Server::~Server(){}

void Server::set_IpAddress(std::string ip){
    ip_address = ip;
}

void    Server::set_Port(std::string port){
   this->port.push_back(port);
}

bool check_all_keys(std::string key){

    if(key == "root" || key == "index" || key == "allowed_methods" || key == "autoindex" || 
        key == "redirect" || key == "cgi_extension" || key == "upload_store" || key =="cgi_path" ||
        key == "listen" || key == "server_name" || key == "client_max_body_size" || key == "error_page" || key =="host" )
        return true;
    else
        return false;

}

std::vector<std::string> peekValues(Tokenizer& tokenizer,bool &foundSemicolon) {//pick
    std::vector<std::string> vec;
    
    while (tokenizer.hasMore() && tokenizer.peek() != ";" && !check_all_keys(tokenizer.peek()) )  { 
        vec.push_back(tokenizer.peek());
        tokenizer.advance();
    }
 
    if (tokenizer.hasMore() && tokenizer.peek() == ";") {
        foundSemicolon = true;
        tokenizer.advance(); 
    }
    return vec;
}

bool    Server::createLocation(Tokenizer& tokenizer) {

    location loc;
    std::map<std::string, std::vector<std::string> > locationParams;

    tokenizer.advance(); 
    std::string path = tokenizer.peek();
    tokenizer.advance(); 
    if(tokenizer.peek() != "{")
        return false;
    tokenizer.braceStack.push('{');
    tokenizer.advance(); 
    if(!loc.validParameter( tokenizer))
        return false;
    if(tokenizer.peek() != "}")
        return false;
    tokenizer.advance();
    tokenizer.braceStack.pop();
    loc.path = path;
    locations[path] = loc;
    
    return true;
}

bool Server::createParam(Tokenizer& tokenizer) {
    std::string key = tokenizer.peek();
    // std::vector<std::string>::iterator it= tokenizer.find(key);
    bool foundSemicolon = false;
    if (key == "listen" || key == "server_name" || key == "client_max_body_size" || key == "error_page" || key =="host" ) {

        tokenizer.advance();
        std::vector<std::string> newValues = peekValues(tokenizer,foundSemicolon);
        
        if (!param_Syntaxe(key,newValues,*this) || !foundSemicolon )
            return false;
    } 
    else {
        std::cout << key << " not a required parameter" << std::endl;
        return false;
    }
    
    std::cout << std::endl;
    return true;
}

    



bool Server::createServer(Tokenizer& tokenizer) {
    while (tokenizer.hasMore() && tokenizer.peek() != "}")
     {
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
    tokenizer.braceStack.pop();
    return true;
}


std::vector<std::string> Server::getPort() {
    if(!port.empty())
        return(port);
        //return a vector of ports if port empty set the fisrt elm of vector to default port
    port.push_back("8080");  
    return(port);
}

std::string Server::getIpAddress(){

    return(ip_address);
}


Socket* findExistingSocket(std::vector<Server>& servers, const std::string& port, int currentIndex)//
{
    for (int i = 0; i < currentIndex; i++) {
        std::map<std::string, Socket>::iterator it = servers[i].comb.find(port);
        if (it != servers[i].comb.end() && it->second.fd_socket != -1) {
            return (&(it->second));
        }
    }
    return NULL;
}

bool Server::initialize(std::vector<Server>& allServers, int currentIndex) {
    
   // initialize with  all the elm in the port vector 
   //befor initializing we check if the fd_socket of a specific port already exist in a the vector of servers 
   for(size_t i = 0; i < port.size(); i++)
    {
        std::string currentPort = port[i];

        Socket* existingSocket = findExistingSocket(allServers, currentPort, currentIndex);
        if(existingSocket != NULL)
        {
            comb[currentPort] = *existingSocket;
            std::cout << "Server " 
                      << " sharing existing socket for port " << currentPort 
                      << " (fd: " << existingSocket->fd_socket << ")" << std::endl;
        }
        else{

            Socket  *socket = &comb[port[i]];
            socket->initialize(port[i],getIpAddress());
        
            if (!socket->create_Socket()) {
                std::cerr << "Failed to create socket for port " << currentPort << std::endl;
                return false;
            }
            std::cout << socket->fd_socket << std::endl;
            if (!socket->bind_Socket()) {
                std::cerr << "Failed to bind socket for port " << currentPort << std::endl;
                return false;
            }
            
            if (!socket->listen_socket()) {
                std::cerr << "Failed to listen on socket for port " << currentPort << std::endl;
                return false;
            }
            
            std::cout << "Server "  
                        << " created new socket for port " << currentPort 
                        << " (fd: " << socket->fd_socket << ")" << std::endl; 
           
        }
    }
    return true;
}

// int Server::getSocketFd()  {
//     return socket.fd_socket;
// }

// std::map<std::string, std::vector<std::string> >& Server::getParameters()  {
//     return params;
// }

location & Server::getLocations(std::string key)  {
    return locations[key];
}

// Socket & Server::getSocket(){
//     return socket;
// }

  
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



bool location::check_locations_key(std::string key){

    if(key == "root" || key == "index" || key == "allowed_methods" || key == "autoindex" || 
        key == "redirect" || key == "cgi_extension" || key == "upload_store" || key =="cgi_path")
        return true;
    else
        return false;

}

bool location::validParameter(Tokenizer& tokenizer) {
    std::string key;
    bool foundSemicolon = false;
    //std::map<std::string, std::vector<std::string> >::iterator it;
    while (tokenizer.hasMore() && tokenizer.peek() != "}") {
        key = tokenizer.peek();
        if(!check_locations_key(key))
        {
            std::cerr << "location key invalid" << std::endl;
            return false;
        }
        tokenizer.advance();
        std::vector<std::string> newValues = peekValues(tokenizer,foundSemicolon);
        if(newValues.empty() || !foundSemicolon )
            return false;
        
        infos.insert(std::make_pair(key, newValues));//insert 
    }
    
    if(!tokenizer.hasMore() )
        return false;

    return true;
}





std::vector<std::string>* location::getInfos(std::string key){

    std::map<std::string, std::vector<std::string> >::iterator it = infos.begin();

    while(it != infos.end())
    {
        if(it->first == key)
            return &it->second;
        it++;
    }
    std::cout << "key is unavailable in the location" << std::endl;
    return(NULL);
}


Server& Server::operator=(const Server &obj) {
    if(this != &obj) {

        ip_address = obj.ip_address;
        port = obj.port;
        comb = obj.comb;
        locations = obj.locations;
      
    }
    return *this;
}