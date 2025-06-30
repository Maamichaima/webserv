#pragma once

#include <iostream>
#include <fstream>

#include "../RegEx/RegEx.hpp"
struct  data_request;
class   client;
class   Server;
class   parser
{   
    public:
        parser();
        parser(const parser &obj);
        parser &operator=(const parser &obj);
        ~parser();
        int parse(client &client);
        void setDateToStruct(client &clien, std::string &buffer);
};
int         parse_header(std::string str);
int         parse_startligne(std::string str);
std::string get_line(std::string str);
std::string trim(const std::string& str);
void        checkKeyValueContent(std::string key, std::string value);
void        toLower(std::string &str);
std::string get_line_size(std::string str, size_t size);
#include    "client.hpp"

