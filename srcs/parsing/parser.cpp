#include "../../_includes/client.hpp"
#include "../../_includes/parser.hpp"
#include "../methods/PostMethod.hpp"
#include "../methods/GetMethod.hpp"
#include "../methods/DeleteMethod.hpp"
// #include "parser.hpp"
parser::parser()
{
}

parser::parser(const parser &obj)
{
    *this = obj;
}

parser &parser::operator=(const parser &obj)
{
    return *this;
}

parser::~parser()
{
}

std::string get_line_size(std::string str, int size)
{
	std::string sub;

	if(size > str.size())
		return str;
	sub = str.substr(0, size);
		return sub;
}

int isNumber(std::string str)
{
	int i = 0;
	while(str[i])
	{
		if(!isalnum(str[i]))
			return 0;
		i++;
	}
	return 1;
}

int parser::parse(client &client)
{
	if(client.flag == 0)
	{
		std::string start_line = get_line(client.buffer);
		if(start_line.find("\r\n") == std::string::npos)
		{
			return -2;
		}
		else if(parse_startligne(start_line))
		{
			this->setDateToStruct(client, start_line, client.flag);
			client.flag = 1;
			client.buffer.erase(0, start_line.size());
			if(client.data_rq.version != "HTTP/1.1")
				throw (505);
		}
		else
		{
			// std::cout << "=====" << start_line << "===== start line problem \n";
			throw (400);
		}
	}
	if(client.flag == 1)
	{
		std::string header = get_line(client.buffer);
		while (header != "\r\n" && header.find("\r\n") != std::string::npos)
		{
			if(parse_header(header))
			{
				this->setDateToStruct(client, header, client.flag);
				client.buffer.erase(0, header.size());
			}
			else
			{
				std::cout << header << " headers problem \n";
				throw (400);
			}
			header = get_line(client.buffer);
		}
		if(header == "\r\n")
		{
			client.check_http_body_rules();
			client.flag = 2;
			client.buffer.erase(0, header.size());
            client.data_rq.bodyNameFile = RandomString(5);
			std::map<std::string, std::vector<std::string> >::iterator it = client.data_rq.myCloseLocation->infos.find("allowed_methods");
			if(it != client.data_rq.myCloseLocation->infos.end() && std::find(it->second.begin(), it->second.end(), client.data_rq.method) == it->second.end())
				throw (405);
			if(client.data_rq.method == "DELETE")
				deleteMethode(client);
		}
	}
	if(client.flag == 2)
	{
		if(client.data_rq.is_chunked == 1)
		{
			if(client.data_rq.size_chunked == -1)
			{
				std::string body = get_line(client.buffer);
				if(isMatch("[0123456789abcdef]+\r\n", body))
				{
					client.data_rq.flag_chunked = client.data_rq.size_chunked = std::strtol(body.c_str(), NULL, 16);
					client.buffer.erase(0, body.size());
				}
				else
					throw (400);
			}
			else if(client.data_rq.size_chunked > 0)
			{
				std::string body = get_line_size(client.buffer, client.data_rq.size_chunked);
				client.data_rq.size_chunked -= body.size();
				this->setDateToStruct(client, body, client.flag);
				client.buffer.erase(0, body.size());
			}
			else if(client.data_rq.size_chunked == 0)
			{
				std::string body = get_line(client.buffer);
				if(body == "\r\n")
				{
					client.data_rq.size_chunked = -1;
					client.buffer.erase(0, body.size());
					if (client.data_rq.flag_chunked == 0)
						client.flag = 3;
				}
				else
					throw (400);
			}
		}
		else 
		{
			if(client.data_rq.size_body)
			{
				std::string body = get_line_size(client.buffer, client.data_rq.size_body);//hdgfbh
				this->setDateToStruct(client, body, client.flag);
				client.data_rq.size_body -= body.size();
				client.buffer.erase(0, body.size());
			}
			if(client.data_rq.size_body == 0)
				client.flag = 3;
		}
	}
	return -1;
}

void toLower(std::string &str)
{
    int i = 0;

    while(str[i])
    {
        str[i] = tolower(str[i]);
        i++;
    }
}

void checkKeyValueContent(std::string key, std::string value)
{
	key = trim(key);
	value = trim(value);
	if(key.length() == 0 || value.length() == 0)
		throw(400);
	if(!isMatch("[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#$%&'*+\\-.^_`|~]+", key))
		throw(400);
}

void parser::setDateToStruct(client &client, std::string &buffer, int flag)//const ??
{
    if(flag == 0)
    {
        std::string str = get_line(buffer);
        std::deque<std::string> startLine = split(str, " ");
        client.data_rq.method = startLine[0];
		size_t pos = startLine[1].find("?");
		if(startLine[1].size() >= 8000)// khedmi b start line kolha 
			throw (414);// URI Too Long
		if(pos != std::string::npos)
		{
			client.data_rq.path = startLine[1].substr(0, pos);
			client.data_rq.queryContent = startLine[1].substr(pos + 1);
		}
		else
			client.data_rq.path = startLine[1];
        client.data_rq.version = startLine[2].substr(0, startLine[2].size() - 2);
    }
    if(flag == 1)
    {
		std::string str = get_line(buffer);
		size_t pos = str.find(":");
		size_t size = str.find("\r\n");
		std::string key = str.substr(0, pos);
		std::string value = str.substr(pos + 2, size - pos - 2);
		checkKeyValueContent(key, value);
		toLower(key);
        client.data_rq.headers[key] = value;
		if(key == "host")
		{
			client.myServer = *chooseServer(SocketToServers[client.server_fd],client.data_rq.headers["host"]);
			client.data_rq.myCloseLocation = getClosestLocation(client.myServer, client.data_rq.path);
			if(client.data_rq.myCloseLocation == NULL)
				throw(404);
			std::map<std::string, std::vector<std::string> >::iterator it = client.data_rq.myCloseLocation->infos.find("redirect");
			if(it != client.data_rq.myCloseLocation->infos.end())
			{
				client.data_rs.headers["Location"] = client.data_rq.myCloseLocation->infos["redirect"][1];
				client.data_rs.flaIsRedirect = 1;
				throw(std::atoi(client.data_rq.myCloseLocation->infos["redirect"][0].c_str()));
			}
		}
    }
    if(flag == 2)
    {
		if(client.data_rq.method == "POST")
            post(client, buffer);
    }
}

