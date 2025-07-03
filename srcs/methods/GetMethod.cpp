#include "../../_includes/ServerManager.hpp"
#include "GetMethod.hpp"
#include <sys/wait.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <iostream>

using std::cout;

std::string normalizePath(const std::string &path) {
  std::string result;
  bool slashSeen = false;

  for (size_t i = 0; i < path.size(); ++i) {
    if (path[i] == '/') {
      if (!slashSeen) {
        result += '/';
        slashSeen = true;
      }
    } else {
      result += path[i];
      slashSeen = false;
    }
  }

  // Supprimer slash final sauf si c'est la racine "/"
  if (result.size() > 1 && result[result.size() - 1] == '/')
    result.erase(result.size() - 1);

  return result;
}

location *getClosestLocation(const Server &server, const std::string &requestPath) {
    location *bestMatch = NULL;
    size_t bestLength = 0;

    std::string normRequest = normalizePath(requestPath);

    for (std::map<std::string, location>::const_iterator it = server.locations.begin();
        it != server.locations.end(); ++it)
    {
        std::string normLocPath = normalizePath(it->first);

        if (normRequest.find(normLocPath) == 0 &&
            (normRequest.size() == normLocPath.size() ||
                normRequest[normLocPath.size()] == '/' || normLocPath == "/")) {

            if (normLocPath.size() > bestLength) {
            bestLength = normLocPath.size();
            bestMatch = const_cast<location *>(&it->second);
            }
        }
    }

    return bestMatch;
}

std::string listDirectory(const std::string& path, string reqPath) {
    DIR* dir = opendir(path.c_str());
    std::string html = "<html><body>\n";
    html += "<h1>Index of " + path + "</h1>\n";
    html += "<ul>\n";

    if (dir == NULL)
        throw(500);
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;

        if (name == "." || name == "..")
            continue;

        html += "<li><a href=\"" + reqPath  + "/" + name + "\">" +path+"|"+ name + "</a></li>\n";
    }

    closedir(dir);
    html += "</ul>\n</body></html>\n";
    return html;
}

int isDirectory(const std::string &fullPath) {
    struct stat info;
    if (stat(fullPath.c_str(), &info) != 0)
        return 0; // Error
    return S_ISDIR(info.st_mode);
}

/// @brief existFiles function check fullPath is file and path is correct
/// @param S_ISREG is a macro for check file is regular or not (wax file ou la directory ou xi 3jab akhur) n9adro ndirouha ta b access !
/// @param stat functin kay jib linformations 3la file (kay9lb 3lih ou sf)
/// @return 0 for success -1 for error
bool existFile(string &path, location *loc)
{
    struct stat st;
    int check_st = stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode);
    if (check_st == 0)
    {
        if (isDirectory(path))
        {
            string checkIndex = checkIndexes(loc, path + "/");
            if (checkIndex == "")
                return false;
            else
                path = checkIndex;
        }
    }

    return(stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode));
}
        
/// @brief 
/// @param ios::binary : open mode bax bax maytbadalx lcontent dial file in read operation  
/// @param ios::in : open mode for readed file
string readFile(const string &path)
{
    std::ifstream file(path.c_str(), ios::in | ios::binary);
    if (!file) return "";
    std::ostringstream fileContent;
    fileContent << file.rdbuf();
    return fileContent.str();
}

// diro map 
std::string getMimeType(const std::string& path) {
    if (path.length() >= 5 && path.substr(path.length() - 5) == ".html") return "text/html";
    if (path.length() >= 4 && path.substr(path.length() - 4) == ".css") return "text/css";
    if (path.length() >= 4 && path.substr(path.length() - 4) == ".php") return "text/php";
    if (path.length() >= 3 && path.substr(path.length() - 3) == ".js") return "application/javascript";
    if (path.length() >= 4 && path.substr(path.length() - 4) == ".png") return "image/png";
    if (path.length() >= 4 && path.substr(path.length() - 4) == ".jpg") return "image/jpeg";
    if (path.length() >= 5 && path.substr(path.length() - 4) == ".mp4") return "video/mp4";
    return "text/plain";
}

//////////////////////////////////////////CGI////////////////////////////////////////////
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

