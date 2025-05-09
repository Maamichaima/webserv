#include "client.hpp"

client::client()
{
}

client::client(std::string buff, int fd): parc(parcere())
{
    this->buffer = buff;
    this->fd_socket = fd;
    this->flag = 0;
    this->data_rq.size_body = 0;
}

client &client::operator=(const client &obj)
{
    if(this != &obj)
    {
        this->buffer = obj.buffer;
        this->fd_socket = obj.fd_socket;
    }
    return *this;
}

client::~client()
{
}

void parcere::setDateToStruct(data_request &data_rq, std::string &buffer, int flag)
{
    std::cout << "buffer -> " << buffer << "\n";
    if(flag == 0)
    {
        std::string str = get_line(buffer);
        std::deque<std::string> startLine = split(str, ' ');
        data_rq.method = startLine[0];
        data_rq.path = startLine[1];
    }
    if(flag == 1)
    {
        std::string str = get_line(buffer);
        std::deque<std::string> headr = split(str, ':');//hmrdeeg
        data_rq.headrs[headr[0]] = headr[1].substr(1);
    }
}

void checkBodyEncoding(std::string str)
{
    std::string line = get_line(str);
    
    while(line != "\r\n")
    {
        if(atoi(line.c_str()) != get_line(str).size() - 2)
            throw std::runtime_error("errooor\n");
        line = get_line(str);
    }
}

void client::parceBody()
{
    std::map<std::string, std::string>::iterator it = this->data_rq.headrs.find("Transfer-Encoding");
    // if(it != this->data_rq.headrs.end())
    // checkBodyEncoding(this->data_rq.body.str());
    // else
    
}

void client::printClient()
{
    std::cout << "Method --> " << this->data_rq.method << std::endl;
    std::cout << "Path --> " << this->data_rq.path << std::endl;
    
    std::map<std::string, std::string>::iterator it;
    for(it = this->data_rq.headrs.begin(); it != this->data_rq.headrs.end(); it++)
    {
        std::cout << "key --> " << it->first << " value --> " << it->second << std::endl;
        
    }
    std::cout << data_rq.body;
}

void client::setBuffer(std::string str)
{
    // std::cout <<"hada buffer l9dim "<<  this->buffer << "---- hada li zdna lih " << str << "\n";
    this->buffer.append(str);
    // if(this->buffer.find("\r\n") != std::string::npos)
    parc.parcHttpCall(*this);
    // this->printClient();
}
