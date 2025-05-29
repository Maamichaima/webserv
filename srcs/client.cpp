#include "../_includes/client.hpp"

std::map<int, std::string> client::errorPages;
std::map<int, std::string> client::description;

std::string readFileContent(const std::string& filePath) {
	std::ifstream file(filePath.c_str());
	if (!file.is_open())
		throw std::runtime_error("this file not open" + filePath);

	std::string content;
	std::string line;
	while (std::getline(file, line)) {
		content += line + "\n";
	}
	return content;
}

void setErrorPages()
{
	client::errorPages[400] = readFileContent("errorPages/400.html");
	client::description[400] = "Bad Request";
	client::errorPages[404] = readFileContent("errorPages/404.html");
	client::description[404] = "Not Found";
	client::errorPages[201] = readFileContent("errorPages/201.html");
	client::description[201] = "Created";
	client::errorPages[500] = readFileContent("errorPages/500.html");
	client::description[500] = "Internal Server Error";
	client::errorPages[411] = readFileContent("errorPages/411.html");
	client::description[411] = "Length Required";
	client::errorPages[501] = readFileContent("errorPages/501.html");
	client::description[501] = "Not Implemented";
	client::errorPages[505] = readFileContent("errorPages/505.html");
	client::description[505] = "Version Not Supported";
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

	setErrorPages();
}

client::client(std::string buff, int fd) : parc(parser())
{
	this->buffer = buff;
	this->fd_socket = fd;
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
		this->fd_socket = obj.fd_socket;
	}
	return (*this);
}

client::~client()
{
}

void	checkBodyEncoding(std::string str)
{
	std::string line = get_line(str);
	while (line != "\r\n")
	{
		if (atoi(line.c_str()) != get_line(str).size() - 2)
			throw std::runtime_error("errooor\n");
		line = get_line(str);
	}
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
		 numStatusCode == 500 || numStatusCode == 501 || numStatusCode == 505)
		return 1;
	return 0;
}
int client::checkRequestProgress() 
{
	if(isError(this->data_rs.status_code)) //ila kan / 4 rah error 
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
	// parc.parcHttpCall(*this);
	this->data_rs.status_code = parc.parse(*this);
	// if(this->data_rs.status_code != 1 && this->data_rs.status_code != 2)//ila kant 400 ola 411 ....
	// {// ila kan hadchi ghalet gadi response o sendiha henaaa 
		// std::cout << this->data_rs.status_code << "\n";
		// exit(0);
	// 	// setDataResponse(this->data_rs.status_code);
	// 	// std::cout << "this is the status code --> " << this->data_rs.status_code << "\n";
    //     // throw std::runtime_error("rjee3 rje3 \n");
	// 	return ;
	// }
	// else 
	if (this->data_rs.status_code == 1)
	{
		check_http_body_rules();
	}
    if(checkRequestProgress())
	{
		this->printClient();
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
void client::setDataResponse()
{
	this->data_rs.startLine = "HTTP/1.1 " + std::to_string(this->data_rs.status_code) + " " + client::description[this->data_rs.status_code] + "\r\n";
	this->data_rs.body = client::errorPages[this->data_rs.status_code];
}


void client::handleResponse(int currentFd)
{
	int is_cgi = 0;
	if(is_cgi)//is cgi
	{

	}
	else if(this->data_rq.method == "GET")
	{
		
	}
	else if(this->data_rq.method == "POST" && !isError(this->data_rs.status_code))
	{
		this->data_rs.status_code = 201;
		this->data_rs.headers["Content-Type"] = "text/html; charset=UTF-8";
		this->data_rs.headers["Content-Length"] = std::to_string(client::errorPages[this->data_rs.status_code].size());
		setDataResponse();
		std::string response = buildResponse();
		send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);
	}
	else if(this->data_rq.method == "DELETE")
	{

	}
	else
	{
		// 405 Method Not Allowed 
	}

	if(isError(this->data_rs.status_code))
	{
		this->data_rs.headers["Content-Type"] = "text/html; charset=UTF-8";
		this->data_rs.headers["Content-Length"] = std::to_string(client::errorPages[this->data_rs.status_code].size());
		setDataResponse();
		std::string response = buildResponse();
		send(currentFd, response.c_str(), response.size(), MSG_NOSIGNAL);	
	}
}
void client::check_http_body_rules()
{
	if(this->data_rq.method != "GET" && this->data_rq.method != "POST" && this->data_rq.method != "DELET")
		this->data_rs.status_code = 501;
	
	std::map<std::string, std::string>::iterator it_cLenght = this->data_rq.headers.find("content-length");
	std::map<std::string, std::string>::iterator it_cEncoding = this->data_rq.headers.find("transfer-encoding");
	std::map<std::string, std::string>::iterator is_Host = this->data_rq.headers.find("host");
	if(is_Host == this->data_rq.headers.end())
		this->data_rs.status_code = 400;
	if(this->data_rq.method == "GET")
	{
	}
	else if(this->data_rq.method == "POST")
	{
		if(it_cEncoding == this->data_rq.headers.end() && it_cLenght == this->data_rq.headers.end())
			this->data_rs.status_code = 411;
	}
	else
	{
	}
}
