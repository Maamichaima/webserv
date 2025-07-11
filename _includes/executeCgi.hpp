#pragma once

#include "ServerManager.hpp"
#include "GetMethod.hpp"

#include <sys/wait.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <iostream>


bool                endsWith(const std::string& str, const std::string& suffix);
bool                checkExtension(const std::string& url, const std::vector<std::string>& cgiExtensions);
bool                isCgiConfigured(location* loc);
std::string         getExtensions(const std::string &path);
std::string         getCgiInterpreter(const std::string &scriptPath, location *loc);
bool                executeCgi(const std::string &scriptPath, const data_request &req, std::string &output);
std::string         parseCgiOutput(const std::string &cgiOutput, bool &hasHeaders);
std::string         buildCgiHttpResponse(const std::string &cgiOutput);
std::vector<char*>  setupCgiEnvironment(const std::string &scriptPath, const data_request &req, const std::string &pathBody, std::vector<std::string> &envStrings);