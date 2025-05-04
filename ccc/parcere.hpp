#pragma once

#include <iostream>

class parcere
{
    public:
        
    public:
        parcere();
        parcere(std::string str);
        parcere(const parcere &obj);
        parcere &operator=(const parcere &obj);
        ~parcere();
};
