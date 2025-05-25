#include <fstream>
#include "GetMethod.hpp"
#include "../../_includes/client.hpp"

std::string RandomString(int len)
{
   std::string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
   std::string newstr;
   int pos;
   while(newstr.size() != len) {
    pos = ((rand() % (str.size() - 1)));
    newstr += str.substr(pos,1);
   }
   return newstr;
}

std::string getExtention(data_request data)
{
    std::string tex;
    std::map<std::string, std::string>::iterator it = data.headers.find("content-type");
    if(it != data.headers.end())
    {
        tex = data.headers["content-type"];
        size_t pos = tex.find("/");
        return tex.substr(pos + 1);
    }
    return "";
}

void post(const client &client, std::string buffer)
{
    // std::cout << "path request " << client.data_rq.path << std::endl;
    location *location = getClosestLocation(client.myServer, client.data_rq.path);
    std::map<std::string, std::vector<std::string>>::iterator it = location->infos.find("upload_store");
    if(location && it != location->infos.end())
    {
        // std::cout << getExtention(client.data_rq) << "\n";
        std::string name_file = location->infos["upload_store"][0] + client.data_rq.bodyNameFile + "." + getExtention(client.data_rq);
		// std::cout << "my body file " << name_file << "\n";
		std::ofstream file(name_file, std::ios::app);
        // data_rq.body.append(buffer);
		file << buffer;
		file.close();
    }
    else 
        throw std::runtime_error("you need the upload store in your location ...");
}