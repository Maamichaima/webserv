#include "../../_includes/client.hpp"
#include "../../_includes/parser.hpp"
#include "../methods/PostMethod.hpp"
#include "../methods/GetMethod.hpp"
#include "../methods/DeleteMethod.hpp"

parser::parser()
{
}

parser::parser(const parser &obj)
{
    *this = obj;
}

parser &parser::operator=(const parser &obj)
{
	(void) obj;
    return *this;
}

parser::~parser()
{
}

int parser::parse(client &client)
{
	if(client.flagProgress == 0)
	{
		std::string start_line = get_line(client.buffer);
		if(start_line.find("\r\n") == std::string::npos)
			return -2;
		else if(parse_startligne(start_line))
		{
			this->setDateToStruct(client, start_line);
			client.flagProgress = 1;
			client.buffer.erase(0, start_line.size());
			if(client.data_rq.version != "HTTP/1.1" && client.data_rq.version != "HTTP/1.0")
				throw (505); //HTTP Version Not Supported
		}
		else
			throw (400); //Bad Request start line probl
	}
	if(client.flagProgress == 1)// check headers dup
	{
		std::string header = get_line(client.buffer);
		while (header != "\r\n" && header.find("\r\n") != std::string::npos)
		{
			if(parse_header(header))
			{
				this->setDateToStruct(client, header);
				client.buffer.erase(0, header.size());
			}
			else
				throw (400);//Bad Request start headers probl
			header = get_line(client.buffer);
		}
		if(header == "\r\n")
		{
			client.check_http_body_rules();
			client.flagProgress = 2;
			client.buffer.erase(0, header.size());
			std::map<std::string, std::vector<std::string> >::iterator it = client.data_rq.myCloseLocation->infos.find("allowed_methods");
			if(it != client.data_rq.myCloseLocation->infos.end() && std::find(it->second.begin(), it->second.end(), client.data_rq.method) == it->second.end())
				throw (405); // Method Not Allowed
			if(client.data_rq.method == "DELETE")
				deleteMethode(client);
		}
	}
	if(client.flagProgress == 2)
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
					throw (400); //Bad Request
			}
			else if(client.data_rq.size_chunked > 0)
			{
				std::string body = get_line_size(client.buffer, client.data_rq.size_chunked);
				client.data_rq.size_chunked -= body.size();
				this->setDateToStruct(client, body);
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
						client.flagProgress = 3;
				}
				else
					throw (400); //Bad Request
			}
		}
		else 
		{
			if(client.data_rq.size_body)
			{
				std::string body = get_line_size(client.buffer, client.data_rq.size_body);
				this->setDateToStruct(client, body);
				client.data_rq.size_body -= body.size();
				client.buffer.erase(0, body.size());
			}
			if(client.data_rq.size_body == 0)
				client.flagProgress = 3;
		}
	}
	return -1;
}

void parser::setDateToStruct(client &client, const std::string &buffer)
{
    if(client.flagProgress == 0)
    {
        std::string str = get_line(buffer);
        std::deque<std::string> startLine = split(str, " ");
        client.data_rq.method = startLine[0];
		size_t pos = startLine[1].find("?");
		if(str.size() >= 8192)
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
    if(client.flagProgress == 1)
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
				throw(404); //Not Found
			std::map<std::string, std::vector<std::string> >::iterator it = client.data_rq.myCloseLocation->infos.find("redirect");
			if(it != client.data_rq.myCloseLocation->infos.end())
			{
				client.data_rs.headers["Location"] = client.data_rq.myCloseLocation->infos["redirect"][1];
				client.data_rs.flaIsRedirect = 1;
				throw(std::atoi(client.data_rq.myCloseLocation->infos["redirect"][0].c_str()));
			}
			std::map<std::string, std::vector<std::string> >::iterator itEx = client.data_rq.myCloseLocation->infos.find("cgi_extension");
			std::map<std::string, std::vector<std::string> >::iterator itPath = client.data_rq.myCloseLocation->infos.find("cgi_path");
			if(itEx != client.data_rq.myCloseLocation->infos.end() && itPath != client.data_rq.myCloseLocation->infos.end())
				client.data_rq.isCgi = 1;
		}
    }
    if(client.flagProgress == 2)
    {
		if(client.data_rq.method == "POST")
            post(client, buffer);
    }
}

