#include "_includes/ServerManager.hpp"
#include "_includes/Socket.hpp"
#include "_includes/Server.hpp"
#include "_includes/ServerLogger.hpp"


int main(int argc,char **argv){

    ServerManager manager;
    if(!parceConfigFile(argc,argv,manager))
        return(1);
    
    if(manager.initializeAll())    
        manager.RunServer();


    return 0;
}


