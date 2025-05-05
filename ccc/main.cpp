#include "client.hpp"

int main()
{
    try
    {
        client client("GET / HTTP/2.6\r\nkey1: value1\r\nkey2: value2\r\nkey3: value3\r\n\r\n4\r\nWiki\r\n5\r\npedia\r\n0\r\n\r\n", 0);
        client.printClient();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}