#include "../../_includes/ServerManager.hpp"
#include "GetMethod.hpp"
#include <sys/wait.h>
#include <dirent.h>

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
    // cout << "path in listDirect: " << path << endl;
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
            string checkIndex = checkIndexes(loc, path + "/");
            if (checkIndex == "")
            {
                // cout << "path in existFile: "<< path << endl;
                std::string body = listDirectory(path, reqPath);
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
                path = checkIndex;
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
    if (!file) return "";
    else cout << "opning file !!\n" ;
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
    std::cout << "No matching extension." << std::endl;
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
#include <string>
#include <vector>
#include <iostream>

std::string getExtension(const std::string &path) {
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos)
        return ""; // No extension
    return path.substr(dot);
}

std::string getCgiInterpreter(const std::string &scriptPath, location &loc) {
    std::vector<std::string>* extensions = loc.getInfos("cgi_extension");
    std::vector<std::string>* interpreters = loc.getInfos("cgi_path");

    if (!extensions || !interpreters) {
        std::cerr << "Missing cgi_extension or cgi_path config" << std::endl;
        return "";
    }

    std::string ext = getExtension(scriptPath);
    for (size_t i = 0; i < extensions->size(); ++i) {
        if ((*extensions)[i] == ext)
            return (*interpreters)[i];
    }

    std::cerr << "No CGI interpreter found for extension: " << ext << std::endl;
    return "";
}

// bool executeCgi(const std::string &scriptPath, const data_request &req, location &loc, std::string &output) {
//     cout << "in excutecgi !!" << endl;
//     int pipefd[2];
//     if (pipe(pipefd) == -1)
//         return false;
//     pid_t pid = fork();
//     if (pid < 0)
//         return false;

//     if (pid == 0) {
//         dup2(pipefd[1], STDOUT_FILENO);
//         close(pipefd[0]);

//         std::string cgiPath = getCgiInterpreter(scriptPath, loc);
// 		if (cgiPath.empty())
// 			exit(1);

//         string query = req.queryContent;
//         char *argv[] = {
//             (char*)cgiPath.c_str(),     
//             (char*)scriptPath.c_str(),  
//             NULL
//         };

//         string reqMethod = "REQUEST_METHOD=" + req.method;
//         char *envp[] = {
//             (char*)"GATEWAY_INTERFACE=CGI/1.1",
//             (char*)reqMethod.c_str(),
//             (char*)query.c_str(),
//             NULL
//         };

//         execve(cgiPath.c_str(), argv, envp);
//         exit(1);
//     } else {
//         close(pipefd[1]);
//         char buffer[4096];
//         ssize_t bytesRead;
//         while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0)
//             output.append(buffer, bytesRead);
//         close(pipefd[0]);
//         int status;
//         waitpid(pid, &status, 0);

//         if (status != 0)
//             throw(500);
//         return true;
//     }
// }

