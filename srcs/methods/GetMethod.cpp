#include "../../_includes/executeCgi.hpp"

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

std::map<std::string, std::string> createExtensionToMimeMap() {
    std::map<std::string, std::string> mimeMap;

    // Text
    mimeMap[".html"] = "text/html";
    mimeMap[".css"] = "text/css";
    mimeMap[".xml"] = "text/xml";
    mimeMap[".txt"] = "text/plain";
    mimeMap[".mml"] = "text/mathml";
    mimeMap[".jad"] = "text/vnd.sun.j2me.app-descriptor";
    mimeMap[".wml"] = "text/vnd.wap.wml";
    mimeMap[".htc"] = "text/x-component";

    // Images
    mimeMap[".gif"] = "image/gif";
    mimeMap[".jpg"] = "image/jpeg";
    mimeMap[".png"] = "image/png";
    mimeMap[".svg"] = "image/svg+xml";
    mimeMap[".tiff"] = "image/tiff";
    mimeMap[".wbmp"] = "image/vnd.wap.wbmp";
    mimeMap[".webp"] = "image/webp";
    mimeMap[".ico"] = "image/x-icon";
    mimeMap[".jng"] = "image/x-jng";
    mimeMap[".bmp"] = "image/x-ms-bmp";
    mimeMap[".avif"] = "image/avif";

    // Fonts
    mimeMap[".woff"] = "font/woff";
    mimeMap[".woff2"] = "font/woff2";

    // Application
    mimeMap[".js"] = "application/javascript";
    mimeMap[".json"] = "application/json";
    mimeMap[".jar"] = "application/java-archive";
    mimeMap[".hqx"] = "application/mac-binhex40";
    mimeMap[".doc"] = "application/msword";
    mimeMap[".pdf"] = "application/pdf";
    mimeMap[".ps"] = "application/postscript";
    mimeMap[".rtf"] = "application/rtf";
    mimeMap[".m3u8"] = "application/vnd.apple.mpegurl";
    mimeMap[".kml"] = "application/vnd.google-earth.kml+xml";
    mimeMap[".kmz"] = "application/vnd.google-earth.kmz";
    mimeMap[".xls"] = "application/vnd.ms-excel";
    mimeMap[".eot"] = "application/vnd.ms-fontobject";
    mimeMap[".ppt"] = "application/vnd.ms-powerpoint";
    mimeMap[".odg"] = "application/vnd.oasis.opendocument.graphics";
    mimeMap[".odp"] = "application/vnd.oasis.opendocument.presentation";
    mimeMap[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
    mimeMap[".odt"] = "application/vnd.oasis.opendocument.text";
    mimeMap[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    mimeMap[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    mimeMap[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    mimeMap[".wmlc"] = "application/vnd.wap.wmlc";
    mimeMap[".wasm"] = "application/wasm";
    mimeMap[".7z"] = "application/x-7z-compressed";
    mimeMap[".cco"] = "application/x-cocoa";
    mimeMap[".jardiff"] = "application/x-java-archive-diff";
    mimeMap[".jnlp"] = "application/x-java-jnlp-file";
    mimeMap[".run"] = "application/x-makeself";
    mimeMap[".pl"] = "application/x-perl";
    mimeMap[".prc"] = "application/x-pilot";
    mimeMap[".rar"] = "application/x-rar-compressed";
    mimeMap[".rpm"] = "application/x-redhat-package-manager";
    mimeMap[".sea"] = "application/x-sea";
    mimeMap[".swf"] = "application/x-shockwave-flash";
    mimeMap[".tcl"] = "application/x-tcl";
    mimeMap[".der"] = "application/x-x509-ca-cert";
    mimeMap[".xpi"] = "application/x-xpinstall";
    mimeMap[".xhtml"] = "application/xhtml+xml";
    mimeMap[".xspf"] = "application/xspf+xml";
    mimeMap[".zip"] = "application/zip";
    mimeMap[".urlencoded"] = "application/x-www-form-urlencoded";
    mimeMap[".bin"] = "application/octet-stream";

    // Audio
    mimeMap[".mid"] = "audio/midi";
    mimeMap[".mp3"] = "audio/mpeg";
    mimeMap[".ogg"] = "audio/ogg";
    mimeMap[".m4a"] = "audio/x-m4a";
    mimeMap[".ra"] = "audio/x-realaudio";

    // Video
    mimeMap[".3gp"] = "video/3gpp";
    mimeMap[".ts"] = "video/mp2t";
    mimeMap[".mp4"] = "video/mp4";
    mimeMap[".mpeg"] = "video/mpeg";
    mimeMap[".mov"] = "video/quicktime";
    mimeMap[".webm"] = "video/webm";
    mimeMap[".flv"] = "video/x-flv";
    mimeMap[".m4v"] = "video/x-m4v";
    mimeMap[".mng"] = "video/x-mng";
    mimeMap[".asf"] = "video/x-ms-asf";
    mimeMap[".wmv"] = "video/x-ms-wmv";
    mimeMap[".avi"] = "video/x-msvideo";

    return mimeMap;
}

std::string getMimeType(const std::string &extension)
{
    std::map<std::string, std::string> mimeMap = createExtensionToMimeMap();
    std::map<std::string, std::string>::iterator it = mimeMap.find(extension);
    if(it != mimeMap.end())
    {
        return mimeMap[extension];
    }
    return "text/plain";
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

string checkIndexes(location* loc, const string path) {
    std::vector<std::string>* indexes = loc->getInfos("index");
    if (!indexes)
        return "";

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
    
    if (fileSize > CHUNKED_THRESHOLD && clientObj != NULL) {
        clientObj->fileStream->open(path.c_str(), std::ios::in | std::ios::binary);
        if (!clientObj->fileStream->is_open())
            throw(500);
        
        clientObj->bytesRemaining = fileSize;
        clientObj->fileSize = fileSize;
        clientObj->fileToSend = path;
        clientObj->closeConnection = false;
        
        std::string mimeType = getMimeType(getExtensions(path));
        
        std::ostringstream headers;
        headers << "HTTP/1.1 200 OK\r\n";
        headers << "Content-Type: " << mimeType << "\r\n";
        headers << "Content-Length: " << fileSize << "\r\n";
        headers << "Accept-Ranges: bytes\r\n";
        headers << "Connection: close\r\n\r\n";
        
        return headers.str();
    }
    
    std::string body = readFile(path);
    if (body == "")
        throw(500);
            
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: " << getMimeType(getExtensions(path)) << "\r\n";
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
        
        std::string mimeType = getMimeType(getExtensions(path));
        
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
