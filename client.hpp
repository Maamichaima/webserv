#pragma once

#include <iostream>

class client
{
    private:
        std::string buffer;
        int fd_socket;
    public:
        client();
        client(std::string buff, int fd);
        client(const client &obj);
        client &operator=(const client &obj);
        ~client();
        void parce_buffer();
};
