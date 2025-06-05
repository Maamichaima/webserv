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
	if(unlink(path.c_str()) == -1)
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
		// std::cout << name << "\n";
		if (name == "." || name == "..")
			continue;
		std::string fullPath = path + "/" + name;
		deleteDir(fullPath);
	}
	closedir(dir);
	if (rmdir(path.c_str()) == 0)
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

void deleteMethode(std::string path)
{
	if(path == "/")
    if (pathType(path) == 1)//is a dir
	{
		if(path[path.length() - 1] != '/')
			throw(409);
		throw (deleteDir(path));
	}
	else if(pathType(path) == 2) //is file 
	{
		deleteFile(path);
	}
	else
		throw (404);// file no exixte ;
}