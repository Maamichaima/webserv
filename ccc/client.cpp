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
}

client::~client()
{
}

