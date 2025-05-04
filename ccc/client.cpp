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
}

client::~client()
{
}

