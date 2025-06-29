#ifndef SERVER_HPP
#define SERVER_HPP
#include "Socket.hpp"
#include "Tokenizer.hpp"


class ServerManager;

struct location{
    
    std::string                                      path;
    std::map<std::string, std::vector<std::string> > infos;
    bool                                             validParameter(Tokenizer& tokenizer);
    bool                                             check_locations_key(std::string key,  std::vector<std::string> values);
    std::string                                      getPath() const ;
    std::vector<std::string>*                        getInfos(std::string key);
};

class Server{
    
    public:
        std::string                       ip_address;
        size_t                            MaxBodySize;
        std::vector<std::string>          port;
        std::map<std::string,Socket>      comb;
        std::vector<std::string>          serverNames;
        std::map<std::string,location>    locations ;
        std::map<std::string,std::string> errorPages;
        Server();
        bool                     createServer(Tokenizer& tokenizer);
        bool                     createParam(Tokenizer& tokenizer);
        bool                     createLocation(Tokenizer& tokenizer);
        void                     set_Port(std::string );
        void                     set_IpAddress(std::string);
        bool                     initialize(std::vector<Server>& allServers, int currentIndex);
        int                      getSocketFd();
        void                     printLocations();
        Server                   &operator=(const Server &obj);
        location &               getLocations(std::string key ) ;
        std::string              getIpAddress();
        std::vector<std::string> getPort();
        bool                     check_required_params();
        ~Server();
    };
    
    

    bool    parceConfigFile(int argc,char **argv,ServerManager &manager);
    bool    param_Syntaxe(std::string key, std::vector<std::string> values,Server &server);
    bool    Directives_syntaxe(std::string key, std::vector<std::string> values);
    bool    check_cgi_directives( std::map<std::string, std::vector<std::string> > infos );
    bool    check_all_keys(std::string key);

#endif