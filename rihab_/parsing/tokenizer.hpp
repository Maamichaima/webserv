
#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP
#include  <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <unistd.h>
#include <cstdio>


class Server;
class ServerManager;
class Tokenizer
{
    public:
    std::vector<std::string> tokens; 
    std::vector<std::string>::iterator current;
    std::vector<std::string>::iterator end;
    Tokenizer(){}
    bool    tokenizeString(std::string line);
    void initialize();
    void    advance();
    std::string peek();
    bool hasMore();
    bool parse(ServerManager &manager);
    ~Tokenizer();
    
};




std::vector<std::string> splitServerConfig(const std::string& input) ;
void print_map(const std::map<std::string, std::vector<std::string> > params) ;
std::vector<std::string> peekValues(Tokenizer& tokenizer) ;


#endif