bool executeCgi(const std::string &scriptPath, const data_request &req, std::string &output) {
    std::string pathBody =  req.bodyNameFile;
    
    int pipefd[2];
    int inputPipe[2];
    
    if (pipe(pipefd) == -1)
        return false;
    
    // Create input pipe for POST data
    bool needInputPipe = (req.method == "POST" && !pathBody.empty());
    if (needInputPipe && pipe(inputPipe) == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        return false;
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        close(pipefd[0]);
        close(pipefd[1]);
        if (needInputPipe) {
            close(inputPipe[0]);
            close(inputPipe[1]);
        }
        return false;
    }

    if (pid == 0) {
        // Redirect stdout to pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        
        // For POST, redirect stdin from input pipe
        if (needInputPipe) {
            dup2(inputPipe[0], STDIN_FILENO);
            close(inputPipe[0]);
            close(inputPipe[1]);
        }

        std::string cgiPath = getCgiInterpreter(scriptPath, req.myCloseLocation);
        if (cgiPath.empty())
            std::exit(1);

        char *argv[] = {
            (char*)cgiPath.c_str(),     
            (char*)scriptPath.c_str(),  
            NULL
        };

        std::vector<std::string> envStrings;
        std::vector<char*> envp = setupCgiEnvironment(scriptPath, req, pathBody, envStrings);
        
        execve(cgiPath.c_str(), argv, &envp[0]);
        std::exit(1);
    } 
    else {
        close(pipefd[1]);
        
        if (needInputPipe) {
            close(inputPipe[0]);
            
            std::ifstream bodyFile(pathBody.c_str(), std::ios::binary);
            if (bodyFile.is_open()) {
                char buffer[4096];
                while (bodyFile.read(buffer, sizeof(buffer) - 1) || bodyFile.gcount() > 0) {
                    std::streamsize bytesRead = bodyFile.gcount();
                    buffer[bytesRead] = '\0';
                    ssize_t written = write(inputPipe[1], buffer, bytesRead);
                    if (written != bytesRead)
                        break;
                }
                bodyFile.close();
            }
            close(inputPipe[1]);
        }
        
        // Read CGI output
        char buffer[4096];
        ssize_t bytesRead;
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            output.append(buffer, bytesRead);
        }
        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);

        if (WEXITSTATUS(status) != 0)
            throw(500);
        
        return true;
    }
}

std::string buildHttpResponse(int statusCode, const std::string &statusMessage, const std::string &body) {
    std::string response;

    response = "HTTP/1.1 " + to_string_98(statusCode) + " " + statusMessage + "\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + to_string_98(body.size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;

    return response;
}

/////////////////////////////////////////////////////////////////////////////////////////

string checkIndexes(location* loc, const string path) {
    std::vector<std::string>* indexes = loc->getInfos("index");
    if (!indexes)
    {
        cerr << "Error: No index files defined for location " << loc->path << endl;
        return "";
    }

    for (size_t i = 0; i < indexes->size(); ++i) {
        std::string fullPath = path + indexes->at(i);
        std::ifstream file(fullPath.c_str());
        if (file.is_open()) {
            file.close();
            return fullPath;
        }
    }
    return "";
}

string switchLocation(const string &locPath, const string &reqPath, const string &rootVar) {
    if (reqPath.compare(0, locPath.length(), locPath) == 0) {
        string suffix = reqPath.substr(locPath.length());

        if (!rootVar.empty() && rootVar[rootVar.length() - 1] == '/' &&
            !suffix.empty() && suffix[0] == '/') {
            return rootVar + suffix.substr(1);
        }
        return rootVar + suffix;
    }
    return reqPath;
}



size_t getFileSize(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return st.st_size;
    }
    return 0;
}

