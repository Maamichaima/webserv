#include "../_includes/client.hpp"
# include "../_includes/executeCgi.hpp"
#include "../_includes/GetMethod.hpp"
#include "../_includes/DeleteMethod.hpp"


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
	this->sizeBody				= 0;
	lastActivityTime = time(NULL);
	cgi_pid = -1;
	cgi_fd = -1;
	cgi_running = false;
	cgi_start_time = 0;
	cgi_epoll_added = false; // <--- Add this line

    setErrorPages();
    setDescription();
}

client::client(std::string buff, int fd) : parc(parser()), bytesRemaining(0), headersSent(false), fileSize(0), fileStream(new std::ifstream())
{
	this->buffer 				= buff;
	this->server_fd 			= fd;
	this->flagProgress			= 0;
	this->data_rq.size_body 	= 0;
	this->data_rq.size_chunked 	= -1;
	this->data_rq.flag_chunked 	= 0;
	this->closeConnection 		= false;
	this->data_rq.is_chunked 	= 0;
	this->data_rq.flag_error 	= 0;
	this->data_rs.flaIsRedirect = 0;
	this->data_rs.status_code 	= -1;
	this->data_rq.isCgi			= 0;
	this->sizeBody				= 0;
	lastActivityTime = time(NULL);
	cgi_pid = -1;
	cgi_fd = -1;
	cgi_running = false;
	cgi_start_time = 0;
	cgi_epoll_added = false;

    setErrorPages();
    setDescription();
}

client &client::operator=(const client &obj)
{
	if (this != &obj)
	{
		// Clean up existing fileStream
		if (fileStream != NULL) {
			if (fileStream->is_open()) {
				fileStream->close();
			}
			// delete fileStream;
			fileStream = NULL;
		}
		
		// Copy all basic data
		this->buffer = obj.buffer;
		this->server_fd = obj.server_fd;
		this->flagProgress = obj.flagProgress;
		this->data_rq = obj.data_rq;
		this->data_rs = obj.data_rs;
		this->closeConnection = obj.closeConnection;
		this->parc = obj.parc;
		this->myServer = obj.myServer;
		this->lastActivityTime = obj.lastActivityTime;
		this->sizeBody = obj.sizeBody;
		this->send_buffer = obj.send_buffer;
		
		// CGI related data
		this->cgi_pid = obj.cgi_pid;
		this->cgi_fd = obj.cgi_fd;
		this->cgi_buffer = obj.cgi_buffer;
		this->cgi_running = obj.cgi_running;
		this->cgi_start_time = obj.cgi_start_time;
		this->cgi_epoll_added = obj.cgi_epoll_added;
		
		// File streaming data - reset for safety (don't copy file streams)
		this->fileStream = new std::ifstream();
		this->bytesRemaining = 0;
		this->headersSent = false;
		this->fileSize = 0;
		this->fileToSend = "";
	}
	return (*this);
}

client::~client()
{
	if (fileStream != NULL) {
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
    cgi_epoll_added = false;
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
		// if(checkRequestProgress())
		// 	std::cout << "in parse request client " << this->server_fd << " requested " << data_rq.path << std::endl;

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
            this->data_rs.headers["Connection"] = "close";
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
            try {
                handleDirectoryRedirect();
            } 
			catch(const int &redirectStatus) 
			{
                if (redirectStatus == 0)
                    return;
                else
                    throw(redirectStatus);
            }

			try {
				handleCgiRequest();
			} 
			catch(const int &cgiStatus)
			{
				if (cgiStatus == 0)
					return;
				else
					throw(cgiStatus);
				
			}
			if(this->data_rq.method == "GET" && this->data_rs.status_code < 0)
			{
				handleGetRequestWithChunking(currentFd);
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
			if(send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL) <= 0)
				ServerLogger::serverError("Send failed ");
			this->closeConnection = true;
			return ;
		}
	}
    setDataResponse();
    std::string response = buildResponse();
    if(send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL) <= 0)
		ServerLogger::serverError("Send failed ");
	this->closeConnection = true;
}

void client::check_http_body_rules()
{
	if(this->data_rq.method != "GET" && this->data_rq.method != "POST" && this->data_rq.method != "DELETE")
		throw(501); // Not Implemented
	
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
		// if(this->data_rq.isCgi == 0)
		this->data_rq.bodyNameFile = this->data_rq.myCloseLocation->infos["upload_store"][0] + "/" + RandomString(5) + getExtension(this->data_rq);
		// else
		// 	this->data_rq.bodyNameFile = "/tmp/" + RandomString(5);//check protect 
	}
}

void client::handleGetRequestWithChunking(int currentFd)
{
    location* loc = getClosestLocation(this->myServer, data_rq.path);
    if (!loc)
        throw(404);

    if (!this->fileStream) {
        this->fileStream = new std::ifstream();
        this->bytesRemaining = 0;
        this->headersSent = false;
        this->fileSize = 0;
	}
	// cout << "inGetRequestWithChunking" << endl;
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
                if (this->fileStream && this->fileStream->is_open() && this->bytesRemaining > 0) {
                    this->headersSent = true;
                    this->sendFileChunk(currentFd);
                    return;
                } else {
                    // Small file or directory listing - response complete
                    this->closeConnection = true;
					// delete fileStream;
                    return;
                }
            }
            else
              ServerLogger::serverError("Send failed ");
        } catch(const int &statusCode) {
            throw(statusCode);
        }
    }

    // Continue sending file chunks if we're in the middle of a transfer
    if (this->headersSent && this->fileStream && this->fileStream->is_open() && this->bytesRemaining > 0) {
        this->sendFileChunk(currentFd);
        return;
    }

    // File transfer complete
    if (this->fileStream && this->fileStream->is_open() && this->bytesRemaining == 0) {
        this->fileStream->close();
        this->closeConnection = true;
        return;
    }
}

