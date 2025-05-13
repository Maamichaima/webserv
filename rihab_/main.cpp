#include "ServerManager.hpp"
#include "Socket.hpp"
#include "parsing/Server.hpp"


int main(int argc,char **argv){

    ServerManager manager;
    if(!parceConfigFile(argc,argv,manager))
        return(1);
    
    manager.printAllServerInfo();
    manager.initializeAll();
    manager.getAllSocketFds();
    std::vector<Server> server = manager.get_server();


    while(1)
    {

        manager.handle_cnx();
          
    }
}