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

int parcere::parce(client &client)
{
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
				// std::cout << header<< "header valide \n";
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
		}
	}
	if(client.flag == 2)
	{
	    std::string body = get_line(client.buffer);
		std::map<std::string, std::string>::iterator it = client.data_rq.headrs.find("Transfer-Encoding");
		// if(!isMatch(".+\r\n", body))
		// 	return 1;
		if(it != client.data_rq.headrs.end() && it->second == "chunked")//
		{
			// body = get_line(client.buffer);
			// std::cout << "body -> " << body;
			if(!client.data_rq.size_chunked && isMatch("[123456789abcdef]+\r\n", body))
			{
				client.data_rq.size_chunked = std::stoi(body, 0, 16);//atoi(body.c_str());
				this->setDateToStruct(client.data_rq, body, client.flag);
				client.buffer.erase(0, body.size());
				// client.printClient();
			}
			else if(client.data_rq.size_chunked)
			{
				if(body.size() <= client.data_rq.size_chunked)
				{
					client.data_rq.size_chunked -= body.size();
					this->setDateToStruct(client.data_rq, body, client.flag);
					client.buffer.erase(0, body.size());
				}
			}
			else
				throw std::runtime_error("Body is larger than Content-Length");
			client.printClient();
		}
		else 
		{
			it = client.data_rq.headrs.find("Content-Length");
			//3 cases 
			client.data_rq.size_body = client.buffer.size();
			if(client.data_rq.size_body <= atoi(it->second.c_str()))
			{
				// std::cout << "===" << client.buffer << "===\n";
				client.data_rq.body.append(client.buffer);
			}
			else if(client.data_rq.size_body > atoi(it->second.c_str()))
			{
				// std::cout << "===" << client.data_rq.body.size() << "===\n";
				throw std::runtime_error("Body is larger than Content-Length");
			}
		}
	}
	return 1;
}
