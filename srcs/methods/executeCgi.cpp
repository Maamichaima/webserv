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
        
        // Set pipe to non-blocking
        int flags = fcntl(pipefd[0], F_GETFL, 0);
        fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);

        // Wait for CGI output with timeout (e.g., 5 seconds)
        fd_set readfds;
        struct timeval timeout;
        int status = 0;
        bool killed = false;
        output.clear();
        const int MAX_CGI_TIME = 5; // seconds

        time_t start = time(NULL);
        while (true) {
            FD_ZERO(&readfds);
            FD_SET(pipefd[0], &readfds);
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            int ret = select(pipefd[0] + 1, &readfds, NULL, NULL, &timeout);
            if (ret > 0 && FD_ISSET(pipefd[0], &readfds)) {
                char buffer[4096];
                ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer));
                if (bytesRead > 0) {
                    output.append(buffer, bytesRead);
                } else if (bytesRead == 0) {
                    // EOF
                    break;
                }
            }
            // Check if child exited
            pid_t w = waitpid(pid, &status, WNOHANG);
            if (w == pid) {
                break;
            }
            // Timeout
            if (time(NULL) - start > MAX_CGI_TIME) {
                kill(pid, SIGKILL);
                killed = true;
                break;
            }
        }
        close(pipefd[0]);
        // Ensure child is reaped
        waitpid(pid, &status, 0);

        if (killed || WEXITSTATUS(status) != 0)
            throw(500);
        
        return true;
    }
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

//