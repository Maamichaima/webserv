#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <deque>

struct data_request
{
    std::string method;
    std::string path;
    std::map<std::string, std::string> headrs;
}typedef data_request;


class client
{
    private:
        std::string buffer;
        int fd_socket;
        data_request data_rq;
    public:
        client();
        client(std::string buff, int fd);
        client(const client &obj);
        client &operator=(const client &obj);
        ~client();
        void parce_buffer();
        void setDateToStruct();
        void printClient();
};

std::string get_line(std::string str);
std::deque<std::string> split(const std::string& str, char delimiter);