#include "Server.hpp"

void    Server::createLocation(Tokenizer& tokenizer) {
    std::map<std::string, std::vector<std::string> > locationParams;
    
    tokenizer.advance(); 
    std::string path = tokenizer.peek();
    tokenizer.advance(); 
    tokenizer.advance(); 
    
    location loc;
    loc.validParameter( tokenizer);
    
    tokenizer.advance(); // Skip '}'
    std::cout << "<---------------location-------------------->" << std::endl;
    print_map(loc.infos);
    
    // Create and store the location
    loc.path = path;
    locations.push_back(loc);
    
    return;
}

bool Server::createParam(Tokenizer& tokenizer) {
    std::string key = tokenizer.peek();
    std::map<std::string, std::vector<std::string> >::iterator it;
    if (key == "listen" || key == "server_name" || key == "client_max_body_size" || key == "error_page") {
        tokenizer.advance();
        std::vector<std::string> newValues = peekValues(tokenizer);
        it = params.find(key);
        if(it != params.end())
        {
            it->second.insert(it->second.end(), newValues.begin(), newValues.end());
        }
        else{

            params.insert(std::make_pair(key, newValues));//insert 
        }
        tokenizer.advance();
    } else {
        std::cout << key << " not a required parameter" << std::endl;
        tokenizer.advance();
        
        while (tokenizer.peek() != ";") {
            tokenizer.advance();
        }
        
        tokenizer.advance(); 
        return false;
    }
    
    std::cout << std::endl;
    return true;
}

    



bool Server::createServer(Tokenizer& tokenizer) {
    while (tokenizer.hasMore() && tokenizer.peek() != "}") {
        if (tokenizer.peek() == "location") {
            createLocation(tokenizer);
        } else {
            createParam(tokenizer);
        }
    }
    
    std::cout << "<---------------params-------------------->" << std::endl;
    print_map(params);
    
    if (!tokenizer.hasMore()) {
        return false;
    }
    
    return true;
}


