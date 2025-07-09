#include "../../_includes/executeCgi.hpp"
#include <fcntl.h>
#include <signal.h>

bool endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) return false;

    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool checkExtension(const std::string& url, const std::vector<std::string>& cgiExtensions) {
    for (std::vector<std::string>::const_iterator it = cgiExtensions.begin(); it != cgiExtensions.end(); ++it) {
        if ((*it).size() > 1 && (*it)[0] == '.' && endsWith(url, *it)) {
            return true;
        }
    }

    return false;
}

bool isCgiConfigured(location* loc) {
    if (!loc)
        return false;

    const std::vector<std::string>* extensions = loc->getInfos("cgi_extension");
    const std::vector<std::string>* paths = loc->getInfos("cgi_path");

    if (!extensions || extensions->empty())
        return false;
    if (!paths || paths->empty())
        return false;

    return true;
}

std::string getExtensions(const std::string &path) {
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos)
        return ""; // No extension

    return path.substr(dot);
}

std::string getCgiInterpreter(const std::string &scriptPath, location *loc) {
    std::vector<std::string>* extensions = loc->getInfos("cgi_extension");
    std::vector<std::string>* interpreters = loc->getInfos("cgi_path");

    if (!extensions || !interpreters) {
        return "";
    }

    std::string ext = getExtensions(scriptPath);
    for (size_t i = 0; i < extensions->size(); ++i) {
        if ((*extensions)[i] == ext)
            return (*interpreters)[i];
    }

    return "";
}

std::string parseCgiOutput(const std::string &cgiOutput, bool &hasHeaders) {
    hasHeaders = false;
    
    size_t headerEnd = cgiOutput.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        headerEnd = cgiOutput.find("\n\n");
        if (headerEnd == std::string::npos)
            return cgiOutput;
    }
    
    std::string headerPart = cgiOutput.substr(0, headerEnd);
    
    if (headerPart.find("Content-Type:") != std::string::npos ||
        headerPart.find("Location:") != std::string::npos ||
        headerPart.find("Set-Cookie:") != std::string::npos ||
        headerPart.find("Status:") != std::string::npos ||
        headerPart.find(":") != std::string::npos) {
        hasHeaders = true;
        return cgiOutput;
    }
    
    return cgiOutput;
}

std::string buildCgiHttpResponse(const std::string &cgiOutput) {
    bool hasHeaders = false;
    std::string processedOutput = parseCgiOutput(cgiOutput, hasHeaders);
    
    if (hasHeaders) {
        if (processedOutput.find("HTTP/1.1") == 0) {
            return processedOutput;
        } else {
            return "HTTP/1.1 200 OK\r\n" + processedOutput;
        }
    } else {
        std::string response;
        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Content-Length: " + to_string_98(processedOutput.size()) + "\r\n";
        response += "Connection: close\r\n";
        response += "\r\n";
        response += processedOutput;
        return response;
    }
}

std::vector<char*> setupCgiEnvironment(const std::string &scriptPath, const data_request &req, const std::string &pathBody, std::vector<std::string> &envStrings) {
    envStrings.clear();
    
    envStrings.push_back("GATEWAY_INTERFACE=CGI/1.1");
    envStrings.push_back("REQUEST_METHOD=" + req.method);
    envStrings.push_back("SCRIPT_NAME=" + scriptPath);
    envStrings.push_back("PATH_INFO=" + req.path);
    envStrings.push_back("SERVER_PROTOCOL=HTTP/1.1");
    envStrings.push_back("REDIRECT_STATUS=200");
    
    if (req.method == "GET")
        envStrings.push_back("QUERY_STRING=" + req.queryContent);
    else if (req.method == "POST") {
        struct stat st;
        size_t contentLength = 0;
        if (stat(pathBody.c_str(), &st) == 0)
            contentLength = st.st_size;
        
        envStrings.push_back("CONTENT_LENGTH=" + to_string_98(contentLength));
        
        std::map<std::string, std::string>::const_iterator contentTypeIt = req.headers.find("content-type");
        if (contentTypeIt != req.headers.end())
            envStrings.push_back("CONTENT_TYPE=" + contentTypeIt->second);
        else
            envStrings.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
        
        envStrings.push_back("QUERY_STRING=" + req.queryContent);
    }
    
    std::map<std::string, std::string>::const_iterator cookieIt = req.headers.find("cookie");
    if (cookieIt != req.headers.end())
        envStrings.push_back("HTTP_COOKIE=" + cookieIt->second);
    
    for (std::map<std::string, std::string>::const_iterator it = req.headers.begin(); 
        it != req.headers.end(); ++it) {
        std::string headerName = "HTTP_" + it->first;
        for (size_t i = 0; i < headerName.length(); ++i) {
            headerName[i] = std::toupper(headerName[i]);
            if (headerName[i] == '-') headerName[i] = '_';
        }
        if (it->first != "cookie")
            envStrings.push_back(headerName + "=" + it->second);
    }
    
    std::vector<char*> envp;
    for (size_t i = 0; i < envStrings.size(); ++i) {
        envp.push_back(const_cast<char*>(envStrings[i].c_str()));
    }
    envp.push_back(NULL);
    
    return envp;
}

