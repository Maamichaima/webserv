#include "../../_includes/tokenizer.hpp"
#include "../../_includes/Server.hpp"


bool   valide_port(const std::string& port){

    for(int i  ; i < port.size() ;i++){
        char c = port[i];
        if(!isdigit(c))
            return false;
    }
    int port_num = std::atoi(port.c_str());
    if (port_num < 1 || port_num > 65535)
        return false;
    
    return true;
}

bool    valide_ip_address(const std::string & ip)
{
    if (ip == "localhost")
        return true;

    std::vector<std::string> octets;
    std::string temp = "";

    for(size_t i = 0; i < ip.size(); ++i)
    {
        if(ip[i] == '.')
        {
            octets.push_back(temp);
            temp = "";
        }
        else
        {
            temp += ip[i];
        }
    }
    if(!temp.empty())
        octets.push_back(temp);
    

    if(octets.size() != 4)
        return false;

    for(int i  ; i < octets.size() ;i++)
    {
        const std::string octet = octets[i];
        if(octet.empty())
            return false;
        for (int i  ; i < octet.size() ;i++)
        {
            char c = octet[i];
            if (!isdigit(c))
                return false;
        }
        int num = std::atoi(octet.c_str());
        if (num < 0 || num > 255)
            return false;
    }

    return true;
}

bool check_listen( std::vector<std::string> values,Server &server)
{
    if(values.empty())
        return false;
    std::vector<std::string>::iterator it = values.begin();
    while(it != values.end())
    {
        std::string value = *it;
        if (!valide_port(value))
            return false;
        if (std::find(server.port.begin(), server.port.end(), value) != server.port.end()) {
            std::cerr << "Error: Duplicate port " << value << std::endl;
            return false;
        }
        server.set_Port(value);
        ++it;
    }
    
    return true;
}

bool check_host(std::vector<std::string> values,Server &server)
{
    if(values.empty())
        return false;
    std::vector<std::string>::iterator it = values.begin();
    while(it != values.end())
    {
        std::string value = *it;
        if (!valide_ip_address(value))
                    return false;
         server.set_IpAddress(value);
        ++it;
    }
    return true;
}

bool check_server_names(std::vector<std::string> values, Server &server)
{
    if (values.empty())
        return false;
        
    for (int i = 0 ; i < values.size() ;i++)
    {
        std::string name = values[i];
        if (name.empty())
            return false;
        if (std::find(server.serverNames.begin(), server.serverNames.end(), name) != server.serverNames.end()) {
            std::cerr << "Error: Duplicate server name " << name << std::endl;
            return false;
        }
        server.serverNames.push_back(name);
    }
    return true;
}

bool    check_error_page(std::vector<std::string> values,Server &server){
    
    if (values.size() != 2)
        return false;

    size_t i = 0;
    const std::string& code = values[0];
    
    for (int i  ; i < code.size() ;i++)  
    {
        char c = code[i];
        if (!isdigit(c))
        return false;
    }
    
    int error_code = std::atoi(code.c_str());
    if (error_code < 100 || error_code > 599)
        return false;
    
    server.errorPages[values[0]] = values[1];
    
    return true; 
}

bool check_client_max_body_size(std::vector<std::string> values,Server &server){
    if (values.size() != 1)
        return false;

    std::string value = values[0];
    size_t i;
    for (i = 0; i < value.size(); ++i)
    {
        if (!isdigit(value[i]))
            break;
    }

    if (i == 0)
        return false;
    if (i < value.size())
    {
        char unit = std::tolower(value[i]);
        if (unit != 'k' && unit != 'm' && unit != 'g')
            return false;
    
        if (i + 1 != value.size())
            return false;
    }
    server.MaxBodySize = value;
    return true;
}


bool    param_Syntaxe(std::string key, std::vector<std::string> values,Server &server){ // it = params.begin()
    

    if(key =="listen" && server.port.empty())
        return (check_listen(values,server));
    else if(key == "host"&& server.ip_address.empty())
        return(check_host(values,server) );
    else if(key == "server_name" && server.serverNames.empty())   
        return(check_server_names(values,server));
    else if( key == "error_page")
        return(check_error_page(values,server));
    else if(key =="client_max_body_size"&& server.MaxBodySize.empty() )
        return(check_client_max_body_size(values,server));
    else if( !server.port.empty() || !server.ip_address.empty() || !server.serverNames.empty() || !server.MaxBodySize.empty())
    {
        std::cout << "duplicated directive" << std::endl;
        return false ;
    }
    else
        return false;
}