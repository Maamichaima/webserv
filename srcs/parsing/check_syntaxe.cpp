#include "../../_includes/tokenizer.hpp"
#include "../../_includes/Server.hpp"


bool   validate_port(const std::string& port){

    for(char c : port){
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

    for(const std::string octet : octets)
    {
        if(octet.empty())
            return false;
        for (char c : octet)
        {
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
        size_t colon_pos = value.find(':');

        if(colon_pos != std::string::npos)
        {
            std::string host = value.substr(0, colon_pos);
            std::string port = value.substr(colon_pos + 1);
            if (!host.empty()) // listen :PORT
            {   
                if (!valide_ip_address(host))
                    return false;
                server.set_IpAddress(host);
            }
            else
                return false;
            if(!port.empty())
            {
                if (!validate_port(port))
                    return false;
                server.set_Port(port);
            }
            else
                return false;
        }
        else
        {
            bool is_port = true; 
            for (char c : value)
            {
                if (!isdigit(c))
                {
                    is_port = false;
                    break;
                }
            }
            if (is_port)
            {
                if (!validate_port(value))
                    return false;
                server.set_Port(value);
            }
            else
            {    if (!valide_ip_address(value))
                    return false;
                server.set_IpAddress(value);
            }
        }
        ++it;
    }
    return true;
}

bool check_server_names(std::vector<std::string> values)
{
    if (values.empty())
        return false;
        
    for (std::string name : values)
    {
        if (name.empty())
            return false;
    }
    
    return true;
}

bool    check_error_page(std::vector<std::string> values){
    
    if (values.size() < 2)
        return false;
    size_t i = 0;
    while (i < values.size())
    {
        size_t start = i;
        while (i < values.size() && !values[i].empty() && values[i][0] != '/')
        {
            const std::string& code = values[i];
            
            for (char c : code)
            {
                if (!isdigit(c))
                    return false;
            }
            
            int error_code = std::atoi(code.c_str());
            if (error_code < 100 || error_code > 599)
                return false;
            
            i++;
        }
        
        if (i == start)
            return false;
            
        if (i >= values.size() || values[i].empty() || values[i][0] != '/')
            return false;
        i++;
    }
    
    return true;
}

bool check_client_max_body_size(std::vector<std::string> values){
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
    return true;
}


bool    param_Syntaxe(std::string key, std::vector<std::string> values,Server &server){ // it = params.begin()
    

    if(key =="listen"){
        return (check_listen(values,server));
    }
    else if(key == "server_name" || key == "server_names" ){ //Setup the server_names or not.
        return(check_server_names(values));
    }
    else if( key == "error_page")
    {
        return(check_error_page(values));

    }
    else if(key =="client_max_body_size" )
        return(check_client_max_body_size(values));
    else
        return false;
}