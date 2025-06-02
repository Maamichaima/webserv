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
		throw std::runtime_error("rje3 rj3 mamsehti wali asat ");
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
	// throw (204);//no content 
}

void deleteDir(const std::string& path)
{
	DIR *dir = opendir(path.c_str());
	if(dir == NULL)
	{
		// if(errno == EACCES)
		// 	throw(403);// no permision to delete
		// else if(errno == ENOENT)
		// 	throw(404);//path not found 
		// throw (500);// mathelch directory mzyan
		
	}

	struct dirent* entry;
	while((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		std::cout << name << "\n";
		if (name == "." || name == "..")
			continue;
		std::string fullPath = path + "/" + name;
		deleteDir(fullPath);
	}
	closedir(dir);

	if (rmdir(path.c_str()) == 0)
	{
		std::cout << "Directory deleted: " << path << "\n";
		//throw(204/200)
	}
	else
	{
		std::cerr << "Failed to delete directory: " << path << "\n";
		throw(500);// delete fail
	}
}

void deleteMethode(std::string path)
{
	// if (access(path.c_str(), F_OK) == -1)
	// {
	// 	std::cerr << "Error: " << strerror(errno) << "\n";
	// }
    if (pathType(path) == 1)//is a dir
	{
		deleteDir(path);
	}
	else if(pathType(path) == 2) //is file 
	{
		deleteFile(path);
	}
	// else
		//throw (404);// file no exixte ;
}