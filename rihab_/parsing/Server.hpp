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
        std::map<std::string,std::vector<std::string> > params;
        std::vector<location> locations ;
        Socket socket;
        
    public:
        Server();
        bool    createServer(Tokenizer& tokenizer);
        bool    createParam(Tokenizer& tokenizer);
        void    createLocation(Tokenizer& tokenizer);
        std::string getPort();
        std::string getIpAddress();
        size_t getMaxBodySize();
        std::vector<std::string> getServerNames();
        std::string get_error_page();
        //const location* getLocation(const std::string& uri);
        std::vector<location>& getLocations() ;
        std::map<std::string, std::vector<std::string> >& getParameters() ;
        bool initialize();
        Socket & getSocket();
        int getSocketFd();
        int acceptConnection();
        ~Server();
    };

    bool   parceConfigFile(int argc,char **argv,ServerManager &manager);
    bool    param_Syntaxe(std::string key, std::vector<std::string> values);
#endif