#include "../_includes/client.hpp"
# include "../_includes/executeCgi.hpp"
#include "methods/DeleteMethod.hpp"


client::client() : bytesRemaining(0), headersSent(false), fileSize(0), fileStream(new std::ifstream())
{
	this->flagProgress 			= 0;
	this->data_rq.size_body 	= 0;
	this->data_rq.size_chunked 	= -1;
	this->data_rq.flag_chunked 	= 0;
	this->data_rq.is_chunked 	= 0;
	this->data_rq.flag_error 	= 0;
	this->closeConnection 		= false;
	this->data_rs.flaIsRedirect = 0;
	this->data_rs.status_code 	= -1;
	this->data_rq.isCgi			= 0;
	lastActivityTime = time(NULL);
	cgi_pid = -1;
	cgi_fd = -1;
	cgi_running = false;
	cgi_start_time = 0;
	cgi_epoll_added = false; // <--- Add this line

    setErrorPages();
    setDescription();
}

client::client(std::string buff, int fd) : parc(parser())
{
	this->buffer 				= buff;
	this->server_fd 			= fd;
	this->flagProgress			= 0;
	this->data_rq.size_body 	= 0;
	this->closeConnection 		= false;
	this->data_rq.size_chunked 	= -1;
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
	if (fileStream) {
		if (fileStream->is_open()) {
			fileStream->close();
		}
		//delete fileStream;
		fileStream = NULL;
	}
	// close (server_fd);
	if (cgi_fd != -1) {
        close(cgi_fd);
        cgi_fd = -1;
    }
    if (cgi_running && cgi_pid > 0) {
        kill(cgi_pid, SIGKILL);
        waitpid(cgi_pid, NULL, 0);
        cgi_pid = -1;
    }
    cgi_epoll_added = false; // <--- Add this line
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
}

void client::setBuffer(std::string str, ssize_t bytesRead)
{
	this->buffer.append(str, bytesRead);
}

int client::checkRequestProgress() 
{
	if(this->data_rs.status_code > 0)// ila kan -1 donc rah koulchi ghadi mzyan hit 1 t9der tkon f redirect 
		return 1;
	std::map<std::string, std::string>::iterator it = this->data_rq.headers.find("transfer-encoding");
	std::map<std::string, std::string>::iterator it1 = this->data_rq.headers.find("content-length");
    
    if(this->flagProgress == 3)
        return 1;
    else if(this->flagProgress == 2 && (it == this->data_rq.headers.end() && it1 == this->data_rq.headers.end()))
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
	}
}


std::string client::buildResponse()
{
	std::string response = this->data_rs.startLine + headersToOneString(this->data_rs.headers) + this->data_rs.body; 
	return response;
}

void client::setDataResponse()
{
	this->data_rs.startLine = "HTTP/1.1 " + padLeftToThree(to_string_98(this->data_rs.status_code)) + " " + client::description[this->data_rs.status_code] + "\r\n";
	
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
			this->data_rs.headers["Content-Length"] = to_string_98(this->data_rs.body.size());
		}
		return;
	}
	this->data_rs.headers["Content-Type"] = "text/html";
	this->data_rs.headers["Content-Length"] = to_string_98(client::errorPages[this->data_rs.status_code].size());
	this->data_rs.body = client::errorPages[this->data_rs.status_code];
}

void client::setStatusCode()
{
	if(this->data_rs.status_code < 0)
	{
		if(this->data_rq.method == "POST")	
			this->data_rs.status_code = 201; // Created
		if(this->data_rq.method == "DELETE")	
			this->data_rs.status_code = 204; // No Content
	}
}

