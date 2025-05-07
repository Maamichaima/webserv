#include "parcere.hpp"

parcere::parcere()
{
}

parcere::~parcere()
{
}

parcere::parcere(std::string str, int &flag)
{
    if(!parce(str, flag))
        throw std::runtime_error("rjee3 rje3 \n");
}