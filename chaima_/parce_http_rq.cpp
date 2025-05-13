#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "RegEx.hpp"
#include <map>

std::string get_line(std::string str)
{
    size_t i = 0;
    std::string line;
    size_t pos = str.find("\r\n", i);
    if (pos == std::string::npos)
    {
        line = str.substr(i);
        // i = str.size();
        return line;
    }
    line = str.substr(i, pos - i + 2);
    i = pos + 2;

    return line;
}

int check_ismethod(std::string str)
{
	if(!isMatch("[A-Z]+", str))
	{
		std::cout<< str << "\n";
		return 0;
	}
	return 1;
}

int check_ispath(std::string str)
{
	if(!isMatch("/.*", str))
	{
		return 0;
	}
	return 1;
}

int is_whitespace(char c)
{
	if(c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '\n')
		return 1;
	return 0;
}

int valid_n_vr(std::string str)
{
	int i = 0;
	while(str[i])
	{
		if(!isdigit(str[i]))
			break ;
		i++;
	}
	if(i == 0 || (str[i] && str[i] != '.'))
		return 0;
	i++;
	while(str[i])
	{
		if(!isdigit(str[i]) && !is_whitespace(str[i]))
		{
			std::cout << "--->" << str[i] << std::endl;
			return 0;
		}
		i++;
	}
	return 1;
}


std::deque<std::string> split(const std::string& str, char delimiter)
{
	std::deque<std::string> result;
	std::stringstream ss(str);
	std::string item;

	while (getline(ss, item, delimiter))
	{
		if(item.length() == 0)
			continue;
		result.push_back(item);
	}
	return result;
}

int check_version(std::string str)
{
	std::string ver = "HTTP/";
	int size = str.length();
	if(size > 5)
		size = 5;
	if(!isMatch(ver.substr(0, size), str.substr(0, size)))
		return 0;
	std::deque<std::string> num = split(str.substr(size, str.length()), '.');
	if(num.size() == 2)
	{
		if(str[size] && !isMatch("\\d+\\.\\d+\r\n", str.substr(size, str.length())))
		{
			return 0;
		}
	}
	else if(num.size() == 1)
	{
		if(str[size] && !isMatch("\\d+\\.?", str.substr(size, str.length())))
		{
			return 0;
		}
	}
	else if (num.size() > 2)
		return 0;
	return 1;
}

int pars_startligne(std::string str)
{
	// if(str.find("\r\n") == std::string::npos)
	// 	return 2;
	if(!isMatch("[A-Z]+ /.* HTTP/\\d+\\.\\d+\r\n", str))
	{
		std::cout << "hh\n";
		return 0;
	}
	return 1;
}

int parce_header(std::string str)
{
	// if(str.find("\r\n") == std::string::npos)
	// 	return 2;
	if(!isMatch("\\S+: .+\r\n", str))
		return 0;
	return 1;
}