void client::handleResponse(int currentFd)
{
    setStatusCode();
    if(this->data_rs.status_code < 0 || this->data_rq.isCgi)
    {
        try
        {
            //////////////ReSend if not "/" in the end////////////////
			if (data_rq.path[data_rq.path.size() - 1] != '/' && this->data_rq.myCloseLocation)
			{
				string locPath = normalizePath(this->data_rq.myCloseLocation->path);
				string reqPath = normalizePath(data_rq.path);
				string root;
				std::map<std::string, std::vector<std::string> >::iterator itRoot = this->data_rq.myCloseLocation->infos.find("root");
				if(itRoot == this->data_rq.myCloseLocation->infos.end()) 
				throw(404);
			
			root  = this->data_rq.myCloseLocation->getInfos("root")->at(0) + "/";          
			string resendPath = switchLocation(locPath, reqPath, root);
			string indexFound = checkIndexes(this->data_rq.myCloseLocation, resendPath + "/");
			if (isDirectory(resendPath) && indexFound != "") {
				
				std::string newLocation = data_rq.path + "/";
				std::string response = 
					"HTTP/1.1 301 Moved Permanently\r\n"
					"Location: " + newLocation + "\r\n"
					"Content-Length: 0\r\n"
					"Connection: close\r\n"
					"\r\n";
	
				send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);
				// closeConnection = true;
				return;
				}
			}

			/////////////////// CGI /////////////////////////
			if (this->data_rq.method != "DELETE" && this->data_rq.myCloseLocation)
			{
				if (isCgiConfigured(this->data_rq.myCloseLocation))
				{
					string locPath = normalizePath(this->data_rq.myCloseLocation->path);
					string reqPath = normalizePath(data_rq.path);
					string root;
					vector<string>* exts;
	
					std::map<std::string, std::vector<std::string> >::iterator itRoot = this->data_rq.myCloseLocation->infos.find("root");
					if(itRoot == this->data_rq.myCloseLocation->infos.end()) 
						throw(404);
					root  = this->data_rq.myCloseLocation->getInfos("root")->at(0) + "/";    
					
					string cgiPath = switchLocation(locPath, reqPath, root);
					std::map<std::string, std::vector<std::string> >::iterator itCgi = this->data_rq.myCloseLocation->infos.find("cgi_extension");
					if(itCgi == this->data_rq.myCloseLocation->infos.end()) 
						throw(404);
					exts = this->data_rq.myCloseLocation->getInfos("cgi_extension");
					
					if (isDirectory(cgiPath) && checkIndexes(this->data_rq.myCloseLocation, cgiPath + "/") != "")
						cgiPath = checkIndexes(this->data_rq.myCloseLocation, cgiPath+ "/");
					if (checkExtension(cgiPath, *exts))
					{
						// --- Start CGI asynchronously ---
						if (!cgi_running) {
							int pipefd[2];
							int inputPipe[2];
							bool needInputPipe = (data_rq.method == "POST" && !data_rq.bodyNameFile.empty());
							if (pipe(pipefd) != 0) {
								throw(500);
							}
							if (needInputPipe && pipe(inputPipe) != 0) {
								close(pipefd[0]);
								close(pipefd[1]);
								throw(500);
							}
							cgi_pid = fork();
							if (cgi_pid == 0) {
								// Child
                                dup2(pipefd[1], STDOUT_FILENO);
                                close(pipefd[0]);
                                close(pipefd[1]);
                                // For POST, redirect stdin from input pipe
                                if (needInputPipe) {
                                    dup2(inputPipe[0], STDIN_FILENO);
                                    close(inputPipe[0]);
                                    close(inputPipe[1]);
                                }
                                std::string cgiInterp = getCgiInterpreter(cgiPath, data_rq.myCloseLocation);
                                if (cgiInterp.empty())
                                    _exit(1);
                                char *argv[] = {
                                    (char*)cgiInterp.c_str(),
                                    (char*)cgiPath.c_str(),
                                    NULL
                                };
                                std::vector<std::string> envStrings;
                                std::vector<char*> envp = setupCgiEnvironment(cgiPath, data_rq, data_rq.bodyNameFile, envStrings);
                                execve(cgiInterp.c_str(), argv, &envp[0]);
                                _exit(1);
                            } else if (cgi_pid > 0) {
                                // Parent
                                close(pipefd[1]);
                                if (needInputPipe) {
                                    close(inputPipe[0]);
                                    // Write POST body to CGI stdin
                                    std::ifstream bodyFile(data_rq.bodyNameFile.c_str(), std::ios::binary);
                                    if (bodyFile.is_open()) {
                                        char buffer[4096];
                                        while (bodyFile.read(buffer, sizeof(buffer)) || bodyFile.gcount() > 0) {
                                            ssize_t bytesRead = bodyFile.gcount();
                                            if (write(inputPipe[1], buffer, bytesRead) != bytesRead)
                                                break;
                                        }
                                        bodyFile.close();
                                    }
                                    close(inputPipe[1]);
                                }
                                cgi_fd = pipefd[0];
                                cgi_running = true;
                                cgi_start_time = time(NULL);
                                // Do not send response yet, return to event loop
                                return;
                            } else {
                                // fork failed
                                if (needInputPipe) {
                                    close(inputPipe[0]);
                                    close(inputPipe[1]);
                                }
                                close(pipefd[0]);
                                close(pipefd[1]);
                                throw(500);
                            }
                        }
                        // If already running, do nothing here (handled in main loop)
                        return;
                    }
                }
            }
			
		/////////////////////////GET with CHUNKED TRANSFER///////////////////////
			if(this->data_rq.method == "GET" && this->data_rs.status_code < 0)
			{
				handleGetRequestWithChunking(currentFd);
				// cout << "inGET************************************************" << endl;
				// closeConnection = true;
				return;
			}
		}
		catch(const int &statusCode)
		{
			this->data_rs.status_code = statusCode;
		}
	}
	std::map<std::string, std::string>::iterator it = this->myServer.errorPages.find(to_string_98(this->data_rs.status_code));// ila kanet tehet get 
	if(it != this->myServer.errorPages.end())
	{
		std::string content;
		if(readFileContent(it->second, content))
		{
			this->data_rs.startLine = "HTTP/1.1 " + to_string_98(this->data_rs.status_code) + " " + client::description[this->data_rs.status_code] + "\r\n";
			this->data_rs.headers["Content-Type"] = "text/html; charset=UTF-8";
			this->data_rs.headers["Content-Length"] = to_string_98(content.size());
			this->data_rs.body = content;
			std::string response = buildResponse();
			send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);
			this->closeConnection = true;
			return ;
		}
	}
    setDataResponse();
    std::string response = buildResponse();
    send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);
	this->closeConnection = true;
}

