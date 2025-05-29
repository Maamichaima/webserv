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
using namespace std;

location* getClosestLocation(const Server& server, const std::string& requestPath);
void print_data(data_request& req);
bool existFile(const string &fullPath);
string readFile(const string &fullPath);
std::string getMimeType(const std::string& path);
string handleGetRequest(data_request &req, location *loc, const Server &myServer);