#include "client.hpp"

client::client()
{
}

client::client(std::string buff, int fd)
{
    this->buffer = buff;
    this->fd_socket = fd;
}

client::~client()
{
}

