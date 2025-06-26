#include "_includes/ServerManager.hpp"
#include "_includes/Socket.hpp"
#include "_includes/Server.hpp"
#include "srcs/methods/DeleteMethod.hpp"
#include "_includes/ServerLogger.hpp"


int main(int argc,char **argv){

    ServerManager manager;
    if(!parceConfigFile(argc,argv,manager))
        return(1);
    
    //manager.printAllServerInfo();
    manager.initializeAll();
    //exampleUsage();

    manager.RunServer();
}


