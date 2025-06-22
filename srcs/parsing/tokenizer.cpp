#include "../../_includes/tokenizer.hpp"
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
                    std::cerr << "Error: server creation failed due to syntaxe error " << std::endl;
                    return false;
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
                        advance();
                    }
                    else
                        return false;
                }
            }
            else {
                std::cerr << "Error: server creation failed due to  syntaxe error " << std::endl;
                return false;
            }
        }
        else if (!peek().empty()) { 
            std::cerr << "Error: server creation failed due to  syntaxe error " << std::endl;
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

    try {
        if (!tokenizer.parse(manager)) {
            return false;  
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Tokenizer error: " << e.what() << std::endl;
        return false; 
    }
 
    return (true);    
}

Tokenizer::~Tokenizer(){}