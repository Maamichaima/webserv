#include "../../_includes/client.hpp"
#include "../../_includes/parser.hpp"
#include "../methods/PostMethod.hpp"
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
    // if(this != &obj)
    // {
        
    // }
    return *this;
}

parser::~parser()
{
}

void parser::parcHttpCall(client &client)//client &client 
{
    if(!parse(client))
        throw std::runtime_error("rjee3 rje3 \n");
}

std::string get_line_size(std::string str, int size)
{
	std::string sub;

	if(size > str.size())
	return str;
	sub = str.substr(0, size);
	return sub;
}

int parser::parse(client &client)
{
	if(client.flag == 0)
	{
	    std::string start_line = get_line(client.buffer);
		if(start_line.find("\r\n") == std::string::npos)
			return 2;
		else if(parse_startligne(start_line))
		{
			this->setDateToStruct(client, start_line, client.flag);
			client.flag = 1;//hezi data dyaalek bach tmshiha
			client.buffer.erase(0, start_line.size());
		}
		else
		{
			std::cout << "=====" << start_line << "===== start line problem \n";
			return 0;
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
				return 0;
			}
			header = get_line(client.buffer);
		}
		if(header == "\r\n")
		{
			std::map<std::string, std::string>::iterator it = client.data_rq.headers.find("content-length");
		    std::map<std::string, std::string>::iterator it1 = client.data_rq.headers.find("transfer-encoding");
            if(it != client.data_rq.headers.end() && it->second == "chunked")
                client.data_rq.is_chunked = 1;
			client.flag = 2;
			client.buffer.erase(0, header.size());
            client.data_rq.bodyNameFile = RandomString(5);
			if(it != client.data_rq.headers.end())
				client.data_rq.size_body = atoi(client.data_rq.headers["content-length"].c_str());
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
					client.data_rq.flag_chunked = client.data_rq.size_chunked = std::stoi(body, 0, 16);
					client.buffer.erase(0, body.size());
				}
				else
					throw std::runtime_error("num error");
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
					throw std::runtime_error("Body is larger than Content-Length");
			}
		}
		else 
		{
			if(client.data_rq.size_body)
			{
				std::string body = get_line_size(client.buffer, client.data_rq.size_body);
				this->setDateToStruct(client, body, client.flag);
				client.data_rq.size_body -= body.size();
				client.buffer.erase(0, body.size());
				if(client.data_rq.size_body == 0)
					client.flag = 3;
			}
		}
		
	}
	return 1;
}

int parser::check_http_body_rules(client client)
{
	if(client.data_rq.method != "GET" || client.data_rq.method != "POST" || client.data_rq.method != "DELET")
		return 0;
	
	std::map<std::string, std::string>::iterator it_cLenght = client.data_rq.headers.find("content-length");
	std::map<std::string, std::string>::iterator it_cEncoding = client.data_rq.headers.find("transfer-encoding");
	std::map<std::string, std::string>::iterator it_Host = client.data_rq.headers.find("host");
	if(it_Host == client.data_rq.headers.end())
		return 0;
	if(client.data_rq.method == "GET")
	{
		// if()
	}
	else if(client.data_rq.method == "POST")
	{
		if(it_cEncoding == client.data_rq.headers.end() && it_cLenght == client.data_rq.headers.end())
			return 0;//no boody??
	}
	else
	{
	}
	return 1;
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

void parser::setDateToStruct(client &client, std::string &buffer, int flag)
{
    if(flag == 0)
    {
        std::string str = get_line(buffer);
        std::deque<std::string> startLine = split(str, ' ');
        client.data_rq.method = startLine[0];
        client.data_rq.path = startLine[1];
    }
    if(flag == 1)
    {
        std::string str = get_line(buffer);
        std::deque<std::string> headr = split(str, ':');//hmrdeeg ila kan key: ::::
        toLower(headr[0]);
        client.data_rq.headers[headr[0]] = headr[1].substr(1, headr[1].size() - 3);
    }
    if(flag == 2)
    {
		if(client.data_rq.method == "POST")
            post(client, buffer);
    }
}