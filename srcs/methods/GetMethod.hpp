#pragma once 

# include "../../_includes/client.hpp"
# include "../../_includes/Server.hpp"
#include <map>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <sys/wait.h>
#include <dirent.h>
using namespace std;

location* getClosestLocation(const Server& server, const std::string& requestPath);
void print_data(data_request& req);
bool existFile(const string &fullPath, location *loc);
string readFile(const string &fullPath);
std::string getMimeType(const std::string& path);
string handleGetRequest(data_request &req, location *loc, int currentFd);
// string handleGetRequest(data_request &req, location *loc, const Server &myServer, int currentFd);
string checkIndexes(location* loc, const string path);

int isDirectory(const std::string &fullPath);

bool isCgiRequest(location *loc, const std::string &path);
bool executeCgiScript(const data_request &req, const std::string &scriptPath, location *loc, std::string &output);
string switchLocation(const string &locPath, const string &reqPath, const string &rootVar);
std::string normalizePath(const std::string &path);

bool endsWith(const std::string& str, const std::string& suffix);
bool checkExtension(const std::string& url, const std::vector<std::string>& cgiExtensions);
bool isCgiConfigured(location* loc);
std::string getExtensions(const std::string &path);
std::string getCgiInterpreter(const std::string &scriptPath, location &loc);
bool executeCgi(const std::string &scriptPath, const data_request &req, std::string &output);
std::string buildHttpResponse(int statusCode, const std::string &statusMessage, const std::string &body);
std::string getExtention(data_request data);