void client::check_http_body_rules()
{
	if(this->data_rq.method != "GET" && this->data_rq.method != "POST" && this->data_rq.method != "DELETE")
		throw(501);// Not Implemented
	
	std::map<std::string, std::string>::iterator it_cLenght   = this->data_rq.headers.find("content-length");
	std::map<std::string, std::string>::iterator it_cType     = this->data_rq.headers.find("content-type");
	std::map<std::string, std::string>::iterator it_cEncoding = this->data_rq.headers.find("transfer-encoding");
	std::map<std::string, std::string>::iterator is_Host      = this->data_rq.headers.find("host");
	std::map<std::string, std::string>::iterator isChunked    = this->data_rq.headers.find("transfer-encoding");

	if(isChunked != this->data_rq.headers.end() && isChunked->second == "chunked")
		this->data_rq.is_chunked = 1;
	if(it_cLenght != this->data_rq.headers.end())
	{
		if(!isMatch("\\d+", it_cLenght->second))
			throw(400); //Bad Request
		this->data_rq.size_body = atoi(this->data_rq.headers["content-length"].c_str());
	}
	if(is_Host == this->data_rq.headers.end())
		throw(400); //Bad Request
	if(this->data_rq.method == "POST")
	{
		if(it_cEncoding == this->data_rq.headers.end() && it_cLenght == this->data_rq.headers.end())
			throw(411);//Length Required
		if(it_cType == this->data_rq.headers.end() && this->data_rq.size_body > 0)
			throw(400); //Bad Request
		std::map<std::string, std::vector<std::string> >::iterator it = this->data_rq.myCloseLocation->infos.find("upload_store");
		if(it == this->data_rq.myCloseLocation->infos.end())
			throw(404);
		if(this->data_rq.isCgi == 0)
			this->data_rq.bodyNameFile = this->data_rq.myCloseLocation->infos["upload_store"][0] + "/" + RandomString(5) + getExtension(this->data_rq);
		else
			this->data_rq.bodyNameFile = "/tmp/" + RandomString(5);//check protect 
	}
}

void client::handleGetRequestWithChunking(int currentFd)
{
    location* loc = getClosestLocation(this->myServer, data_rq.path);
    if (!loc)
        throw(404);

    // If we're already sending a file in chunks
    if (this->fileStream->is_open() && this->bytesRemaining > 0) {
        this->sendFileChunk(currentFd);
        return;
    }

    // If headers not sent yet, prepare and send the response
    if (!this->headersSent && !this->fileStream->is_open()) {
        try {
            std::string response = handleGetRequest(this->data_rq, loc, this);
            ssize_t sent = send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);
            if (sent > 0) {
                // If this was a chunked response (large file), headers are sent
                if (this->fileStream->is_open() && this->bytesRemaining > 0) {
                    this->headersSent = true;
                    this->sendFileChunk(currentFd);
                    return;
                } else {
                    // Small file or directory listing - response complete
                    this->closeConnection = true;
                    return;
                }
            }
        } catch(const int &statusCode) {
            throw(statusCode);
        }
    }

    // Continue sending file chunks if we're in the middle of a transfer
    if (this->headersSent && this->fileStream->is_open() && this->bytesRemaining > 0) {
        this->sendFileChunk(currentFd);
        return;
    }

    // File transfer complete
    if (this->fileStream->is_open() && this->bytesRemaining == 0) {
        this->fileStream->close();
        this->closeConnection = true;
        return;
    }
}
