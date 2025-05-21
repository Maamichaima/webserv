#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include <fstream>
#include <sstream>

#include "parser.hpp"
#include "Server.hpp"
// class parser;
struct data_request
{
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::string body;
    int size_body;
    int size_chunked;
    int flag_chunked;
}typedef data_request;


class client
{
    private:
    public:
        std::string buffer;
        int fd_socket;
        int server_fd;
        data_request data_rq;
        int flag;
        parser parc;
        Server myServer;
        client();
        client(std::string buff, int fd);
        // client(const client &obj)
        // {

        // }
        client &operator=(const client &obj);
        ~client();
        // void parce_buffer();
        void parceBody();
        void printClient();
        void setBuffer(std::string str);
        int checkRequestProgress();
        void parseRequest();
};

std::string get_line(std::string str);
std::deque<std::string> split(const std::string& str, char delimiter);

void post(const client &client, const Server& server);