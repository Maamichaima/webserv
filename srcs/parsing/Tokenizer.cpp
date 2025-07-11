#include "../../_includes/Tokenizer.hpp"
#include "../../_includes/ServerManager.hpp"
#include <cstddef>
#include <utility>



Tokenizer::Tokenizer(){}

void Tokenizer::initialize() {
    current = tokens.begin();
    end = tokens.end();
}

void   Tokenizer::advance() {
    if (current == end ) throw std::runtime_error("Unexpected end of input");
    current++;
}

std::string Tokenizer::peek() {
    if (current == end)
        throw std::runtime_error("Unexpected end of input");
    return *current;
}

bool Tokenizer::hasMore()  { 
    return current != end;
}

std::vector<std::string> splitServerConfig(const std::string& input) {
    std::vector<std::string> parts;
    std::string current;
    
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];

        if (c == '{' || c == '}'|| c == ';') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
            parts.push_back(std::string(1, c));
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        parts.push_back(current);
    }
    return parts;
}

bool    Tokenizer::tokenizeString(std::string line)
{
    std::stringstream s(line);
    std::string word;
    while(s >> word) {
        std::vector<std::string> parts = splitServerConfig(word);
        tokens.insert(tokens.end(), parts.begin(), parts.end());
    }
    return true;
}


bool    Tokenizer::parse(ServerManager &manager) {
    while (hasMore()) {
        if (peek() == "server" ) {
            advance();
            if (peek() == "{") {
                braceStack.push('{'); 
                advance();
                Server server;
                if (!server.createServer(*this))
                {
                    return false;
                }
                else
                {
                    if (peek() == "}") {
                        if (!braceStack.empty()) {
                            braceStack.pop(); 
                        }
                        if(!braceStack.empty())
                        {   
                            ServerLogger::configSyntaxError(" UNCLOSED PRACES {} ");
                            return false;
                        }
                        manager.addServer(server);
                        advance();
                    }
                    else
                        return false;
                }
            }
            else {
                ServerLogger::configSyntaxError(" EXPECTED { AFTER SERVER KEYWORD ");
                return false;
            }
        }
        else if (!peek().empty()) { 
            ServerLogger::configSyntaxError(" ");
            return false;
        }
    }
    return true;
}

bool   parceConfigFile(int argc,char **argv,ServerManager &manager)
{
    if (argc < 2) {
        ServerLogger::serverError("Usage: ./webserv <config_file>" );
        return false;
    }

    std::string line;
    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        ServerLogger::serverError(" FAILED TO OPEN CONFIGURATION FILE !!  " );
        return false;
    }
    
    Tokenizer tokenizer; 
    while (std::getline(inputFile, line)) {

        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        if (line.empty() || line.find_first_not_of(" \t") == std::string::npos) {
            continue;
        }

        tokenizer.tokenizeString(line);
    }
    tokenizer.initialize();
    try {
        if (!tokenizer.parse(manager)) 
            return false;  
    } 
    catch (...) 
    {   
        return false; 
    }
    ServerLogger::serverStarted();
 
    return (true);    
}

Tokenizer::~Tokenizer(){}