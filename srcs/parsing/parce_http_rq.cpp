#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "../../RegEx/RegEx.hpp"
#include <map>

int parse_startligne(std::string str)
{
	if(!isMatch("[A-Z]+ /.* HTTP/\\d+\\.\\d+\r\n", str))
	{
		return 0;
	}
	return 1;
}

int parse_header(std::string str)
{
	if(!isMatch("\\S+: .+\r\n", str))
		return 0;
	return 1;
}
