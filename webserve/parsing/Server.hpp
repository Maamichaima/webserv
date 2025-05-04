#ifndef SERVER_HPP
#define SERVER_HPP


#include "tokenizer.hpp"

class Server{

    private:
    
        int index;
        std::map<std::string,std::vector<std::string> > params;
        std::vector<location> locations ;
        
    public:
        Server(){}
        bool    createServer(Tokenizer& tokenizer);
        bool    createParam(Tokenizer& tokenizer);
        void    createLocation(Tokenizer& tokenizer);
        int getPort(){}
        int get_max_body_size(){}
        std::string get_server_name(){}
        std::string get_error_page(){}
    };

#endif