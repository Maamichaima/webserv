#include "_includes/ServerManager.hpp"
#include "_includes/Socket.hpp"
#include "_includes/Server.hpp"


int main(int argc,char **argv){

    ServerManager manager;
    if(!parceConfigFile(argc,argv,manager))
        return(1);
    
    manager.printAllServerInfo();
    manager.initializeAll();
    
   

    while(1)
    {
        
        manager.handle_cnx();
          
    }
}