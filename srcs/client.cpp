#include "../_includes/client.hpp"

client::client()
{
	this->flag = 0;
	this->data_rq.size_body = 0;
	this->data_rq.size_chunked = -1;
	this->data_rq.flag_chunked = 0;
	// std::cout << data_rq.size_chunked << "\n";
	// exit (0);
}

client::client(std::string buff, int fd) : parc(parser())
{
	this->buffer = buff;
	this->fd_socket = fd;
	this->flag = 0;
	this->data_rq.size_body = 0;
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
		std::string>::iterator it = this->data_rq.headers.find("Transfer-Encoding");
	// if(it != this->data_rq.headers.end())
	// checkBodyEncoding(this->data_rq.body.str());
	// else
}

void client::printClient()
{
	std::cout << "Method --> " << this->data_rq.method << std::endl;
	std::cout << "Path --> " << this->data_rq.path << std::endl;
	std::map<std::string, std::string>::iterator it;
	for (it = this->data_rq.headers.begin(); it != this->data_rq.headers.end(); it++)
	{
		// std::cout << "hh\n";
		std::cout << "key --> " << it->first << " value --> " << it->second << std::endl;
	}
	std::cout << "this is our body " << data_rq.body << "\n";
}

void client::setBuffer(std::string str)
{
	// std::cout <<"hada buffer l9dim "<<  this->buffer << "---- hada li zdna lih " << str << "\n";
	this->buffer.append(str);
	// std::cout << "\033[31m" << "haniii!" << "\033[0m" << std::endl;
	// if(this->buffer.find("\r\n") != std::string::npos)
	// if(this->flag != 3)
	// parc.parcHttpCall(*this);
    // if(checkRequestProgress())
	//     this->printClient();

	// if(str.size() == 0)
	//     this->flag = -1;
}

int client::checkRequestProgress() 
{
	std::map<std::string, std::string>::iterator it = this->data_rq.headers.find("Transfer-Encoding");
	std::map<std::string, std::string>::iterator it1 = this->data_rq.headers.find("Content-Length");
    
	// std::cout << this->flag << "\n";//== 2;
	// std::cout << (it == this->data_rq.headers.end()) << "\n";//== 2;
	// std::cout << (it1 == this->data_rq.headers.end() )<< "\n";//== 2;
    if(this->flag == 3)
        return 1;
    else if(this->flag == 2 && (it == this->data_rq.headers.end() && it1 == this->data_rq.headers.end()))
        return 1;
    return 0;
}

void client::parseRequest()
{
	parc.parcHttpCall(*this);
    if(checkRequestProgress())
	{
	    this->printClient();
			
	}
	    // this->printClient();
}
