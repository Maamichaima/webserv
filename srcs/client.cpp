#include "../_includes/client.hpp"
#include "methods/GetMethod.hpp"
#include "methods/DeleteMethod.hpp"

int readFileContent(const std::string& filePath, std::string &content) {
	std::ifstream file(filePath.c_str());
	if (!file.is_open())
		return 0;
		// throw std::runtime_error("this file not open" + filePath);

	std::string line;
	while (std::getline(file, line)) {
		content += line + "\n";
	}
	return 1;
}

client::client() 
{
	this->flag = 0;
	this->data_rq.size_body = 0;
	this->data_rq.size_chunked = -1;
	this->data_rq.flag_chunked = 0;
	this->data_rq.is_chunked = 0;
	this->data_rq.flag_error = 0;
	this->closeConnection = false;
	this->data_rs.flaIsRedirect = 0;
	this->data_rs.status_code = -1;
	this->data_rq.isCgi = 0;

	setErrorPages();
	setDescription();
}

client::client(std::string buff, int fd) : parc(parser())
{
	this->buffer = buff;
	this->server_fd = fd;
	this->flag = 0;
	this->data_rq.size_body = 0;
	this->closeConnection = false;
	this->data_rq.size_chunked = -1;
}

client &client::operator=(const client &obj)
{
	if (this != &obj)
	{
		this->buffer = obj.buffer;
		this->server_fd = obj.server_fd;
	}
	return (*this);
}

client::~client()
{
	// close (server_fd);
}

void client::parceBody()
{
	std::map<std::string,
		std::string>::iterator it = this->data_rq.headers.find("transfer-encoding");
}

void client::printClient()
{
	std::cout << "Method --> " << this->data_rq.method << std::endl;
	std::cout << "Path --> " << this->data_rq.path << std::endl;
	std::cout << "version --> " << this->data_rq.version << std::endl;
	std::cout << "query Content --> " << this->data_rq.queryContent << std::endl;
	std::map<std::string, std::string>::iterator it;
	for (it = this->data_rq.headers.begin(); it != this->data_rq.headers.end(); it++)
	{
		std::cout << "key --> " << it->first << " value --> " << it->second << std::endl;
	}
	std::cout << "our body in file ourBody.txt\n";
}

void client::setBuffer(std::string str, ssize_t bytesRead)
{
	this->buffer.append(str, bytesRead);
}

int isError(int numStatusCode)
{
	if(numStatusCode == 400 || numStatusCode == 404 || numStatusCode == 411 ||
		 numStatusCode == 500 || numStatusCode == 501 || numStatusCode == 505 || 
		 numStatusCode == 405 || numStatusCode == 409 || numStatusCode == 403 ||
		 numStatusCode == 413 || numStatusCode == 301 || numStatusCode == 308 || 
		 numStatusCode == 307 || numStatusCode == 302 || numStatusCode == 303 || numStatusCode == 304)
		return 1;
	return 0;
}
int client::checkRequestProgress() 
{
	if(this->data_rs.status_code > 0)// ila kan -1 donc rah koulchi ghadi mzyan hit 1 t9der tkon f redirect 
		return 1;
	std::map<std::string, std::string>::iterator it = this->data_rq.headers.find("transfer-encoding");
	std::map<std::string, std::string>::iterator it1 = this->data_rq.headers.find("content-length");
    
    if(this->flag == 3)
        return 1;
    else if(this->flag == 2 && (it == this->data_rq.headers.end() && it1 == this->data_rq.headers.end()))
        return 1;
    return 0;
}

void client::parseRequest()
{
	try
	{
		this->data_rs.status_code = parc.parse(*this);
		if(checkRequestProgress()) {}
			// this->printClient();
	}
	catch(const int status_code)
	{
		this->data_rs.status_code = status_code;
		// std::cout << "our status code === " << this->data_rs.status_code << " ====== \n";
		// exit(0);
	}
}