void client::handleCgiRequest(int currentFd)
{
    if (this->data_rq.method == "DELETE" || !this->data_rq.myCloseLocation)
        return;
        
    if (!isCgiConfigured(this->data_rq.myCloseLocation))
        return;
        
    string locPath = normalizePath(this->data_rq.myCloseLocation->path);
    string reqPath = normalizePath(data_rq.path);
    string root;
    vector<string>* exts;

    std::map<std::string, std::vector<std::string> >::iterator itRoot = this->data_rq.myCloseLocation->infos.find("root");
    if(itRoot == this->data_rq.myCloseLocation->infos.end()) 
        throw(404);
    
    root = this->data_rq.myCloseLocation->getInfos("root")->at(0);
    if (root[root.size() - 1] != '/') {
        root += "/";
    }
    
    string cgiPath = switchLocation(locPath, reqPath, root);
    std::map<std::string, std::vector<std::string> >::iterator itCgi = this->data_rq.myCloseLocation->infos.find("cgi_extension");
    if(itCgi == this->data_rq.myCloseLocation->infos.end()) 
        throw(404);
    exts = this->data_rq.myCloseLocation->getInfos("cgi_extension");
    
    if (isDirectory(cgiPath) && checkIndexes(this->data_rq.myCloseLocation, cgiPath + "/") != "")
        cgiPath = checkIndexes(this->data_rq.myCloseLocation, cgiPath+ "/");					
    
    if (!checkExtension(cgiPath, *exts))
        return;
        
    if (access(cgiPath.c_str(), F_OK) != 0)
        throw(404);
    
    if (access(cgiPath.c_str(), R_OK) != 0)
        throw(403);
    
    // --- Start CGI asynchronously ---
    if (!cgi_running) {
        int pipefd[2];
        int inputPipe[2];
        bool needInputPipe = (data_rq.method == "POST" && !data_rq.bodyNameFile.empty());
        
        if (pipe(pipefd) != 0)
            throw(500);
        if (needInputPipe && pipe(inputPipe) != 0) {
            close(pipefd[0]);
            close(pipefd[1]);
            throw(500);
        }
        cgi_pid = fork();
        if (cgi_pid == 0) {
            if (access(cgiPath.c_str(), F_OK) != 0)
                std::exit(1);

            if (access(cgiPath.c_str(), R_OK) != 0)
                std::exit(1);
            
            std::string cgiInterp = getCgiInterpreter(cgiPath, data_rq.myCloseLocation);
            if (cgiInterp.empty())
                std::exit(1);
            
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);
            // For POST, redirect stdin from input pipe
            if (needInputPipe) {
                dup2(inputPipe[0], STDIN_FILENO);
                close(inputPipe[0]);
                close(inputPipe[1]);
            }
            char *argv[] = {
                (char*)cgiInterp.c_str(),
                (char*)cgiPath.c_str(),
                NULL
            };
            std::vector<std::string> envStrings;
            std::vector<char*> envp = setupCgiEnvironment(cgiPath, data_rq, data_rq.bodyNameFile, envStrings);
            
            execve(cgiInterp.c_str(), argv, &envp[0]);
            std::exit(1);
        } else if (cgi_pid > 0) {
            close(pipefd[1]);
            if (needInputPipe) {
                close(inputPipe[0]);
                // Write POST body to CGI stdin
                std::ifstream bodyFile(data_rq.bodyNameFile.c_str(), std::ios::binary);
                if (bodyFile.is_open()) {
                    char buffer[4096];
                    while (bodyFile.read(buffer, sizeof(buffer)) || bodyFile.gcount() > 0) {
                        ssize_t bytesRead = bodyFile.gcount();
                        if (write(inputPipe[1], buffer, bytesRead) != bytesRead)
                            break;
                    }
                    bodyFile.close();
                }
                close(inputPipe[1]);
            }
            cgi_fd = pipefd[0];
            cgi_running = true;
            cgi_start_time = std::time(NULL);
    
            throw(0);
        } else {
            // fork failed
            if (needInputPipe) {
                close(inputPipe[0]);
                close(inputPipe[1]);
            }
            close(pipefd[0]);
            close(pipefd[1]);
            throw(500);
        }
    }
    throw(0);
}

void client::handleDirectoryRedirect(int currentFd)
{
    if (data_rq.path[data_rq.path.size() - 1] == '/' || !this->data_rq.myCloseLocation)
        return;
        
    string locPath = normalizePath(this->data_rq.myCloseLocation->path);
    string reqPath = normalizePath(data_rq.path);
    string root;
    
    std::map<std::string, std::vector<std::string> >::iterator itRoot = this->data_rq.myCloseLocation->infos.find("root");
    if(itRoot == this->data_rq.myCloseLocation->infos.end()) 
        throw(404);

    root = this->data_rq.myCloseLocation->getInfos("root")->at(0) + "/";          
    string resendPath = switchLocation(locPath, reqPath, root);
    string indexFound = checkIndexes(this->data_rq.myCloseLocation, resendPath + "/");
    
    if (isDirectory(resendPath) && indexFound != "") {
        std::string newLocation = data_rq.path + "/";
        std::string response = 
            "HTTP/1.1 301 Moved Permanently\r\n"
            "Location: " + newLocation + "\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n"
            "\r\n";
        // cout << "inResend************************************************" << endl;
        send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);
        // closeConnection = true;
        throw(0); 
    }
}
