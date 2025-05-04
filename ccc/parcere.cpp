#include "parcere.hpp"

parcere::parcere()
{
}

parcere::~parcere()
{
}

parcere::parcere(std::string str)
{
    if(!parce(str))
        throw std::runtime_error("rjee3 rje3 \n");
}