bool executeCgi(const std::string &scriptPath, const data_request &req, location &loc, std::string &output) {
    int pipefd[2];
    int inputfd[2];

    if (pipe(pipefd) == -1 || pipe(inputfd) == -1)
        return false;

    pid_t pid = fork();
    if (pid < 0)
        return false;

    string postBody;

    std::map<std::string, std::vector<std::string> >::iterator itBody = loc.infos.find("upload_store");
    if(itBody == loc.infos.end())
        throw(404);

    postBody = loc.getInfos("upload_store")->at(0);

    if (pid == 0) {

        dup2(pipefd[1], STDOUT_FILENO); // stdout -> pipe
        close(pipefd[0]);
        close(pipefd[1]);

        if (req.method == "POST") {
            dup2(inputfd[0], STDIN_FILENO); // stdin <- pipe
        }
        close(inputfd[1]);
        close(inputfd[0]);

        std::string cgiPath = getCgiInterpreter(scriptPath, loc);
        if (cgiPath.empty())
            exit(1);

        char *argv[] = {
            (char *)cgiPath.c_str(),
            (char *)scriptPath.c_str(),
            NULL
        };


        std::string method = "REQUEST_METHOD=" + req.method;
        std::string queryString = "QUERY_STRING=" + req.queryContent;
        // std::string contentLength = "CONTENT_LENGTH=" + std::to_string(req.body.length());
        std::string contentType = "CONTENT_TYPE=application/x-www-form-urlencoded"; // or req.contentType

        std::vector<char *> envp;
        envp.push_back((char *)"GATEWAY_INTERFACE=CGI/1.1");
        envp.push_back((char *)method.c_str());

        if (req.method == "GET")
            envp.push_back((char *)queryString.c_str());
        // else if (req.method == "POST") {
        //     envp.push_back((char *)contentLength.c_str());
        //     envp.push_back((char *)contentType.c_str());
        // }

        envp.push_back(NULL);

        execve(cgiPath.c_str(), argv, envp.data());
        exit(1);
    } else {

        close(pipefd[1]);
        close(inputfd[0]);

        if (req.method == "POST") {
            write(inputfd[1], postBody.c_str(), postBody.length());
        }
        
        close(inputfd[1]);

        char buffer[4096];
        ssize_t bytesRead;
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0)
            output.append(buffer, bytesRead);
        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);
        if (status != 0)
            throw(500);
        return true;
    }
}

std::string buildHttpResponse(int statusCode, const std::string &statusMessage, const std::string &body) {
    std::string response;

    response = "HTTP/1.1 " + std::to_string(statusCode) + " " + statusMessage + "\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;

    return response;
}

/////////////////////////////////////////////////////////////////////////////////////////

string checkIndexes(location* loc, const string path) {
    std::vector<std::string>* indexes = loc->getInfos("index");
    if (!indexes)
        return "";

    for (size_t i = 0; i < indexes->size(); ++i) {
        ifstream file(path + indexes->at(i).c_str());
        if (file.is_open()) {
            file.close();
            return path + indexes->at(i).c_str();
        }
    }
    return "";
}

////////////////////////////////////////////////////////////////////////////////////////

string removeLeadingSlashes(const string& input) {
    size_t firstNonSlash = input.find_first_not_of('/');
    if (firstNonSlash == string::npos)
        return "";
    return input.substr(firstNonSlash);
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

    if (!cgi_exts)
        return false;
    for (size_t i = 0; i < cgi_exts->size(); ++i) {
        const std::string &ext = (*cgi_exts)[i];

        if (path.size() >= ext.size())
            if (path.compare(path.size() - ext.size(), ext.size(), ext) == 0)
                return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////

string handleGetRequest(data_request &req, location *loc, const Server &myServer, int currentFd)
{
    string locPath = normalizePath(loc->path);
    string reqPath = normalizePath(req.path);
    string rootVar;

    std::map<std::string, std::vector<std::string> >::iterator itRoot = loc->infos.find("root");
    if(itRoot == loc->infos.end())
        throw(404);
    rootVar = loc->getInfos("root")->at(0) + "/";
    string path = switchLocation(locPath, reqPath, rootVar);

    
    DIR* dir = opendir(path.c_str());
    string indexFound = checkIndexes(loc, rootVar + "/");
    if (indexFound == "")
    {
        if (loc->getInfos("autoindex"))
        {
            if (loc->getInfos("autoindex")->at(0) == "on" && dir != NULL)
            {
                std::string body = listDirectory(path, reqPath);
                std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + std::to_string(body.size()) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" +
                    body;
                send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);
            }
        }
        else
            throw(403);
    }
    
    //////////////////////////////////////////////////

    if (!existFile(path, loc, reqPath, locPath, currentFd)) 
        throw(404);

        std::string body = readFile(path);
        if (body == "")
            throw(500);
    
    //////////////////////////////////////////
        std::ostringstream ait_response;

        ait_response << "HTTP/1.1 200 OK\r\n";
        ait_response << "Content-Type: " << getMimeType(path) << "\r\n";
        ait_response << "Content-Length: " << body.size() << "\r\n";
        ait_response << "Connection: close\r\n\r\n";
        ait_response << body << endl;
        return ait_response.str();
}
