#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "./parsing/Server.hpp"
#include <cstddef>
#include "../chaima_/client.hpp"


class ServerManager{

    private:
    std::vector<Server> servers;
    std::map<int,client> clients;
    static size_t numServer ;
    int epollFd;
    struct epoll_event events[MAX_EVENTS];
    public:
        ServerManager();
        std::vector<Server>  get_servers();
        std::map<int,client>  get_clients();
        size_t  getNumServer();
        void addServer(Server& server);
        bool initializeAll() ;

        std::vector<int> getAllSocketFds() ;
        Server* findServerByFd(int fd) ;
        void printAllServerInfo() ;

        bool Add_new_event(int fd_socket);
        // bool find_client_server(int fd)
        // {
        //     for(size_t i = 0;i < servers.size() ;i++)
        //     {
        //         for(size_t i = 0;i < servers.size() ;i++)
        //         {

        //         }
        //     }
        // }

       void handle_cnx();






};


#endif

