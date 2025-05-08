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
	if(client.flag == 0){
	    std::string start_line = get_line(client.buffer);
		if(start_line.find("\r\n") == std::string::npos)
			return 2;
		else if(pars_startligne(start_line))
		{
			// std::cout << "hada 9beel ->" << client.buffer << "\n";
			std::cout << start_line << " start line valide \n";
			client.flag = 1;//hezi data dyaalek bach tmshiha
			this->setDateToStruct(client.data_rq, start_line, client.flag);
			client.buffer.erase(0, start_line.size());
			// std::cout << "hada b3ed ->" << client.buffer << "\n";
		}
		else
		{
			// std::cout << "=====" << start_line << "===== start line problem \n";
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
				std::cout << header<< "header valide \n";
				client.flag = 2;
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
	}
	return 1;
}
