#include <map>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

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
        
        cout << "fullPath" << fullPath << endl;

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

// int main() {
//     int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd == 0) {
//         perror("socket failed");
//         return 1;
//     }

//     sockaddr_in address = {};
//     int opt = 1;
//     setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(8080);

//     bind(server_fd, (struct sockaddr *)&address, sizeof(address));
//     listen(server_fd, 3);
//     std::cout << "Server listening on port 8080...\n";

//     int addrlen = sizeof(address);
//     int client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
//     if (client_socket < 0) {
//         perror("accept");
//         return 1;
//     }

//     char buffer[3000] = {0};
//     read(client_socket, buffer, 3000);
//     std::cout << "Received:\n" << buffer << std::endl;

//     std::string response = handleGetRequest(req, root);
//     send(client_socket, response.c_str(), response.size(), 0);

//     close(client_socket);
//     close(server_fd);
//     return 0;

// }

int main() {

    data_request req;
    req.method = "GET";
    req.path = "/index.htmlll";
    req.headrs["Host"] = "localhost:8080";
    req.headrs["User-Agent"] = "TestClient/1.0";
    req.body = "";
    req.size_body = 0;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    while (true) {
        int new_socket = accept(server_fd, nullptr, nullptr);
        if (new_socket < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        char buffer[1024] = {0};
        read(new_socket, buffer, 1024);

        std::string request(buffer);
        std::cout << "Request received:\n" << request << std::endl;

        std::string response = handleGetRequest(req, "./www");

        send(new_socket, response.c_str(), response.length(), 0);
        std::cout << "Response sent\n";

        close(new_socket);
    }

    close(server_fd);
    return 0;
}