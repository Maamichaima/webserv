#ifndef SERVER_HPP
#define SERVER_HPP



#include "../Socket.hpp"

#include "tokenizer.hpp"


class ServerManager;

struct location{

    std::string path;
    std::map<std::string, std::vector<std::string> > infos;
    bool validParameter(Tokenizer& tokenizer);

    std::string getPath();
    std::string getInfos();
    void printInfos();

};


class Server{

    private:
    
        static int index ;
        std::string ip_address;
        std::vector<std::string> port;
        std::map<std::string,std::vector<std::string> > params;
        std::map<std::string,location> locations ;
        Socket socket;
        
        
    public:
        Server();
        bool    createServer(Tokenizer& tokenizer);
        bool    createParam(Tokenizer& tokenizer);
        bool    createLocation(Tokenizer& tokenizer);
        std::string getPort();
        void set_Port(std::string );
        void set_IpAddress(std::string);
        std::string getIpAddress();
        size_t getMaxBodySize();
        std::vector<std::string> getServerNames();
        std::string get_error_page();
        //const location* getLocation(const std::string& uri);
        location & getLocations(std::string key ) ;
        std::map<std::string, std::vector<std::string> >& getParameters() ;
        bool initialize();
        Socket & getSocket();
        int getSocketFd();
        int acceptConnection();
        void printLocations();
        ~Server();
    };

    bool   parceConfigFile(int argc,char **argv,ServerManager &manager);
    bool    param_Syntaxe(std::string key, std::vector<std::string> values,Server &server);
#endif