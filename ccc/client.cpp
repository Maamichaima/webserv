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
    /// khode les information o3mer data_request ?
    this->setDateToStruct();
}

client::~client()
{
}

void client::setDateToStruct()
{
    std::string str = get_line(this->buffer);
    std::deque<std::string> startLine = split(str, ' ');
    this->data_rq.method = startLine[0];
    this->data_rq.path = startLine[1];
    str = get_line(this->buffer);
    while(str != "\r\n")
    {
        std::deque<std::string> headr = split(str, ' ');
        this->data_rq.headrs[headr[0]] = headr[1];
        str = get_line(this->buffer);
    }
}
void client::printClient()
{
    // std::cout << "Method --> " << this->data_rq.method << std::endl;
    // std::cout << "Path --> " << this->data_rq.path << std::endl;

    // std::map<std::string, std::string>::iterator it;
    // for(it = this->data_rq.headrs.begin(); it != this->data_rq.headrs.end(); it++)
    // {
    //     std::cout << "key : " << it->first << " value : " << it->second << std::endl;

    // }
}