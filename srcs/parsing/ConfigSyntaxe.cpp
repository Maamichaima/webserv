#include "../../_includes/Tokenizer.hpp"
#include "../../_includes/Server.hpp"


bool   valide_port(const std::string& port){

    for(size_t i = 0 ; i < port.size() ;i++){
        char c = port[i];
        if(!isdigit(c))
            return false;
    }
    int port_num = std::atoi(port.c_str());
    if (port_num < 1 || port_num > 65535)
        return false;
    
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
    if(values.size() != 1)
        return false;
    server.set_IpAddress(values[0]);
    return true;
}

bool check_server_names(std::vector<std::string> values, Server &server)
{
    if (values.empty())
        return false;
        
    for (size_t i = 0 ; i < values.size() ;i++)
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

    const std::string& code = values[0];
    
    for (size_t i  = 0 ; i < code.size() ;i++)  
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

bool check_client_max_body_size(std::vector<std::string> values, Server &server) {
    if (values.size() != 1)
        return false;

    std::string value = values[0];
    size_t i;
    for (i = 0; i < value.size(); ++i) {
        if (!isdigit(value[i]))
            break;
    }

    if (i == 0)
        return false;
    
    std::string numeric_part = value.substr(0, i);
    
    char* endptr;
    errno = 0;
    unsigned long result = std::strtoul(numeric_part.c_str(), &endptr, 10);
    
    if (errno == ERANGE || *endptr != '\0') {
        return false;
    }
    
    size_t base_value = static_cast<size_t>(result);

    size_t multiplier = 1;
    if (i < value.size()) {
        char unit = std::tolower(value[i]);
        if (unit != 'k' && unit != 'm' && unit != 'g')
            return false;
        if (unit == 'k')
            multiplier = 1024;
        else if (unit == 'm')
            multiplier = 1024 * 1024;
        else if (unit == 'g')
            multiplier = 1024 * 1024 * 1024;
        if (i + 1 != value.size())
            return false;
    }
    
    if (base_value > SIZE_MAX / multiplier) {
        return false; 
    }
    
    server.MaxBodySize = base_value * multiplier;
    return true;
}


bool    param_Syntaxe(std::string key, std::vector<std::string> values, Server &server){

    if(key =="listen" && server.port.empty())
        return (check_listen(values,server));
    else if(key == "host" && server.ip_address.empty())
        return(check_host(values,server) );
    else if(key == "server_name" && server.serverNames.empty())   
        return(check_server_names(values,server));
    else if( key == "error_page")
        return(check_error_page(values,server));
    else if(key =="client_max_body_size" && server.MaxBodySize == 0 )
        return(check_client_max_body_size(values,server));
    else 
        return false;
}

bool check_all_keys(std::string key){

    if(key == "root" || key == "index" || key == "allowed_methods" || key == "autoindex" || 
        key == "redirect" || key == "cgi_extension" || key == "upload_store" || key =="cgi_path" ||
        key == "listen" || key == "server_name" || key == "client_max_body_size" || key == "error_page" || key =="host" )
        return true;
    else
        return false;
}

bool check_allowed_methods(std::vector<std::string> values)
{
    std::vector<std::string>::iterator it = values.begin();
    while(it != values.end())
    {
        std::string value = *it;
        if(value != "POST" && value != "GET" && value != "DELETE")
            return false;
        ++it;

    }
    return true;
}

bool check_autoindex(std::vector<std::string> values)
{
    if(values.size() != 1)
        return false;
    if(values[0] != "on" && values[0] != "off")
        return false;
    return true;
}


bool check_cgi_directives( std::map<std::string, std::vector<std::string> > infos )
{
    std::map<std::string, std::vector<std::string> >::iterator cgi_ext_it = infos.find("cgi_extension");
    std::map<std::string, std::vector<std::string> >::iterator cgi_path_it = infos.find("cgi_path");
    
    if (cgi_ext_it != infos.end() && cgi_path_it != infos.end()) 
        return cgi_ext_it->second.size() == cgi_path_it->second.size();
    
    bool cgi_ext_exists = (cgi_ext_it != infos.end());
    bool cgi_path_exists = (cgi_path_it != infos.end());
    
    if (cgi_ext_exists != cgi_path_exists) {
        return false; 
    }
    
    if (cgi_ext_exists && cgi_path_exists) {
        return cgi_ext_it->second.size() == cgi_path_it->second.size();
    }
    return true;
}



bool check_redirect(std::vector<std::string> values){

    if(values.size() != 2)
        return false;
    std::string code = values[0];
    for(size_t i = 0 ; i < code.size() ;i++){
        if(!isdigit(code[i]))
            return false;
    }
    return true;
}

bool Directives_syntaxe(std::string key, std::vector<std::string> values)
{
    
    if(key =="root" &&  values.size() != 1)
        return (false);
    else if(key == "allowed_methods" )   
        return(check_allowed_methods(values));
    else if( key == "autoindex")
        return(check_autoindex(values));
    else if(key == "redirect")
        return(check_redirect(values));
    else if(key == "upload_store" &&  values.size() != 1) 
        return(false);
    else
        return true;

}