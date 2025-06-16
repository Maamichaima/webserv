#pragma once

#include <iostream>
#include <fstream>

#include "../RegEx/RegEx.hpp"
struct data_request;
class client;
class Server;
class parser
{
    public:
        
    public:
        void parcHttpCall(client &client);
        parser();
        parser(const parser &obj);
        parser &operator=(const parser &obj);
        ~parser();
        int parse(client &client);
        void setDateToStruct(client &clien, std::string &buffer, int flag);
};
int parse_header(std::string str);
int parse_startligne(std::string str);
std::string get_line(std::string str);

#include "client.hpp"

