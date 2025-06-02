#include "../../_includes/ServerManager.hpp"
#include "GetMethod.hpp"
#include <sys/wait.h>
// khass rihab txouf fin dirha
using std::cout;

location *getClosestLocation(const Server &server, const std::string &requestPath) {
  location *bestMatch = nullptr;
  size_t bestLength = 0;

//   cout << "PATH: " << requestPath << std::endl;
  for (std::map<std::string, location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it)
  {
    const std::string &locPath = it->first;
    if (requestPath.find(locPath) == 0 &&
        (requestPath.size() == locPath.size() ||
         requestPath[locPath.size()] == '/' || locPath == "/")) {

      if (locPath.size() > bestLength) {
        bestLength = locPath.size();
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

/// @brief existFiles function check fullPath is file and path is correct
/// @param S_ISREG is a macro for check file is regular or not (wax file ou la directory ou xi 3jab akhur) n9adro ndirouha ta b access !
/// @param stat functin kay jib linformations 3la file (kay9lb 3lih ou sf)
/// @return 0 for success -1 for error
bool existFile(const string &fullPath)
{
    struct stat st;
    return(stat(fullPath.c_str(), &st) == 0 && S_ISREG(st.st_mode));
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

//
string handleGetRequest(data_request &req, location *loc, const Server &myServer)

{
    // cout << "locationFile : " << locationFile << endl << endl;
    // 1 check method
    if (req.method == "GET")
    {
        string root = loc->getInfos("root")->at(0);
        //2 fullPath
        string fullPath = loc->getInfos("root")->at(0) + req.path;
        if (!fullPath.empty() && fullPath[fullPath.size() - 1] == '/') // ila t7a9ay y3ni rah folder
            fullPath += "index.html";
        
        cout << "fullPath__ : " << fullPath << endl;

        ///////////////////// CGI/////////////////////////
        
        std::string extension;
                
        size_t dotPos = fullPath.find_last_of('.');
        size_t qPos = fullPath.find_last_of('?');

        if (dotPos != string::npos && qPos != string::npos && dotPos < qPos)
            extension = fullPath.substr(dotPos, qPos - dotPos);
        else if (dotPos != std::string::npos)
            extension = fullPath.substr(dotPos);
        cout << "extension: " << extension << endl;
        
        loc = getClosestLocation(myServer, "/api");
        std::vector<std::string>* cgiExt = loc->getInfos("cgi_extension");
        std::vector<std::string>* cgiPath = loc->getInfos("cgi_path");        
        
        bool isCgi = false;
        
        if (cgiExt) {
            for (size_t i = 0; i < cgiExt->size(); ++i) {
                cout << "result: " << (*cgiExt)[i] << endl;
                if (extension == (*cgiExt)[i]) {
                    isCgi = true;
                    break;
                }
            }
        }
        //
        cout << isCgi << endl;

        if (isCgi && cgiPath && !cgiPath->empty()) {
            cout << "in CGI\n" << endl;
            std::string interpreter = (*cgiPath)[0];
            cout << "interpreter: " << interpreter << endl;

            cout << "inQuery\n" << endl;
            std::string query;
            size_t pos = req.path.find('?');
            if (pos != std::string::npos) {
                query = req.path.substr(pos + 1);
                fullPath = root + req.path.substr(0, pos);
            }
            cout << "************test: "<< fullPath << endl;
            string cgiOutput = executeCgi(fullPath, query, interpreter);
            std::ostringstream response;

            response << "HTTP/1.1 200 OK\r\n";
            response << "Content-Type: " << getMimeType(fullPath) << "\r\n";
            response << "Content-Length: " << cgiOutput.size() << "\r\n";
            response << "Connection: close\r\n\r\n";
            response << cgiOutput << endl;
            return response.str();
        }
        ///////////////////////////////////////////
        
        //3 exist file (path)
        if (!existFile(fullPath))
            return "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        
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