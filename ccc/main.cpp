#include "client.hpp"

int main()
{
    try
    {
        client client("GET / HTTP/2.6\r\nhhh: ds\r\n\r\njhgbdjbhjbjhbdf\r\n", 0);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}