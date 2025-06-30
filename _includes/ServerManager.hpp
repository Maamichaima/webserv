#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Server.hpp"
#include <cstddef>
#include "client.hpp"
#include "parser.hpp"
#include <set>


class ServerManager{

    private:
        int                     epollFd;
        struct epoll_event      events[MAX_EVENTS];
        std::vector<Server>     servers;
        std::map<int,client>    clients;
    public:
        ServerManager();
        std::vector<Server>     get_servers();
        std::map<int,client>    get_clients();
        void                    addServer(Server& server);
        bool                    initializeAll() ;
        std::vector<int>        getAllSocketFds() ;
        Server*                 findServerByFd(int fd) ;
        void                    printAllServerInfo() ;
        bool                    Add_new_event(int fd_socket);
        void                    RunServer();
        Socket*                 findSocketByFd(int fd);
        void                    routeRequest();
        void                    checkTimeOut();
        void                    ClientDisconnected(int currentFd);
        std::string             findPort(int currentFd);
};


#endif

