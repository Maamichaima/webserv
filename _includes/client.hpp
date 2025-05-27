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
    ///
    ///
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::string bodyNameFile;
    int size_body;
    int size_chunked;
    int flag_chunked;
    int is_chunked;
    int flag_error;
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
            std::string send_buffer;    // la réponse HTTP à envoyer
        size_t send_offset = 0;     // combien d'octets ont été envoyés
        bool headers_parsed = false;
        parser parc;
        Server myServer;
        static std::map<int, std::string> errorPages;
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
        void setBuffer(std::string str, ssize_t );
        int checkRequestProgress();
        void parseRequest();
};

std::string get_line(std::string str);
std::deque<std::string> split(const std::string& str, char delimiter);
void post(const client &client, const Server& server);
Server*     chooseServer(std::vector<Server*> &routeServer,std::string host);
extern std::vector<Server*> routeServer;