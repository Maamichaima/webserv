#include "client.hpp"
#include "parcere.hpp"
parcere::parcere()
{
}

parcere::parcere(const parcere &obj)
{
    *this = obj;
}

parcere &parcere::operator=(const parcere &obj)
{
    // if(this != &obj)
    // {
        
    // }
    return *this;
}

parcere::~parcere()
{
}

void parcere::parcHttpCall(client &client)//client &client 
{
    if(!parce(client))
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

int parcere::parce(client &client)
{
	// std::cout << client.flag << "\n";
	// std::cout << "checking body.............\n";
	if(client.flag == 0)
	{
	    std::string start_line = get_line(client.buffer);
		if(start_line.find("\r\n") == std::string::npos)
			return 2;
		else if(pars_startligne(start_line))
		{
			// std::cout << "hada 9beel ->" << client.buffer << "\n";
			// std::cout << start_line << " start line valide \n";
			this->setDateToStruct(client.data_rq, start_line, client.flag);
			client.flag = 1;//hezi data dyaalek bach tmshiha
			client.buffer.erase(0, start_line.size());
			// std::cout << "hada b3ed ->" << client.buffer << "\n";
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
			// std::cout << "header ==" << header << "==\n";
			if(parce_header(header))
			{
				// std::cout << header << "header valide \n";
				// client.flag = 2;
				this->setDateToStruct(client.data_rq, header, client.flag);
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
			client.flag = 2;
			client.buffer.erase(0, header.size());
			client.data_rq.size_body = atoi(client.data_rq.headers["Content-Length"].c_str());
		}
		// else if(header == "")
		// {
		// 	client.flag = 3;
		// }
	}
	if(client.flag == 2)
	{
		std::map<std::string, std::string>::iterator it = client.data_rq.headers.find("Transfer-Encoding");
		if(it != client.data_rq.headers.end() && it->second == "chunked")
		{
			if(client.data_rq.size_chunked == -1)
			{
				std::string body = get_line(client.buffer);
				// std ::cout <<  "hada -> " << client.data_rq.size_chunked << " " << body << "\n";
				if(isMatch("[0123456789abcdef]+\r\n", body))
				{
					client.data_rq.flag_chunked = client.data_rq.size_chunked = std::stoi(body, 0, 16);
					this->setDateToStruct(client.data_rq, body, client.flag);
					client.buffer.erase(0, body.size());
				}
				else
					throw std::runtime_error("num error");
			}
			else if(client.data_rq.size_chunked > 0)
			{
				std::string body = get_line_size(client.buffer, client.data_rq.size_chunked);
				// std ::cout <<  "hada -> " << client.data_rq.size_chunked << " " << body << "\n";
				// std::string body_ = get_line(client.buffer);
				client.data_rq.size_chunked -= body.size();
				this->setDateToStruct(client.data_rq, body, client.flag);
				client.buffer.erase(0, body.size());
			}
			else if(client.data_rq.size_chunked == 0)
			{
				std::string body = get_line(client.buffer);
				// std ::cout <<  "hada -> " << client.data_rq.size_chunked << " " << body << "\n";
				if(body == "\r\n")
				{
					client.data_rq.size_chunked = -1;
					this->setDateToStruct(client.data_rq, body, client.flag);
					client.buffer.erase(0, body.size());
					if (client.data_rq.flag_chunked == 0)
						client.flag = 3;
				}
				else
					throw std::runtime_error("Body is larger than Content-Length");
			}
			// std::cout << " body valid \n";
		}
		else 
		{
			if(client.data_rq.size_body)
			{
				std::string body = get_line_size(client.buffer, client.data_rq.size_body);
				// std::cout << body << "\n";
				this->setDateToStruct(client.data_rq, body, client.flag);
				client.data_rq.size_body -= body.size();
				// std::cout << client.data_rq.size_body << "\n";
				client.buffer.erase(0, body.size());
				// exit (0);
			}
			else
			{
				// std::cout << "hh\n";
				client.flag = 3;
				// client.printClient();
				// exit (0);
			}
		}
		
	}
	// if(client.flag == 3)
	// {
	// 	client.printClient();
	// 	exit (0);
	// }
	return 1;





}

void parcere::setDateToStruct(data_request &data_rq, std::string &buffer, int flag)
{
    // std::cout << "buffer -> " << buffer << "\n";
    if(flag == 0)
    {
        std::string str = get_line(buffer);
        std::deque<std::string> startLine = split(str, ' ');
        data_rq.method = startLine[0];
        data_rq.path = startLine[1];
    }
    if(flag == 1)
    {
        std::string str = get_line(buffer);
        std::deque<std::string> headr = split(str, ':');//hmrdeeg
        data_rq.headers[headr[0]] = headr[1].substr(1, headr[1].size() - 3);
    }
    if(flag == 2)
    {
        data_rq.body.append(buffer);
    }
}
