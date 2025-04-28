#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

std::string get_line(std::string str)
{
	static int i = 0;
	std::string line;
	size_t pos = str.find("\r\n");
	if(pos == std::string::npos)
	{
		return str;
	}
	line = str.substr(0, pos + 2);
	return line;
}

int check_ismethod(std::string str, int &pos, int &flag)
{
	int i = 0;

	if(str[i] == ' ')
		return 0;
	while(str[i] && str[i] != ' ')
	{
		if(!isupper(str[i]))
			return 0;
		i++;
	}
	pos = i;
	if(str[i] == ' ')
		flag = 1;
	return 1;
}

int check_ispath(std::string str, int &pos, int &flag)
{
	int i = pos;

	while(str[i] && str[i] == ' ')//find_first_noof
	{
		i++;
	}
	// std::cout << str[i] ;
	// i--;
	if(str[i] != '/')
	{
		// std::cout << str[i] << "hhhhh\n";
		return 0;
	}
	while (str[i] && str[i] != ' ')//find
	{
		i++;
	}
	pos = i;
	// std::cout << "positon -> " << pos << "\n" ;
	if(str[i] == ' ')
		flag = 2;
	return 1;
}

int is_whitespace(char c)// deja kayena
{
	// int i = 0;
	// while(str[i])
	// {
		if(c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '\n')
			return 1;
	// 	i++;
	// }
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


int check_version(std::string str, int &pos, int &flag)
{
	int i = pos;
	std::string ver = "HTTP/";

	while(str[i] && str[i] == ' ')
	{
		i++;
	}
	int size = str.length() - i;
	if(size > 5)
		size = 5;
	std::string cc = str.substr(i, size);
	if(cc != ver.substr(0, size))
	{
		return 0;
	}
	i += size;
	if(str[i] && !valid_n_vr(str.substr(i, str.length())))
		return 0;
	pos = i;
	// if(str[i] == ' ')
	// 	flag = 3;
	return 1;
}

//flag hwa bach ghan3rfo achmen part 
int pars_startligne(std::string str)
{
	static int flag = 0;
	static int pos = 0;
	
	// part of method 
	if(flag == 0)
	{
		if(!check_ismethod(str, pos, flag))
			return 0;
	}
	// part of path
	if(flag == 1)
	{
		// std::cout << pos << " \n";
		// std::cout << str[pos] << "\n";
		// std::cout << "hh \n";
		if(!check_ispath(str, pos, flag))
		{
			return 0;
		}
	}
	if(flag == 2)
	{
		if(!check_version(str, pos, flag))
		{
			return 0;
		}
	}
	return 1;
}

int main()
{
	// std::string htt = "GET /ggg///////lllll HTTP/1.1\r\nHost: localhost:1234\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nsec-ch-ua: \"Google Chrome\";v=\"135\", \"Not-A.Brand\";v=\"8\", \"Chromium\";v=\"135\"\r\nsec-ch-ua-mobile: ?0\r\nsec-ch-ua-platform: \"macOS\"\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/135.0.0.0 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\nSec-Fetch-Site: none\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate, br, zstd\r\nAccept-Language: fr-FR,fr;q=0.9,en-US;q=0.8,en;q=0.7,es;q=0.6";
	std::string htt = "GET /hdf HTTP/2.6  h";

	std::string jj = get_line(htt);
	jj = get_line(htt);
	if(pars_startligne(jj))
		std::cout << "all good \n";
	else
		std::cout << "errrrrroooooor\n";

}
