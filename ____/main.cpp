#include "ServerManager.hpp"
#include "Socket.hpp"
#include "parsing/Server.hpp"


int main(int argc,char **argv){

    ServerManager manager;
    if(!parceConfigFile(argc,argv,manager))
        return(1);
    
   // manager.initializeAll();
    manager.printAllServerInfo();
    
   

    // while(1)
    // {

    //     manager.handle_cnx();
          
    // }
}