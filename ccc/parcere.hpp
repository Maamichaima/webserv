#pragma once

#include <iostream>
struct data_request;
class client;
class parcere
{
    public:
        
    public:
        void parcHttpCall(client &client);
        parcere();
        parcere(const parcere &obj);
        parcere &operator=(const parcere &obj);
        ~parcere();
        int parce(client &client);
        void setDateToStruct(data_request &data_rq, std::string &buffer, int flag);
};
int parce_header(std::string str);
int pars_startligne(std::string str);
std::string get_line(std::string str);
#include "client.hpp"