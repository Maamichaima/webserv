
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
#include <stack>

class Server;
struct location;
class ServerManager;
class Tokenizer
{
    public:
    std::vector<std::string>           tokens; 
    std::stack<char>                   braceStack;
    std::vector<std::string>::iterator current;
    std::vector<std::string>::iterator end;
    Tokenizer();
    ~Tokenizer();
    bool                                tokenizeString(std::string line);
    void                                initialize();
    void                                advance();
    bool                                hasMore();
    bool                                parse(ServerManager &manager);
    std::string                         peek();
    
};

std::vector<std::string> splitServerConfig(const std::string& input) ;

#endif