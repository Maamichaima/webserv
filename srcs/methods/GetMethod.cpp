#include "../../_includes/ServerManager.hpp"
#include "GetMethod.hpp"
#include <sys/wait.h>
#include <dirent.h>

// khass rihab txouf fin dirha
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



void print_data(data_request& req) 
{
    std::cout << "===== HEADERS RECEIVED =====" << std::endl;   
    
    for (std::map<std::string, std::string>::const_iterator it = req.headers.begin(); it != req.headers.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
}

#include <iostream>
#include <string>
#include <dirent.h>

std::string listDirectory(const std::string& path, string reqPath) {
    cout << "path in listDirect: " << path << endl;
    DIR* dir = opendir(path.c_str());
    std::string html = "<html><body>\n";
    html += "<h1>Index of " + path + "</h1>\n";
    html += "<ul>\n";
    // path = switchLocation();
    if (dir == NULL) {
        cout << " Cannot open directory." << endl;
        return NULL;
    }
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

string removeLocation(string& input, const string& prefix) {
    size_t pos = 0;

    while (pos < input.size() && input[pos] == '/')
        pos++;
    input = string("/") + input;
    if (input.substr(pos, prefix.size()) == prefix)
        pos += prefix.size();

    return input.substr(pos);
}

std::string cleanPath(const std::string& input, const std::string& prefix) {
    size_t start = input.find_first_not_of('/');
    if (start == std::string::npos)
        return "";

    std::string cleaned = input.substr(start);
    cout << "cleaned: " << cleaned << endl;
    if (cleaned.compare(0, prefix.size(), prefix) == 0) {
        cout << "is here !!" << endl;
        return cleaned.substr(prefix.size());
    }

    return cleaned;
}

/// @brief existFiles function check fullPath is file and path is correct
/// @param S_ISREG is a macro for check file is regular or not (wax file ou la directory ou xi 3jab akhur) n9adro ndirouha ta b access !
/// @param stat functin kay jib linformations 3la file (kay9lb 3lih ou sf)
/// @return 0 for success -1 for error
bool existFile(string &path, location *loc, string reqPath, string locPath, int currentFd)
{
    struct stat st;
    int check_st = stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode);
    if (check_st == 0)
    {
        if (isDirectory(path))
        {
            string check = checkIndexes(loc, path + "/");
            cout << "check: " << check << endl;
            if (check == "")
            {
                // string paths = removeLocation(reqPath, locPath + "/");
                // string path = removeLocation(reqPath, locPath + "/");
                cout << "path in existFile: "<< path << endl;
                std::string body = listDirectory(path, reqPath);
                // cout << "body : "<< body << endl;
                std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + std::to_string(body.size()) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" +
                    body;
                send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);   
            }
            else
                path = check;
        }
    }
    return(stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode));
}
        
/// @brief 
/// @param ios::binary : open mode bax bax maytbadalx lcontent dial file in read operation  
/// @param ios::in : open mode for readed file
string readFile(const string &fullPath)
{
    std::ifstream file(fullPath.c_str(), ios::in | ios::binary);
    if (!file) return "error opening !!";
    else cout << "opning file !!\n" ;
    std::ostringstream fileContent;
    fileContent << file.rdbuf();
    return fileContent.str();
}

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

string executeCgi(const string path, string query, string interpreter) {
    int fd[2];
    pid_t pid;
    if(pipe(fd) == -1) {perror("pipe"); return NULL;}
    pid = fork();
    if (pid == -1){perror("pipe"); return NULL;}
    if (pid == 0)
    {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        char *args[] = {
			(char *)interpreter.c_str(),//1
			(char *)path.c_str(), //2
            NULL
        };

        string script_filename = "SCRIPT_FILENAME=" + path;
        string query_string = "QUERY_STRING=" + query;

        char *env[] = {
            (char *)"REQUEST_METHOD=GET",
            (char *)script_filename.c_str(),
            (char *)query_string.c_str(),
            (char *)"SERVER_PROTOCOL=HTTP/1.1",
            (char *)"REMOTE_ADDR=127.0.0.1",
            (char *)"HTTP_HOST=localhost",
            NULL
        };

        
        execve(interpreter.c_str(), args, env);

        perror("execve");
        exit(1);
    }

     close(fd[1]);

    string output;
    char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(fd[0], buffer, sizeof(buffer))) > 0) {
        output.append(buffer, bytesRead);
    }

    close(fd[0]);
    waitpid(pid, NULL, 0);
    return output;
}


