#include "../_includes/Server.hpp"
#include <cstddef>

Server::Server() {
    ip_address = "";
    MaxBodySize = 0;
    for (std::vector<std::string>::iterator it = port.begin(); it != port.end(); ++it) {
        comb[*it] = Socket();
    }
}
    
Server::~Server(){
}

void Server::set_IpAddress(std::string ip) {
    ip_address = ip;
}

void    Server::set_Port(std::string port) {
   this->port.push_back(port);
}

std::vector<std::string> Server::getPort() {
    if(!port.empty())
        return(port);
    port.push_back("8080"); 
    std::cout << "port default " << port[0] << std::endl; 
    return(port);
}

std::string Server::getIpAddress(){
    return(ip_address);
}

location & Server::getLocations(std::string key) {
    return locations[key];
}


Server& Server::operator=(const Server &other) {

    if (this != &other) {  
        ip_address = other.ip_address;
        MaxBodySize = other.MaxBodySize;
        port = other.port;
        comb = other.comb;
        serverNames = other.serverNames;
        locations = other.locations;
        errorPages = other.errorPages;
    }
    return *this;

}

std::vector<std::string> peekValues(Tokenizer& tokenizer,bool &foundSemicolon) {
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

bool location::check_locations_key(std::string key, std::vector<std::string> values){

    if(key == "root" || key == "index" || key == "allowed_methods" || key == "autoindex" || 
        key == "redirect" || key == "cgi_extension" || key == "upload_store" || key =="cgi_path")
        return  Directives_syntaxe( key,  values);
    else
        return false;

}

bool location::validParameter(Tokenizer& tokenizer) {
    std::string key;
    bool foundSemicolon = false;
    while (tokenizer.hasMore() && tokenizer.peek() != "}") {
        key = tokenizer.peek();
        foundSemicolon = false;
        if (infos.find(key) != infos.end()) 
            return false;
        tokenizer.advance();
        std::vector<std::string> newValues = peekValues(tokenizer,foundSemicolon);
        if( !foundSemicolon || newValues.empty() )
            return false;
        if(!check_locations_key(key,newValues))
            return false;
        infos.insert(std::make_pair(key, newValues));
    }
    if(!tokenizer.hasMore() || !check_cgi_directives(infos))
        return false;

    return true;
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
    loc.path = "/" + path;
    locations[path] = loc;
    return true;
}

bool    Server::createParam(Tokenizer& tokenizer) {

    std::string key = tokenizer.peek();
    bool foundSemicolon = false;
    tokenizer.advance();
    std::vector<std::string> newValues = peekValues(tokenizer,foundSemicolon);
    if (!param_Syntaxe(key,newValues,*this) || !foundSemicolon )
        return false;
    
    return true;
}
bool Server::check_required_params()
{
    if(ip_address == "" || port.empty())
        return false;
    return true;
}


bool    Server::createServer(Tokenizer& tokenizer) {
    while (tokenizer.hasMore() && tokenizer.peek() != "}")
     {
        if (tokenizer.peek() == "location") {
            if(!createLocation(tokenizer))
            {   
                ServerLogger::configSyntaxError(" ERROR IN LOCATION ");
                return false;
            }
        } else {
            if(!createParam(tokenizer))
            {
                ServerLogger::configSyntaxError(" ERROR IN SERVER DIRECTIVES ");
                return false;
            }
        }
    }
    if (!tokenizer.hasMore()) {
        return false;
    }
    tokenizer.braceStack.pop();
    return true;
}

Socket* findExistingSocket(std::vector<Server>& servers, const std::string& port, int currentIndex)
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
    
    if(port.empty())
    {
        std::cout << " No port available  " << BOLD << RED << "✗ Failed to bind port "  << RESET << std::endl;
        return false;
    }
   for(size_t i = 0; i < port.size(); i++)
    {
        std::string currentPort = port[i];

        Socket* existingSocket = findExistingSocket(allServers, currentPort, currentIndex);
        Socket  *socket = &comb[port[i]];
        if(existingSocket != NULL)
        {
            comb[currentPort] = *existingSocket;
            std::cout << BOLD << BLUE << "[" << ServerLogger::getCurrentTime() << "]" << RESET 
            << " " << BOLD << CYAN << "[WebServ]" << RESET 
            << " " << BOLD << GREEN << "🚀 Server  created successfully sharing existing PORT !! "  
            << BOLD << YELLOW  << ":" << port[i] << RESET << std::endl;
        }
        else{
            if (!socket->initialize(port[i],getIpAddress()) || !socket->create_Socket())
            {
                std::cout << " <getaddrinfo> error " << BOLD << RED << "✗ Failed to bind port " << port[i] << RESET << std::endl;
                return false;
            }
            if (!socket->bind_Socket()) {
                ServerLogger::portBindError(port[i]); 
                return false;
            }
            if (!socket->listen_socket()) {
                ServerLogger::portListenError(port[i]);
                return false;
            }
            ServerLogger::serverCreated(port[i]);
           
        }

        // if (socket->host_info != NULL) {
        //     freeaddrinfo(socket->host_info);
        //     socket->host_info = NULL;
        // }
    }
    return true;
}


std::string location::getPath() const {
    return path;
}

std::vector<std::string>* location::getInfos(std::string key){

    std::map<std::string, std::vector<std::string> >::iterator it = infos.begin();

    while(it != infos.end())
    {
        if(it->first == key)
            return &it->second;
        it++;
    }
    // std::cout << "key is unavailable in the location" << std::endl;
    return(NULL);
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




