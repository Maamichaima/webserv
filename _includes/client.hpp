#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include <fstream>
#include <sstream>

#include "parser.hpp"
#include "Server.hpp"
#include <time.h>
// class parser;
#define TIMEOUT 180
struct data_request
{
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string bodyNameFile;
	std::string queryContent;
	struct location *myCloseLocation; 
    int size_body;
    int size_chunked;
    int flag_chunked;
    int is_chunked;
    int flag_error;
    int isCgi;
}typedef data_request;

struct data_response
{
	std::string startLine;
	std::map<std::string, std::string> headers;
	std::string body;
	int status_code;
	int flaIsRedirect;
}typedef data_response;

class client
{
    private:
    public:
        std::string buffer;
        // int ;
        int server_fd;
        data_request data_rq;
        int flag;
        std::string send_buffer;    // la réponse HTTP à envoyer
        // size_t send_offset = 0;     // combien d'octets ont été envoyés
        // bool headers_parsed = false;
        parser parc;
        Server myServer;
		bool closeConnection;
        static std::map<int, std::string> errorPages;
        static std::map<int, std::string> description;
		data_response data_rs;
        time_t lastActivityTime;

        client();
        client(std::string buff, int fd);
        client &operator=(const client &obj);
        ~client();
        void parceBody();
        void printClient();
        void setBuffer(std::string str, ssize_t );
        int checkRequestProgress();
		void check_http_body_rules();
        void parseRequest();
		std::string buildResponse();
		void setDataResponse();
		void handleResponse(int currentFd);
		void setDescription();
		void setErrorPages();
		void setStatusCode();
};

std::string get_line(std::string str);
std::deque<std::string> split(const std::string& str, const std::string& delimiter);

void post(const client &client, const Server& server);
Server     *chooseServer(std::vector<Server*> &routeServer,std::string host);
extern std::map<int ,std::vector<Server*> >  SocketToServers;