void client::sendFileChunk(int currentFd) {
    const size_t CHUNK_SIZE = 8192;
    char buffer[CHUNK_SIZE];
    
    size_t toRead = std::min(CHUNK_SIZE, this->bytesRemaining);
    if (!this->fileStream->is_open()) {
        this->closeConnection = true;
        return;
    }
    this->fileStream->read(buffer, toRead);
    std::streamsize bytesRead = this->fileStream->gcount();
    
    
    if (bytesRead <= 0) {
        if (this->fileStream->is_open()) {
            this->fileStream->close();
        }
        this->closeConnection = true;
        return;
    }
    
    ssize_t bytesSent = send(currentFd, buffer, bytesRead, MSG_NOSIGNAL);
    
    if (bytesSent > 0) {
        this->bytesRemaining -= bytesSent;
        
        if (bytesSent < bytesRead) {
            this->fileStream->seekg(-(bytesRead - bytesSent), std::ios::cur);
            this->bytesRemaining += (bytesRead - bytesSent);
        }
    } 
    else if (bytesSent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        this->fileStream->seekg(-bytesRead, std::ios::cur);
        return;
    }
    else {
        if (this->fileStream->is_open()) {
            this->fileStream->close();
        }
        this->closeConnection = true;
        return;
    }
    
    // Si tout le fichier est envoyé
    if (this->bytesRemaining == 0) {
        if (this->fileStream->is_open()) {
            this->fileStream->close();
        }
        this->closeConnection = true;
    }
}

string handleGetRequest(data_request &req, location *loc, client* clientObj = NULL)
{
    string locPath = normalizePath(loc->path);
    string reqPath = normalizePath(req.path);
    string rootVar;

    std::map<std::string, std::vector<std::string> >::iterator itRoot = loc->infos.find("root");
    if(itRoot == loc->infos.end())
        throw(404);
    rootVar = loc->getInfos("root")->at(0) + "/";
    string path = switchLocation(locPath, reqPath, rootVar);

    // Handle directory cases
    if (isDirectory(path)) {
        string indexFound = checkIndexes(loc, path + "/");
        if (indexFound == "") {
            if (loc->getInfos("autoindex") && loc->getInfos("autoindex")->at(0) == "on") {
                std::string body = listDirectory(path, reqPath);
                std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + to_string_98(body.size()) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" +
                    body;
                return response;
            } 
            else
                throw(403);
        } else {
            path = indexFound;
        }
    }

    // Check if file exists
    struct stat st;
    if (stat(path.c_str(), &st) != 0 || !S_ISREG(st.st_mode)) {
        throw(404);
    }

    size_t fileSize = getFileSize(path);
    const size_t CHUNKED_THRESHOLD = 1024 * 1024; // 1MB threshold
    
    // Handle large files with chunked transfer
    if (fileSize > CHUNKED_THRESHOLD && clientObj != NULL) {
        clientObj->fileStream->open(path.c_str(), std::ios::in | std::ios::binary);
        if (!clientObj->fileStream->is_open())
            throw(500);
        
        clientObj->bytesRemaining = fileSize;
        clientObj->fileSize = fileSize;
        clientObj->fileToSend = path;
        clientObj->closeConnection = false;
        
        std::string mimeType = getMimeType(path);
        
        std::ostringstream headers;
        headers << "HTTP/1.1 200 OK\r\n";
        headers << "Content-Type: " << mimeType << "\r\n";
        headers << "Content-Length: " << fileSize << "\r\n";
        headers << "Accept-Ranges: bytes\r\n";
        headers << "Connection: close\r\n\r\n";
        
        return headers.str();
    }
    
    // Handle small files - load entirely into memory
    std::string body = readFile(path);
    if (body == "")
        throw(500);
            
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: " << getMimeType(path) << "\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    response << "Connection: close\r\n\r\n";
    response << body;
    
    return response.str();    
}

