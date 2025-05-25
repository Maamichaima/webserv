#include "../../_includes/ServerManager.hpp"
#include "GetMethod.hpp"

// khass rihab txouf fin dirha
using std::cout;

location *getClosestLocation(const Server &server,
                             const std::string &requestPath) {
  location *bestMatch = nullptr;
  size_t bestLength = 0;

//   cout << "PATH: " << requestPath << std::endl;
  for (std::map<std::string, location>::const_iterator it =
           server.locations.begin();
       it != server.locations.end(); ++it) {
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
//
string handleGetRequest(data_request &req, location *loc, const std::vector<Server> &servers)

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
        
        loc = getClosestLocation(servers[0], "/api");
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

            cout << "inQuery\n" << endl;
            std::string query;
            size_t pos = req.path.find('?');
            if (pos != std::string::npos) {
                query = req.path.substr(pos + 1);
                fullPath = root + req.path.substr(0, pos);
            }
        // Extraire query string
        /*execute cgi soon*/
    // and return cgi_responce
    }
        ///////////////////////////////////////////

        //3 exist file (path)
        if (!existFile(fullPath))
            return "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        
        // 4 open file and read content
        std::string body = readFile(fullPath);
        if (body == "error opening !!")
            return "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        
        std::ostringstream ait_responce;

        ait_responce << "HTTP/1.1 200 OK\r\n";
        ait_responce << "Content-Type: " << getMimeType(fullPath) << "\r\n";
        ait_responce << "Content-Length: " << body.size() << "\r\n";
        ait_responce << "Connection: close\r\n\r\n";
        ait_responce << body << endl;
        return ait_responce.str();
    }
    return "amine";
}