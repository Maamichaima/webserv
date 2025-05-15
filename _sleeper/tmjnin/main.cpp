#include <iostream>
#include <string>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>
# include <fstream>
#include <sstream>

using namespace std;

struct data_request
{
    std::string method;
    std::string path;
    std::map<std::string, std::string> headrs;
    std::string body;
    int size_body;
} typedef data_request;

void print_data(data_request& req) 
{
    std::cout << "===== HEADERS RECEIVED =====" << std::endl;   
    
    for (std::map<std::string, std::string>::const_iterator it = req.headrs.begin(); it != req.headrs.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
}

/// @brief 
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
    if (path.length() >= 3 && path.substr(path.length() - 3) == ".js") return "application/javascript";
    if (path.length() >= 4 && path.substr(path.length() - 4) == ".png") return "image/png";
    if (path.length() >= 4 && path.substr(path.length() - 4) == ".jpg") return "image/jpeg";
    if (path.length() >= 5 && path.substr(path.length() - 5) == ".mp4") return "video/mp4";
    return "text/plain";
}
//
string handleGetRequest(data_request &req, string root)
{
    // 1 check method
    if (req.method == "GET")
    {
        //2 fullPath
        string fullPath = root + req.path;
        if (!fullPath.empty() && fullPath[fullPath.size() - 1] == '/') // ila t7a9ay y3ni rah folder
            fullPath += "index.html";

        //3 exist file (path)
        if (existFile(fullPath) == 0)
            cout << "file not Exist" << endl;
        else
            cout << "existed file !!" << endl;
        
        // 4 open file and read content
        std::string body = readFile(fullPath);
        
        std::ostringstream ait_responce;

        ait_responce << "HTTP/1.1 200 OK\r\n";
        ait_responce << "Content-Type: " << getMimeType(fullPath) << "\r\n";
        ait_responce << "Content-Length: " << body.size() << "\r\n";
        ait_responce << "Connection: close\r\n\r\n";
        ait_responce << body;
        return ait_responce.str();
    }
    return "amine";
}

int main() {
        data_request req;
        req.method = "GET";
        req.path = "/index.html";
        req.headrs["Host"] = "localhost:8080";
        req.headrs["User-Agent"] = "TestClient/1.0";
        req.body = "";
        req.size_body = 0;
    
        std::string root = "./www";
        std::string response = handleGetRequest(req, root);
        cout << response << endl; 
    print_data(req);

}