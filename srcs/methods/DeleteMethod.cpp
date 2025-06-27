#include "DeleteMethod.hpp"

#include <iostream>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

int pathType(const std::string& path)
{
	struct stat pathStat; // statistique d un fichier 
	if(stat(path.c_str(), &pathStat) != 0)
	{
		throw(404);
	}
	if(S_ISDIR(pathStat.st_mode))
		return 1;
	else if(S_ISREG(pathStat.st_mode))
		return 2;
	return 0;
}

void deleteFile(const std::string& path)
{
	if(std::remove(path.c_str()) == -1)
		throw (403); //forbiden no permision to delete
	throw (204);//no content 
}

int deleteDir(const std::string& path)
{
	DIR *dir = opendir(path.c_str());
	if(dir == NULL)
	{
		if(access(path.c_str(), W_OK | X_OK) == -1)
			throw(403);// no permision to delete
		else if(access(path.c_str(), F_OK) == 0)
			throw(404);//path not found 
		throw (500);// mathelch directory mzyan
	}
	struct dirent* entry;
	while((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		if (name == "." || name == "..")
			continue;
		std::string fullPath = path + "/" + name;
		deleteDir(fullPath);
	}
	closedir(dir);// hhh/ccc/./
	if (std::remove(path.c_str()) == 0)
	{
		std::cout << "Directory deleted: " << path << "\n";
		return (204);
	}
	else
	{
		std::cerr << "Failed to delete directory: " << path << "\n";
		return (500);// delete fail
	}
}

std::string getPathToDelete(const client &client)
{
	std::string new_path = client.data_rq.path;
	std::map<std::string, std::vector<std::string> >::iterator it = client.data_rq.myCloseLocation->infos.find("root");
	if(it == client.data_rq.myCloseLocation->infos.end())
		throw(403); // Forbiden
	int pos = new_path.find(client.data_rq.myCloseLocation->path);
	if(pos != std::string::npos)
		new_path.erase(pos, client.data_rq.myCloseLocation->path.length());
	new_path = client.data_rq.myCloseLocation->infos["root"][0] + "/" + new_path;
	std::cout << new_path << "\n";
	return new_path;
}

void deleteMethode(const client &client)
{
	std::string pathtoDelete = getPathToDelete(client);
	if(pathtoDelete == "/")//add path of our project w
		throw(403);
    if (pathType(pathtoDelete) == 1)//is a dir
	{
		if(!pathtoDelete.empty() && pathtoDelete[pathtoDelete.length() - 1] != '/')// ila kan empty 
			throw(409);
		throw (deleteDir(pathtoDelete));
	}
	else if(pathType(pathtoDelete) == 2) //is file 
	{
		deleteFile(pathtoDelete);
	}
	else
		throw (404);// file no exixte ;
}