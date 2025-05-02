#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "RegEx.hpp"
#include <map>
std::string get_line(std::string str)
{
    static size_t i = 0;
    std::string line;

    if (i >= str.size())
        return "\r\n";

    size_t pos = str.find("\r\n", i);
    if (pos == std::string::npos)
    {
        // No more \r\n found, return the rest of the string
        line = str.substr(i);
        i = str.size(); // move index to end
        return line;
    }

    // Extract line including \r\n
    line = str.substr(i, pos - i + 2);
    i = pos + 2; // move index to after \r\n
    return line;
}

int check_ismethod(std::string str)//, int &pos1, int &flag)
{
	if(!isMatch("[A-Z]+", str))//.substr(0, pos1)))
	{
		std::cout<< str << "\n";
		return 0;
	}
	return 1;
}

int check_ispath(std::string str)//, int &pos, int &flag)
{
	if(!isMatch("/.*", str))//.substr(start)))
	{
		// std::cout <<"hadi----" <<  str<< "--\n";
		return 0;
	}
	return 1;
}

int is_whitespace(char c)// deja kayena
{
	if(c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '\n')
		return 1;
	return 0;
}

int valid_n_vr(std::string str)//213abc \d+  
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
	//empty
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

int check_version(std::string str)//, int &pos, int &flag)
{
	std::string ver = "HTTP/";
	// std::cout << str << "\n";
	int size = str.length();
	if(size > 5)
		size = 5;
	// std::cout << "--" << ver.substr(0, size) << "--" << str.substr(0, size) << "--\n";
	if(!isMatch(ver.substr(0, size), str.substr(0, size)))
		return 0;
	// std::cout << "--" << str.substr(size, str.length()) << "--\n";
	std::deque<std::string> num = split(str.substr(size, str.length()), '.');
	if(num.size() == 2)
	{
		// std::cout << "---" << str.substr(size, str.length()) << "--\n";
		if(str[size] && !isMatch("\\d+\\.\\d+\r\n", str.substr(size, str.length())))
		{
			// std::cout <<"hh\n";

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

//flag hwa bach ghan3rfo achmen part 
int pars_startligne(std::string str)// zidi ila kant kamla ??
{
	if(!isMatch("[A-Z]+ /.* HTTP/\\d+\\.\\d+\r\n", str))
	{
		return 0;
	}
	// std::deque<std::string> dq = split(str, ' ');
	// int count = dq.size();
	// int i = 0;

	// while (i < count)
	// {
	// 	switch (i)
	// 	{
	// 		case 0:
	// 			if(!check_ismethod(dq[i]))
	// 			{
	// 				// std::cout << "method \n";
	// 				return 0;
	// 			}
	// 			break;
	// 		case 1:
	// 			if(!check_ispath(dq[i]))
	// 			{
	// 				// std::cout << "path \n";
	// 				return 0;
	// 			}
	// 			break;
	// 		case 2:
	// 			if(!check_version(dq[i]))
	// 			{
	// 				// std::cout << "version \n";
	// 				return 0;
	// 			}
	// 			break;
	// 		default:
	// 			break;
	// 	}
	// 	i++;
	// }
	return 1;
}

int parce_header(std::string str)
{
	// if(str.find("\r\n") != std::string::npos)
	// {
		if(!isMatch("\\S+: .+\r\n", str))
		{
			// std::cout << "hh\n";
			return 0;
		}
	// }
	// else
	// {
	// 	std::deque<std::string> s = split(str, ' ');
	// 	if(s.size() == 2)
	// 	{
	// 		if(!isMatch("\\S+:", s[0]) || !isMatch(".+", s[1]))
	// 			return 0;
	// 	}
	// 	else if(s.size() == 1)
	// 	{
	// 		if(!isMatch("\\S+:?", s[0]))
	// 			return 0;
	// 	}
	// 	else
	// 		return 0;
	// }
	return 1;
}
	
int parce(std::string http_rq)// jm3 hena koulchi 
{
	std::string start_line = get_line(http_rq);
		// std::cout << "start_line = " << start_line;
	if(pars_startligne(start_line))
		std::cout << "start line valide \n";
	else
	{
		std::cout << "start line problem \n";
		return 0;
	}
	std::string header = get_line(http_rq);
	while (header != "\r\n")
	{
		// std::cout << "henaaa\n";
		// std::cout << "header = " << header;
		if(parce_header(header))
			std::cout << "headr valide \n";
		else
		{
			std::cout << "headrs prob \n";
			return 0;
		}
		header = get_line(http_rq);
	}
	return 1;
}

int main()
{
	// std::string htt = "GET /ggg///////lllll HTTP/1.1\r\nHost: localhost:1234\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nsec-ch-ua: \"Google Chrome\";v=\"135\", \"Not-A.Brand\";v=\"8\", \"Chromium\";v=\"135\"\r\nsec-ch-ua-mobile: ?0\r\nsec-ch-ua-platform: \"macOS\"\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/135.0.0.0 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\nSec-Fetch-Site: none\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate, br, zstd\r\nAccept-Language: fr-FR,fr;q=0.9,en-US;q=0.8,en;q=0.7,es;q=0.6";
	std::string htt = "HTT /j HTTP/4.6\r\nhhhh: dg\r\n";
	if(parce(htt))
		std::cout << "Naziii\n";
	else
		std::cout << "rjee3 rjee3 \n";
}

//GET /JHFD HTTP/3.5\r\n