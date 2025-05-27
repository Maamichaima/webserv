#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Server.hpp"
#include <cstddef>
#include "client.hpp"
#include "parser.hpp"
#include <set>


class ServerManager{

    private:
        std::vector<Server> servers;
        std::map<int,client> clients;
        static size_t numServer ;
        int epollFd;
        struct epoll_event events[MAX_EVENTS];
    public:
        ServerManager();
        std::vector<Server>     get_servers();
        std::map<int,client>    get_clients();
        size_t                  getNumServer();
        void                    addServer(Server& server);
        bool                    initializeAll() ;
        std::vector<int>        getAllSocketFds() ;
        Server*                 findServerByFd(int fd) ;
        void                    printAllServerInfo() ;
        bool                    Add_new_event(int fd_socket);
        void                    handle_cnx();
        Socket*                 findSocketByFd(int fd);
        std::map<int, Socket*>  getFdToSocketMap();
        std::vector<Server*>    routeRequest(client cl );
};


#endif

