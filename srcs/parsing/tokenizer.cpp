#include "../../_includes/tokenizer.hpp"
#include "../../_includes/ServerManager.hpp"
#include <cstddef>
#include <utility>

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

bool Tokenizer::hasMore()  { return current != end;
}
        

void print_map(std::map<std::string, std::vector<std::string> > params) {
    std::map<std::string, std::vector<std::string > >::iterator it;
    for (it = params.begin(); it != params.end(); ++it) {
        std::cout << it->first << ": ";
        
        std::vector<std::string> values = it->second;
        for (size_t i = 0; i < values.size(); ++i) {
            std::cout << values[i];
            if (i < values.size() - 1) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
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

bool Tokenizer::skipToNextServerBlock() {
    while (hasMore()) {
        std::string current = peek();
        
        if (current == "{") {
            braceStack.push('{');
        } 
        else if (current == "}") {
            if (!braceStack.empty()) {
                braceStack.pop();
            }
        }
        else if (current == "server" && braceStack.empty()) {
            return true;
        }
        
        advance();
    }
    return false;
}

bool    Tokenizer::parse(ServerManager &manager) {
    while (hasMore()) {
        if (peek() == "server" ) { // make sure it's the first element in the server or it's preceded by }
            advance();
            if (peek() == "{") {
                braceStack.push('{'); 
                advance();
                Server server;
                if (!server.createServer(*this))
                {
                    std::cerr << "Error: Server #"  << " creation failed" << std::endl;
                    if(skipToNextServerBlock())
                        continue;
                }
                else
                {
                    if (peek() == "}") {
                        if (!braceStack.empty()) {
                            braceStack.pop(); 
                        }
                        if(!braceStack.empty())
                            return false;
                        manager.addServer(server);
                        std::cout << "new server added" << std::endl;
                        advance();
                    }
                    else
                        return false;
                }

            }
            else 
                return false;
        }
        else if (!peek().empty()) { 
            return false;
        }

    }
    return true;
}

bool   parceConfigFile(int argc,char **argv,ServerManager &manager)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return false;
    }

    std::string line;
    std::ifstream inputFile(argv[1]);
    
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return false;
    }
    
    Tokenizer tokenizer; 
    while (std::getline(inputFile, line)) {
        if (line.empty()) {
            continue;
        }
        tokenizer.tokenizeString(line);
    }
    
    tokenizer.initialize();
   
    if(!tokenizer.parse(manager))
        return false;
 
    
    return (true);
    

         
}
Tokenizer::~Tokenizer(){}