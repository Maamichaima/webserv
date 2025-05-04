#pragma once

#include <iostream>

class parcere
{
    public:
        
    public:
        parcere(std::string str);
        parcere();
        parcere(const parcere &obj);
        parcere &operator=(const parcere &obj);
        ~parcere();
};
int parce(std::string http_rq);