std::string client::prepareGetResponse(data_request &req, location *loc)
{
    string locPath = normalizePath(loc->path);
    string reqPath = normalizePath(req.path);
    string rootVar;

    std::map<std::string, std::vector<std::string> >::iterator itRoot = loc->infos.find("root");
    if(itRoot == loc->infos.end())
        throw(404);
    rootVar = loc->getInfos("root")->at(0) + "/";
    string path = switchLocation(locPath, reqPath, rootVar);

    if (isDirectory(path)) {
        string indexFound = checkIndexes(loc, path + "/");
        if (indexFound == "") {
            if (loc->getInfos("autoindex") && loc->getInfos("autoindex")->at(0) == "on") {
                std::string body = listDirectory(path, reqPath);
                std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + to_string_98(body.size()) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" +
                    body;
                return response;
            } 
            else
                throw(403);
        } else {
            path = indexFound;
        }
    }
    
    struct stat st;
    if (stat(path.c_str(), &st) != 0 || !S_ISREG(st.st_mode)) {
        throw(404);
    }

    size_t fileSize = getFileSize(path);
    
    const size_t CHUNKED_THRESHOLD = 1024 * 1024;
    
    if (fileSize > CHUNKED_THRESHOLD) {
        
        this->fileStream->open(path.c_str(), std::ios::in | std::ios::binary);
        if (!this->fileStream->is_open())
            throw(500);
        
        this->bytesRemaining = fileSize;
        this->fileSize = fileSize;
        this->fileToSend = path;
        this->closeConnection = false;
        
        std::string mimeType = getMimeType(path);
        
        std::ostringstream headers;
        headers << "HTTP/1.1 200 OK\r\n";
        headers << "Content-Type: " << mimeType << "\r\n";
        headers << "Content-Length: " << fileSize << "\r\n";
        headers << "Accept-Ranges: bytes\r\n";
        headers << "Connection: close\r\n\r\n";
        
        return headers.str();
    }
    
    return "";
}

std::string parseCgiOutput(const std::string &cgiOutput, bool &hasHeaders) {
    hasHeaders = false;
    
    // Look for the double CRLF that separates headers from body
    size_t headerEnd = cgiOutput.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        headerEnd = cgiOutput.find("\n\n");
        if (headerEnd == std::string::npos) {
            // No headers found, return as is
            return cgiOutput;
        }
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

//////////////////////////////////////////CGI////////////////////////////////////////////

std::vector<char*> setupCgiEnvironment(const std::string &scriptPath, const data_request &req, const std::string &pathBody, std::vector<std::string> &envStrings) {
    envStrings.clear();
    
    // Basic CGI environment
    envStrings.push_back("GATEWAY_INTERFACE=CGI/1.1");
    envStrings.push_back("REQUEST_METHOD=" + req.method);
    envStrings.push_back("SCRIPT_NAME=" + scriptPath);
    envStrings.push_back("PATH_INFO=" + req.path);
    envStrings.push_back("SERVER_PROTOCOL=HTTP/1.1");
    envStrings.push_back("REDIRECT_STATUS=200");
    
    // Method-specific environment variables
    if (req.method == "GET") {
        envStrings.push_back("QUERY_STRING=" + req.queryContent);
    }
    else if (req.method == "POST") {
        struct stat st;
        size_t contentLength = 0;
        if (stat(pathBody.c_str(), &st) == 0)
            contentLength = st.st_size;
        
        envStrings.push_back("CONTENT_LENGTH=" + to_string_98(contentLength));
        
        // Set Content-Type from headers if available
        std::map<std::string, std::string>::const_iterator contentTypeIt = req.headers.find("content-type");
        if (contentTypeIt != req.headers.end())
            envStrings.push_back("CONTENT_TYPE=" + contentTypeIt->second);
        else
            envStrings.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
        
        // Also include query string for POST if it exists
        envStrings.push_back("QUERY_STRING=" + req.queryContent);
    }
    
    // Add cookies to environment if present
    std::map<std::string, std::string>::const_iterator cookieIt = req.headers.find("cookie");
    if (cookieIt != req.headers.end())
        envStrings.push_back("HTTP_COOKIE=" + cookieIt->second);
    
    // Add HTTP headers as environment variables
    for (std::map<std::string, std::string>::const_iterator it = req.headers.begin(); 
        it != req.headers.end(); ++it) {
        std::string headerName = "HTTP_" + it->first;
        // Convert to uppercase and replace - with _
        for (size_t i = 0; i < headerName.length(); ++i) {
            headerName[i] = std::toupper(headerName[i]);
            if (headerName[i] == '-') headerName[i] = '_';
        }
        // Skip cookie header since we already handled it specifically above
        if (it->first != "cookie")
            envStrings.push_back(headerName + "=" + it->second);
    }
    
    // Convert strings to char* array
    std::vector<char*> envp;
    for (size_t i = 0; i < envStrings.size(); ++i) {
        envp.push_back(const_cast<char*>(envStrings[i].c_str()));
    }
    envp.push_back(NULL);
    
    return envp;
}
