#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include <fstream>
#include <sstream>
#include <time.h>
#include <memory>

#include "parser.hpp"
#include "Server.hpp"

#define    TIMEOUT 30

struct data_request
{
    std::string                         method;
    std::string                         path;
    std::string                         version;
    std::map<std::string, std::string>  headers;
    std::string                         bodyNameFile;
	std::string                         queryContent;
	struct location                     *myCloseLocation; 
    int                                 size_body;
    int                                 size_chunked;
    int                                 flag_chunked;
    int                                 is_chunked;
    int                                 flag_error;
    int                                 isCgi;
}typedef data_request;

struct data_response
{
	std::string                         startLine;
	std::map<std::string, std::string>  headers;
	std::string                         body;
	int                                 status_code;
	int                                 flaIsRedirect;
}typedef data_response;

class client
{
    public:
        std::string                         buffer;
        int                                 server_fd;
        data_request                        data_rq;
        int                                 flagProgress;
        std::string                         send_buffer;
        parser                              parc;
        Server                              myServer;
		bool                                closeConnection;
        static std::map<int, std::string>   errorPages;
        static std::map<int, std::string>   description;
		data_response                       data_rs;
        time_t                              lastActivityTime;
		size_t								sizeBody;
        std::string                         fileToSend;
        size_t                              bytesRemaining;
        bool                                headersSent;
        size_t                              fileSize;
        std::ifstream*                      fileStream;
        int                                 cgi_pid;
        int                                 cgi_fd;
        bool                                cgi_running;
        std::string                         cgi_buffer;
        time_t                              cgi_start_time;
        bool                                cgi_epoll_added;

        client();
        client(std::string buff, int fd);
        client(const client &obj);
        client &operator=(const client &obj);
        ~client();
        void        printClient();
        void        setBuffer(std::string str, ssize_t );
        int         checkRequestProgress();
		void        check_http_body_rules();
        void        parseRequest();
		std::string buildResponse();
		void        setDataResponse();
        void        handleResponse(int currentFd);
        void        sendFileChunk(int currentFd);
		void        setDescription();
		void        setErrorPages();
		void        setStatusCode();
        void        handleGetRequestWithChunking(int currentFd);
        void        handleCgiRequest();
        void        handleDirectoryRedirect();
};

std::string                         get_line(std::string str);
std::deque<std::string>             split(const std::string& str, const std::string& delimiter);
void                                post(const client &client, const Server& server);
Server                              *chooseServer(std::vector<Server*> &routeServer,std::string host);
int                                 readFileContent(const std::string& filePath, std::string &content);
template <typename T>
std::string                         to_string_98(T value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
std::string 						headersToOneString(std::map<std::string, std::string> headers);
std::string                         padLeftToThree(const std::string& input);
int                                 isRedirect(int red);
std::string 						RandomString(size_t len);
std::string 						getExtension(data_request data);
extern std::map<int ,std::vector<Server*> >  SocketToServers;
std::map<std::string, std::string>  createMimeTypeMap();