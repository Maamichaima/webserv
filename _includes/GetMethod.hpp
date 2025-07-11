#pragma once 

# include "client.hpp"
# include "Server.hpp"
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
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <iostream>

using std::cout;
using std::string;
using std::cerr;

#include <sys/wait.h>
#include <dirent.h>
using namespace std;

location* getClosestLocation(const Server& server, const std::string& requestPath);
void print_data(data_request& req);
bool existFile(const string &fullPath, location *loc);
string readFile(const string &fullPath);
std::string getMimeType(const std::string& extension);
string handleGetRequest(data_request &req, location *loc, client* clientObj);
string checkIndexes(location* loc, const string &path);

int isDirectory(const std::string &fullPath);

bool isCgiRequest(location *loc, const std::string &path);
bool executeCgiScript(const data_request &req, const std::string &scriptPath, location *loc, std::string &output);
string switchLocation(const string &locPath, const string &reqPath, const string &rootVar);
std::string normalizePath(const std::string &path);
std::string getExtensions(const std::string &path);
std::string buildHttpResponse(int statusCode, const std::string &statusMessage, const std::string &body);
std::string parseCgiOutput(const std::string &cgiOutput, bool &hasHeaders);
std::string buildCgiHttpResponse(const std::string &cgiOutput);
std::string getExtention(data_request data);
std::vector<char*> setupCgiEnvironment(const std::string &scriptPath, const data_request &req, const std::string &pathBody, std::vector<std::string> &envStrings);