std::string headersToOneString(std::map<std::string, std::string> headers)
{
	std::string head;

	std::map<std::string, std::string>::iterator it = headers.begin();
	while(it != headers.end())
	{
		head += it->first + ": " + it->second + "\r\n";
		it++;
	}
	head += "\r\n";
	return head;
}

std::string client::buildResponse()
{
	std::string response = this->data_rs.startLine + headersToOneString(this->data_rs.headers) + this->data_rs.body; 
	return response;
}

template <typename T>
std::string to_string(T value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

std::string padLeftToThree(const std::string& input) {
    if (input.length() >= 3)
        return input;
    std::string padded = input;
    while (padded.length() < 3) {
        padded = "0" + padded;
    }
    return padded;
}


int isRedirect(int red)
{
	if(red == 301 || red == 302 || red == 304 || red == 307 || red == 308)
		return 1;
	return 0;
}

void client::setDataResponse()
{
	this->data_rs.startLine = "HTTP/1.1 " + padLeftToThree(to_string(this->data_rs.status_code)) + " " + client::description[this->data_rs.status_code] + "\r\n";
	
	if(this->data_rs.flaIsRedirect)
	{
		if(isRedirect(this->data_rs.status_code))//without body
		{
			//location deja 3mraat fach kantrowi num of redirect
		}
		else// body with location
		{
			this->data_rs.body = this->data_rq.myCloseLocation->infos["redirect"][1];
			this->data_rs.headers["Content-Type"] = "text/txt;";
			this->data_rs.headers["Content-Length"] = to_string(this->data_rs.body.size());
		}
		return;
	}
	this->data_rs.headers["Content-Type"] = "text/html; charset=UTF-8";
	this->data_rs.headers["Content-Length"] = to_string(client::errorPages[this->data_rs.status_code].size());
	this->data_rs.body = client::errorPages[this->data_rs.status_code];
}

void client::setStatusCode()
{
	if(this->data_rs.status_code < 0 )//&& this->data_rs.flaIsRedirect != 1)//!isError(this->data_rs.status_code))
	{
		if(this->data_rq.method == "POST")	
			this->data_rs.status_code = 201;
		if(this->data_rq.method == "DELETE")	
			this->data_rs.status_code = 204;
	}
}

void client::handleResponse(int currentFd)
{
	setStatusCode();
	std::map<std::string, std::string>::iterator it = this->myServer.errorPages.find(to_string(this->data_rs.status_code));
	if(it != this->myServer.errorPages.end())
	{
		std::string content;
		if(readFileContent(it->second, content))
		{
			this->data_rs.startLine = "HTTP/1.1 " + to_string(this->data_rs.status_code) + " " + client::description[this->data_rs.status_code] + "\r\n";
			this->data_rs.headers["Content-Type"] = "text/html; charset=UTF-8";// "charset=UTF-8" « é » affiché comme � ou un caractère bizarre.
			this->data_rs.headers["Content-Length"] = to_string(content.size());
			this->data_rs.body = content;
			std::string response = buildResponse();
			send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);
			return ;
		}
	}
	if(this->data_rs.status_code < 0 || this->data_rq.isCgi)
	{
		try
		{
			location *cgiLoc = getClosestLocation(this->myServer, data_rq.path);
			//////////////ReSend if not "/" in the end//////////////// mzl fiha moxkil
			if (data_rq.path.back() != '/' && cgiLoc)
			{
				string locPath = normalizePath(cgiLoc->path);
				string reqPath = normalizePath(data_rq.path);
				string root;
				std::map<std::string, std::vector<std::string> >::iterator itRoot = cgiLoc->infos.find("root");
				if(itRoot == cgiLoc->infos.end()) 
					throw(404);
	
				root  = cgiLoc->getInfos("root")->at(0) + "/";          
				string resendPath = switchLocation(locPath, reqPath, root);
		
				// cout << "***********************\n";
				// cout << "reqPath: " << resendPath << endl;
				// cout << isDirectory(resendPath) << endl;
				// cout << "***********************\n";
	
				if (isDirectory(resendPath) && checkIndexes(cgiLoc, resendPath + "/") != "" && 
						!resendPath.empty()) {
				
				std::string newLocation = data_rq.path + "/";
				std::string response = 
					"HTTP/1.1 301 Moved Permanently\r\n"
					"Location: " + newLocation + "\r\n"
					"Content-Length: 0\r\n"
					"Connection: close\r\n"
					"\r\n";
	
				send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);
				return;
				}
	
			}
	
			//////////////////////////////////////////////////////////

			/////////////////// CGI /////////////////////////
			if (this->data_rq.method != "DELETE" && cgiLoc)
			{
				if (isCgiConfigured(cgiLoc))
				{
					string locPath = normalizePath(cgiLoc->path);
					string reqPath = normalizePath(data_rq.path);
					string root;
					vector<string>* exts;
	
					std::map<std::string, std::vector<std::string> >::iterator itRoot = cgiLoc->infos.find("root");
					if(itRoot == cgiLoc->infos.end()) 
						throw(404);
					root  = cgiLoc->getInfos("root")->at(0) + "/";    
					
					string cgiPath = switchLocation(locPath, reqPath, root);
					std::map<std::string, std::vector<std::string> >::iterator itCgi = cgiLoc->infos.find("cgi_extension");
					if(itCgi == cgiLoc->infos.end()) 
						throw(404);
					exts = cgiLoc->getInfos("cgi_extension");
					
					if (isDirectory(cgiPath) && checkIndexes(cgiLoc, cgiPath + "/") != "")
						cgiPath = checkIndexes(cgiLoc, cgiPath+ "/");
					if (checkExtension(cgiPath, *exts))
					{
						string cgiOutput;
						if (executeCgi(cgiPath, data_rq, *cgiLoc, cgiOutput)) {
							send(currentFd, buildHttpResponse(200, "OK", cgiOutput).c_str(), buildHttpResponse(200, "OK", cgiOutput).size(), MSG_NOSIGNAL); 
							return;
						}
						throw(500);
					}
				}
			}
			/////////////////////////GET///////////////////////
			if(this->data_rq.method == "GET" && this->data_rs.status_code < 0)//!isError(this->data_rs.status_code))
			{
				std::string response;
				location* loc = getClosestLocation(this->myServer, data_rq.path);
				if (loc)
					response = handleGetRequest(this->data_rq, loc, this->myServer, currentFd);
				else
				{
					throw(404);
				}
				send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);
				return ;
			}
		}
		catch(const int &statusCode)
		{
			this->data_rs.status_code = statusCode;
		}

	}
    setDataResponse();
    std::string response = buildResponse();
	// std::cout << "***********" << response << "***************" << "\n";
    send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);
}

