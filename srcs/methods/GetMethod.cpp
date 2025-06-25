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
    cout << "path in listDirect: " << path << endl;
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
        cout << "path: " << path << endl;
        if (isDirectory(path))
        {
            string checkIndex = checkIndexes(loc, path + "/");
            cout << "checkIndex: " << checkIndex << endl;
            if (checkIndex == "")
            {
                cout << "path in existFile: "<< path << endl;
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

bool executeCgi(const std::string &scriptPath, const data_request &req, location &loc, std::string &output) {
    int pipefd[2];
    if (pipe(pipefd) == -1)
        return false;
    pid_t pid = fork();
    if (pid < 0)
        return false;

    if (pid == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);

        std::string cgiPath = getCgiInterpreter(scriptPath, loc);
		if (cgiPath.empty())
			exit(1);

        string query = req.queryContent;
        char *argv[] = {
            (char*)cgiPath.c_str(),     
            (char*)scriptPath.c_str(),  
            NULL
        };

        string reqMethod = "REQUEST_METHOD=" + req.method;
        char *envp[] = {
            (char*)"GATEWAY_INTERFACE=CGI/1.1",
            (char*)reqMethod.c_str(),
            (char*)query.c_str(),
            NULL
        };

        execve(cgiPath.c_str(), argv, envp);
        perror("execve failed");
        exit(1);
    } else {
        close(pipefd[1]);
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

//
// string handleGetRequest1(data_request &req, location *loc, const Server &myServer, int currentFd)
// {
//     // 1 check method
//     if (req.method == "GET")
//     {
//         string locPath = normalizePath(loc->path);
//         string reqPath = normalizePath(req.path);
//         cout << "localPath : " << locPath << endl;
//         cout << "reqPath : " << reqPath << endl;
//         if (loc->getInfos("root") == NULL)
//             cout << " rehaaab" << endl;

//         string rootVar = loc->getInfos("root")->at(0);
//         string fullPath = reqPath;
//         string fullPathWithR = reqPath;

//         cout << "rootVar : " << rootVar << endl;
//         cout << "locPath: " << locPath << endl;
//         cout << "reqPath: "<< fullPath << endl;
        
//         if (locPath == reqPath || locPath + string("/") == fullPath \
//             || normalizePath(locPath + string("/") + string(rootVar)) == normalizePath(fullPathWithR + "/"))
//         {
//             string indexRe = checkIndexes(loc, rootVar + "/");
//             if (indexRe == "" && loc->getInfos("autoindex")->at(0) == "on")
//             {
//                 std::string body = listDirectory(removeLocation(reqPath, locPath + "/"), "amine");
//                 // cout << "body : "<< body << endl;
//                 std::string response =
//                     "HTTP/1.1 200 OK\r\n"
//                     "Content-Type: text/html\r\n"
//                     "Content-Length: " + std::to_string(body.size()) + "\r\n"
//                     "Connection: close\r\n"
//                     "\r\n" +
//                     body;
//                 send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);
//             }
//             cout << "indexRe: " << indexRe<< endl;
//             fullPath = indexRe;
//         }
//         else
//         {
//             std::string relativePath = fullPath.substr(locPath.length());
            
//             size_t found = relativePath.find(rootVar);
//             if (found == std::string::npos)
//                 fullPath = rootVar + relativePath;
//             else
//             {

//                 cout << "********************PROB HNA************************" << endl;
//                 cout << "fullPath before: " << fullPath << endl;
//                 fullPath = removeLeadingSlashes(removeLocation(relativePath, locPath + "/" + rootVar));
//                 cout << "fullPath: " << fullPath << endl;
//                 cout << "rootVar: "  << rootVar  << endl;
//                 cout << "test: " << removeLocation(fullPath, "/html") << endl;
//                 fullPath = removeLeadingSlashes(removeLocation(fullPath, rootVar));
//                 fullPath = removeLeadingSlashes(removeLocation(fullPath, "www/"));
//                 cout << "second fullPath: " << fullPath << endl;
                
//                 // fullPath = removeLocation(relativePath, locPath + "/" + rootVar);
                
//                 cout << "****************************************************\n";
//             }
//         }
//         cout << "fullPath__after condition : " << fullPath << endl;
//         // fullPath = "www/html/index.html";
//         ///////////////////// CGI /////////////////////////
        
//         // std::string extension;
                
//         // size_t dotPos = fullPath.find_last_of('.');
//         // size_t qPos = fullPath.find_last_of('?');

//         // if (dotPos != string::npos && qPos != string::npos && dotPos < qPos)
//         //     extension = fullPath.substr(dotPos, qPos - dotPos);
//         // else if (dotPos != std::string::npos)
//         //     extension = fullPath.substr(dotPos);
//         // cout << "extension: " << extension << endl;
        
//         // loc = getClosestLocation(myServer, "/api");
//         // std::vector<std::string>* cgiExt = loc->getInfos("cgi_extension");
//         // std::vector<std::string>* cgiPath = loc->getInfos("cgi_path");        
        
//         // bool isCgi = false;
        
//         // if (cgiExt) {
//         //     for (size_t i = 0; i < cgiExt->size(); ++i) {
//         //         cout << "result: " << (*cgiExt)[i] << endl;
//         //         if (extension == (*cgiExt)[i]) {
//         //             isCgi = true;
//         //             break;
//         //         }
//         //     }
//         // }
//         // //
//         // cout << isCgi << endl;

//         // if (isCgi && cgiPath && !cgiPath->empty()) {
//         //     cout << "in CGI\n" << endl;
//         //     std::string interpreter = (*cgiPath)[0];
//         //     cout << "interpreter: " << interpreter << endl;

//         //     cout << "inQuery\n" << endl;
//         //     std::string query;
//         //     size_t pos = req.path.find('?');
//         //     if (pos != std::string::npos) {
//         //         query = req.path.substr(pos + 1);
//         //         fullPath = root + req.path.substr(0, pos);
//         //     }
//         //     cout << "test: "<< fullPath << endl;
//         //     string cgiOutput = executeCgi(fullPath, query, interpreter);
//         //     std::ostringstream response;

//         //     response << "HTTP/1.1 200 OK\r\n";
//         //     response << "Content-Type: " << getMimeType(fullPath) << "\r\n";
//         //     response << "Content-Length: " << cgiOutput.size() << "\r\n";
//         //     response << "Connection: close\r\n\r\n";
//         //     response << cgiOutput << endl;
//         //     return response.str();
//         // }
//         ///////////////////////////////////////////
        
//         //3 exist file (path)
//         if (!existFile(fullPath, loc, reqPath, locPath, currentFd))
//         {
//             cout << "error dir: "<< fullPath << endl;
//             return "HTTP/1.1 405 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
//         }
//         cout << "success dir: "<< fullPath << endl;

//         // 4 open file and read content
//         std::string body = readFile(fullPath);
//         if (body == "error opening !!")
//             return "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        
//         std::ostringstream ait_response;

//         ait_response << "HTTP/1.1 200 OK\r\n";
//         ait_response << "Content-Type: " << getMimeType(fullPath) << "\r\n";
//         ait_response << "Content-Length: " << body.size() << "\r\n";
//         ait_response << "Connection: close\r\n\r\n";
//         ait_response << body << endl;
//         return ait_response.str();
//     }
//     return "amine";
// }

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
    rootVar = loc->getInfos("root")->at(0);
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
            else
			{
				throw(403);
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
    
        std::ostringstream ait_response;

        ait_response << "HTTP/1.1 200 OK\r\n";
        ait_response << "Content-Type: " << getMimeType(path) << "\r\n";
        ait_response << "Content-Length: " << body.size() << "\r\n";
        ait_response << "Connection: close\r\n\r\n";
        ait_response << body << endl;
        return ait_response.str();
}