// std::string     normalizePath(const std::string& path) {
//     std::string result;
//     bool prevSlash = false;

//     for (size_t i = 0; i < path.length(); ++i) {
//         char c = path[i];
//         if (c == '/') {
//             if (!prevSlash) {
//                 result += c;
//                 prevSlash = true;
//             }
//         } else {
//             result += c;
//             prevSlash = false;
//         }
//     }
//     return result;
// }

string checkIndexes(location* loc, const string path) {
    std::vector<std::string>* indexes = loc->getInfos("index");
    if (!indexes) {
        cout << "No index list found." << endl;
        return "";
    }

    for (size_t i = 0; i < indexes->size(); ++i) {
        ifstream file(path + indexes->at(i).c_str());
        if (file.is_open()) {
            cout << "Found existing index file: " << indexes->at(i) << endl;
            file.close();
            return path + indexes->at(i).c_str();
        }
    }

    cout << "**************No index files found.*************" << endl;
    return "";
}

////////////////////////////////////////////////////////////////////////////////////////

string removeLeadingSlashes(const string& input) {
    size_t firstNonSlash = input.find_first_not_of('/');
    if (firstNonSlash == string::npos)
        return "";
    return input.substr(firstNonSlash);
}

//
string handleGetRequest1(data_request &req, location *loc, const Server &myServer, int currentFd)
{
    // 1 check method
    if (req.method == "GET")
    {
        string locPath = normalizePath(loc->path);
        string reqPath = normalizePath(req.path);
        cout << "localPath : " << locPath << endl;
        cout << "reqPath : " << reqPath << endl;
        if (loc->getInfos("root") == NULL)
            cout << " rehaaab" << endl;

        string rootVar = loc->getInfos("root")->at(0);
        string fullPath = reqPath;
        string fullPathWithR = reqPath;

        cout << "rootVar : " << rootVar << endl;
        cout << "locPath: " << locPath << endl;
        cout << "reqPath: "<< fullPath << endl;
        
        if (locPath == reqPath || locPath + string("/") == fullPath \
            || normalizePath(locPath + string("/") + string(rootVar)) == normalizePath(fullPathWithR + "/"))
        {
            string indexRe = checkIndexes(loc, rootVar + "/");
            if (indexRe == "" && loc->getInfos("autoindex")->at(0) == "on")
            {
                std::string body = listDirectory(removeLocation(reqPath, locPath + "/"), "amine");
                // cout << "body : "<< body << endl;
                std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + std::to_string(body.size()) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" +
                    body;
                send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);
            }
            cout << "indexRe: " << indexRe<< endl;
            fullPath = indexRe;
        }
        else
        {
            std::string relativePath = fullPath.substr(locPath.length());
            
            size_t found = relativePath.find(rootVar);
            if (found == std::string::npos)
                fullPath = rootVar + relativePath;
            else
            {

                cout << "********************PROB HNA************************" << endl;
                cout << "fullPath before: " << fullPath << endl;
                fullPath = removeLeadingSlashes(removeLocation(relativePath, locPath + "/" + rootVar));
                cout << "fullPath: " << fullPath << endl;
                cout << "rootVar: "  << rootVar  << endl;
                cout << "test: " << removeLocation(fullPath, "/html") << endl;
                fullPath = removeLeadingSlashes(removeLocation(fullPath, rootVar));
                fullPath = removeLeadingSlashes(removeLocation(fullPath, "www/"));
                cout << "second fullPath: " << fullPath << endl;
                
                // fullPath = removeLocation(relativePath, locPath + "/" + rootVar);
                
                cout << "****************************************************\n";
            }
        }
        cout << "fullPath__after condition : " << fullPath << endl;
        // fullPath = "www/html/index.html";
        ///////////////////// CGI /////////////////////////
        
        // std::string extension;
                
        // size_t dotPos = fullPath.find_last_of('.');
        // size_t qPos = fullPath.find_last_of('?');

        // if (dotPos != string::npos && qPos != string::npos && dotPos < qPos)
        //     extension = fullPath.substr(dotPos, qPos - dotPos);
        // else if (dotPos != std::string::npos)
        //     extension = fullPath.substr(dotPos);
        // cout << "extension: " << extension << endl;
        
        // loc = getClosestLocation(myServer, "/api");
        // std::vector<std::string>* cgiExt = loc->getInfos("cgi_extension");
        // std::vector<std::string>* cgiPath = loc->getInfos("cgi_path");        
        
        // bool isCgi = false;
        
        // if (cgiExt) {
        //     for (size_t i = 0; i < cgiExt->size(); ++i) {
        //         cout << "result: " << (*cgiExt)[i] << endl;
        //         if (extension == (*cgiExt)[i]) {
        //             isCgi = true;
        //             break;
        //         }
        //     }
        // }
        // //
        // cout << isCgi << endl;

        // if (isCgi && cgiPath && !cgiPath->empty()) {
        //     cout << "in CGI\n" << endl;
        //     std::string interpreter = (*cgiPath)[0];
        //     cout << "interpreter: " << interpreter << endl;

        //     cout << "inQuery\n" << endl;
        //     std::string query;
        //     size_t pos = req.path.find('?');
        //     if (pos != std::string::npos) {
        //         query = req.path.substr(pos + 1);
        //         fullPath = root + req.path.substr(0, pos);
        //     }
        //     cout << "test: "<< fullPath << endl;
        //     string cgiOutput = executeCgi(fullPath, query, interpreter);
        //     std::ostringstream response;

        //     response << "HTTP/1.1 200 OK\r\n";
        //     response << "Content-Type: " << getMimeType(fullPath) << "\r\n";
        //     response << "Content-Length: " << cgiOutput.size() << "\r\n";
        //     response << "Connection: close\r\n\r\n";
        //     response << cgiOutput << endl;
        //     return response.str();
        // }
        ///////////////////////////////////////////
        
        //3 exist file (path)
        if (!existFile(fullPath, loc, reqPath, locPath, currentFd))
        {
            cout << "error dir: "<< fullPath << endl;
            return "HTTP/1.1 405 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        }
        cout << "success dir: "<< fullPath << endl;

        // 4 open file and read content
        std::string body = readFile(fullPath);
        if (body == "error opening !!")
            return "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        
        std::ostringstream ait_response;

        ait_response << "HTTP/1.1 200 OK\r\n";
        ait_response << "Content-Type: " << getMimeType(fullPath) << "\r\n";
        ait_response << "Content-Length: " << body.size() << "\r\n";
        ait_response << "Connection: close\r\n\r\n";
        ait_response << body << endl;
        return ait_response.str();
    }
    return "amine";
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

//////////////////////////////////////HELP CGI///////////////////////////////////////
bool isCgiRequest(location *loc, const std::string &path) {
    std::vector<std::string>* cgi_exts = loc->getInfos("cgi_extension");
    std::cout << "salam" << std::endl;

    if (!cgi_exts) {
        std::cout << "cgi_extension NULL" << std::endl;
        return false;
    }

    for (size_t i = 0; i < cgi_exts->size(); ++i) {
        const std::string &ext = (*cgi_exts)[i];

        if (path.size() >= ext.size() &&
            path.compare(path.size() - ext.size(), ext.size(), ext) == 0) {
            return true;
        }

        std::cout << "check: " << path.compare(path.size() - ext.size(), ext.size(), ext) << std::endl;
    }

    return false;
}

bool executeCgiScript(const data_request &req, const std::string &scriptPath, location *loc, std::string &output) {
    int pipefd[2];
    if (pipe(pipefd) == -1)
        return false;

    pid_t pid = fork();
    if (pid < 0)
        return false;

    if (pid == 0) {
        // Child process
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        std::vector<std::string> envVec;
        envVec.push_back("REQUEST_METHOD=GET");
        envVec.push_back("SCRIPT_FILENAME=" + scriptPath);
        envVec.push_back("REDIRECT_STATUS=200");

        std::vector<char*> envp;
        for (size_t i = 0; i < envVec.size(); ++i) {
            envp.push_back(const_cast<char*>(envVec[i].c_str()));
        }
        envp.push_back(NULL);

        // ✅ Secure access to interpreter
        std::vector<std::string>* cgiPathVec = loc->getInfos("cgi_path");
        if (!cgiPathVec || cgiPathVec->empty()) {
            std::cerr << "cgi_path is missing or empty!" << std::endl;
            exit(1);
        }

        std::string interpreter = (*cgiPathVec)[0];

        char *args[] = {
            const_cast<char*>(interpreter.c_str()),
            const_cast<char*>(scriptPath.c_str()),
            NULL
        };

        execve(interpreter.c_str(), args, &envp[0]);
        perror("execve failed"); // pour debug
        exit(1); // in case execve fails
    } else {
        // Parent process
        close(pipefd[1]);

        char buffer[1024];
        ssize_t n;
        output.clear();
        while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            output.append(buffer, n);
        }

        close(pipefd[0]);
        waitpid(pid, NULL, 0);
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////

string handleGetRequest(data_request &req, location *loc, const Server &myServer, int currentFd)
{
    string locPath = normalizePath(loc->path);
    string reqPath = normalizePath(req.path);
    string rootVar = loc->getInfos("root")->at(0);

    if (loc->getInfos("root") == NULL)
        cout << " rehaaab" << endl;
    
    cout << "localPath : " << locPath << endl;
    cout << "reqPath : " << reqPath << endl;
    cout << "rootVar : " << rootVar << endl;

    string path = switchLocation(locPath, reqPath, rootVar);
    cout << "path: " << path << endl;
    
    DIR* dir = opendir(path.c_str());
    
    string indexFound = checkIndexes(loc, rootVar + "/");
    if (indexFound == "" && loc->getInfos("autoindex")->at(0) == "on" && dir != NULL)
    {
        std::string body = listDirectory(path, reqPath);
        // cout << "body : "<< body << endl;
        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            body;
        send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);
    }

    if (loc->getInfos("autoindex")->at(0) == "off" && indexFound == "")
    {
        std::string body = "<html><body><h1>403 Forbidden</h1></body></html>";
        std::string response =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            body;
            return response;
    }
    
    ///////////////////// CGI /////////////////////////
    if (isCgiRequest(getClosestLocation(myServer, "/api"), path)) {
        std::string cgiOutput;
        if (!executeCgiScript(req, path, getClosestLocation(myServer, "/api"), cgiOutput)) {
            return "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        }
        cout << "in CGI***********************" << endl;
        std::ostringstream cgiResponse;
        cgiResponse << "HTTP/1.1 200 OK\r\n";
        cgiResponse << "Content-Type: text/html\r\n";
        cgiResponse << "Content-Length: " << cgiOutput.size() << "\r\n";
        cgiResponse << "Connection: close\r\n\r\n";
        cgiResponse << cgiOutput;
        return cgiResponse.str();
    }

    //////////////////////////////////////////////////

    if (!existFile(path, loc, reqPath, locPath, currentFd))
    {
        cout << "error dir: "<< path << endl;
        std::string body = "<html><body><h1>404 Not Found</h1></body></html>";
        std::string response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            body;
        return response;
    }
        cout << "success dir: "<< path << endl;
        // 4 open file and read content
        std::string body = readFile(path);
        if (body == "error opening !!")
            return "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
    
        std::ostringstream ait_response;

        ait_response << "HTTP/1.1 200 OK\r\n";
        ait_response << "Content-Type: " << getMimeType(path) << "\r\n";
        ait_response << "Content-Length: " << body.size() << "\r\n";
        ait_response << "Connection: close\r\n\r\n";
        ait_response << body << endl;
        return ait_response.str();
}
