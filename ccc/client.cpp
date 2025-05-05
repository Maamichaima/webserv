#include "client.hpp"
#include "parcere.hpp"

client::client()
{
}

client::client(std::string buff, int fd)
{
    this->buffer = buff;
    this->fd_socket = fd;
    parcere parc(buff);
    this->setDateToStruct();
    this->parceBody();
}

client::~client()
{
}

void client::setDateToStruct()
{
    std::string str = get_line(this->buffer, 0);
    std::deque<std::string> startLine = split(str, ' ');
    this->data_rq.method = startLine[0];
    this->data_rq.path = startLine[1];
    str = get_line(this->buffer, 1);
    while(str != "\r\n")
    {
        std::deque<std::string> headr = split(str, ' ');
        this->data_rq.headrs[headr[0].substr(0, headr[0].size() - 1)] = headr[1];
        str = get_line(this->buffer, 1);
    }
    str = get_line(this->buffer, 1);
    while (str != "\r\n")
    {
        this->data_rq.body << str;
        str = get_line(this->buffer, 1);
    }
    this->data_rq.body << "\r\n";
}

void checkBodyEncoding(std::string str)
{
    std::string line = get_line(str, 0);

    // std::cout << "hh\n";
    while(line != "\r\n")
    {
        // if(line)
        // std::cout << atoi(line.c_str());
        if(atoi(line.c_str()) != get_line(str, 1).size() - 2)
            std::cout << "errooor\n";
        line = get_line(str, 1);
    }
}

void client::parceBody()
{
    std::map<std::string, std::string>::iterator it = this->data_rq.headrs.find("Transfer-Encoding");
    if(it != this->data_rq.headrs.end())
    {
        checkBodyEncoding(this->data_rq.body.str());
    }
}

void client::printClient()
{
    std::cout << "Method --> " << this->data_rq.method << std::endl;
    std::cout << "Path --> " << this->data_rq.path << std::endl;

    std::map<std::string, std::string>::iterator it;
    for(it = this->data_rq.headrs.begin(); it != this->data_rq.headrs.end(); it++)
    {
        std::cout << "key : " << it->first << " value : " << it->second << std::endl;

    }
    std::cout << data_rq.body.str();
}