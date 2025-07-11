#include "../_includes/ServerManager.hpp"
# include "../_includes/GetMethod.hpp"
#include <cstddef>
#include <ostream>


bool ctrC = true;

std::map<int, std::vector<Server *> > SocketToServers;

ServerManager::ServerManager(){
    epollFd = epoll_create(1024);
}
std::vector<Server>  &ServerManager::get_servers(){
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
        ServerLogger::serverError( "No server available " );
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
            if (it->second.getSocketFd() == currentFd) {  
                return it->first;  
            }
        }   
    }
    return "";
}

ServerManager::~ServerManager()
{
	close(epollFd);
	std::vector<int> fds = getAllSocketFds();
	for (size_t i = 0; i < fds.size(); i++)
	{
		close(fds[i]);
	}

	std::map<int,client>::iterator it = clients.begin();
	while(it != clients.end())
	{
		close(it->first);
		it++;
	}
	
}
bool ServerManager::Add_new_event(int fd_socket)
{
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
		// std::cout << "client to be closed " << currentFd << " error string: " << strerror(errno) << "====" << std::endl;
		// std::cout << epollFd << "\n==============\n";
        ServerLogger::serverError("epoll_ctl: EPOLL_CTL_DEL");
    }
    // std::cout <<"close fd " << currentFd << "\n==============\n";
    ServerLogger::clientDisconnected();
    clients.erase(currentFd);
    close(currentFd);
}

void ServerManager::checkTimeOut() {
    time_t now = std::time(NULL);
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
            if( send(clientFd, response.c_str(), response.size(), MSG_NOSIGNAL) <= 0)
                ServerLogger::serverError("Send failed ");
            ClientDisconnected(clientFd);
        }
    }
    
}

void handler(int sig)
{
    (void) sig;
    ctrC = false;
}

void    ServerManager::RunServer()
{
	signal(SIGINT, handler);

    while(ctrC)
    {
        checkTimeOut();
        char buffer[BUFFER_SIZE];
        int client_fd;
        std::vector<int> fds = getAllSocketFds();
        int numEvents = epoll_wait(epollFd,events,MAX_EVENTS,30);
        // if(numEvents < 0){
        //     ServerLogger::serverError("epoll_wait failed");
        // }
       
        for(int i = 0; i < numEvents; i++){
            int currentFd = events[i].data.fd; 
            // --- Check if this is a CGI fd for any client ---
            bool isCgiFd = false;
            for (std::map<int, client>::iterator it = clients.begin(); it != clients.end(); ++it) {
                if (it->second.cgi_running && it->second.cgi_fd == currentFd) {
                    isCgiFd = true;
                    // Read CGI output
                    char cgiBuf[4096];
                    ssize_t n = read(currentFd, cgiBuf, sizeof(cgiBuf));
                    if (n > 0) {
                        it->second.cgi_buffer.append(cgiBuf, n);
                    }
                    // Check if CGI finished (EOF or timeout)
                    if (n == 0 || (std::time(NULL) - it->second.cgi_start_time > 5)) {
                        // Timeout or EOF
                        if (n != 0) {
                            kill(it->second.cgi_pid, SIGKILL);
                        }
                        int status;
                        waitpid(it->second.cgi_pid, &status, 0);
                        
                        if (status != 0) {
                            it->second.data_rs.status_code = 500;
                            it->second.setDataResponse();
                            std::string errorResponse = it->second.buildResponse();
                            send(it->first, errorResponse.c_str(), errorResponse.size(), MSG_NOSIGNAL);
                            epoll_ctl(epollFd, EPOLL_CTL_DEL, it->second.cgi_fd, NULL);
                            close(it->second.cgi_fd);
                            it->second.cgi_running = false;
                            it->second.cgi_fd = -1;
                            it->second.cgi_pid = -1;
                            it->second.cgi_epoll_added = false;
                            ClientDisconnected(it->first);
                            break;
                        }
                        
                        epoll_ctl(epollFd, EPOLL_CTL_DEL, it->second.cgi_fd, NULL);
                        close(it->second.cgi_fd);
                        it->second.cgi_running = false;
                        it->second.cgi_fd = -1;
                        it->second.cgi_pid = -1;
                        it->second.cgi_epoll_added = false;
                        std::string response = buildCgiHttpResponse(it->second.cgi_buffer);
                        send(it->first, response.c_str(), response.size(), MSG_NOSIGNAL);
                        ClientDisconnected(it->first);
                    }
                    break;
                }
            }
            if (isCgiFd)
                continue;
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
                // std::cout << "buffer" << buffer << "\n";
                 clients[currentFd].lastActivityTime = std::time(NULL);
                
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
                // If a CGI was just started, add its fd to epoll (only once)
                if (clients[currentFd].cgi_running && clients[currentFd].cgi_fd != -1 && !clients[currentFd].cgi_epoll_added) {
                    struct epoll_event ev;
                    ev.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
                    ev.data.fd = clients[currentFd].cgi_fd;
                    epoll_ctl(epollFd, EPOLL_CTL_ADD, clients[currentFd].cgi_fd, &ev);
                    clients[currentFd].cgi_epoll_added = true; // <--- Set the flag
                }
            }
            if(clients[currentFd].closeConnection)
			{
                ClientDisconnected(currentFd);
			}
        }

    }
    // #close(epollFd);
}

Server     *chooseServer(std::vector<Server*> &routeServer,std::string host)
{
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
   
    return(routeServer[0]); 
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
 
}