void client::check_http_body_rules()
{
	if(this->data_rq.method != "GET" && this->data_rq.method != "POST" && this->data_rq.method != "DELETE")
		throw(501);// Not Implemented
	std::map<std::string, std::string>::iterator it_cLenght = this->data_rq.headers.find("content-length");
	std::map<std::string, std::string>::iterator it_cType = this->data_rq.headers.find("content-type");
	std::map<std::string, std::string>::iterator it_cEncoding = this->data_rq.headers.find("transfer-encoding");
	std::map<std::string, std::string>::iterator is_Host = this->data_rq.headers.find("host");
	std::map<std::string, std::string>::iterator isChunked = this->data_rq.headers.find("transfer-encoding");
	if(isChunked != this->data_rq.headers.end() && isChunked->second == "chunked")
		this->data_rq.is_chunked = 1;
	if(it_cLenght != this->data_rq.headers.end())
	{
		if(!isMatch("\\d+", it_cLenght->second))
			throw(400);
		this->data_rq.size_body = atoi(this->data_rq.headers["content-length"].c_str());
	}
	if(is_Host == this->data_rq.headers.end())
		throw(400);
	if(this->data_rq.method == "POST")
	{
		if(it_cEncoding == this->data_rq.headers.end() && it_cLenght == this->data_rq.headers.end())
			throw(411);
		if(it_cType == this->data_rq.headers.end() && this->data_rq.size_body > 0)
			throw(400);
	}
}
