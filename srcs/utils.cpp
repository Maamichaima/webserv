#include "../_includes/client.hpp"
# include "../_includes/executeCgi.hpp"
#include "methods/DeleteMethod.hpp"

int readFileContent(const std::string& filePath, std::string &content)
{
	std::ifstream file(filePath.c_str());
	if (!file.is_open())
		return 0;

	std::string line;
	while (std::getline(file, line)) {
		content += line + "\n";
	}
	return 1;
}

std::string headersToOneString(std::map<std::string, std::string> headers)
{
	std::string head;

	std::map<std::string, std::string>::iterator it = headers.begin();
	while(it != headers.end())
	{
		head += it->first + ": " + it->second + "\r\n";
		it++;
	}
	head += "\r\n";
	return head;
}

// template <typename T>
// std::string to_string_98(T value)
// {
// 	std::ostringstream oss;
// 	oss << value;
// 	return oss.str();
// }

std::string padLeftToThree(const std::string& input)
{
    if (input.length() >= 3)
        return input;
    std::string padded = input;
    while (padded.length() < 3) {
        padded = "0" + padded;
    }
    return padded;
}

int isRedirect(int red)
{
	if(red == 301 || red == 302 || red == 304 || red == 307 || red == 308)
		return 1;
	return 0;
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
		throw(400); //Bad Request
	if(!isMatch("[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#$%&'*+\\-.^_`|~]+", key))
		throw(400); //Bad Request invalide caracter (key)
}

std::string trim(const std::string& str) {
    size_t start = 0;
    while (start < str.size() && std::isspace(str[start]))
        ++start;

    size_t end = str.size();
    while (end > start && std::isspace(str[end - 1]))
        --end;

    return str.substr(start, end - start);
}

std::string get_line(std::string str)
{
    size_t i = 0;
    std::string line;
    size_t pos = str.find("\r\n", i);
    if (pos == std::string::npos)
    {
        line = str.substr(i);
        return line;
    }
    line = str.substr(i, pos - i + 2);
    i = pos + 2;

    return line;
}

std::deque<std::string> split(const std::string& str, const std::string& delimiter)
{
    std::deque<std::string> result;
    size_t start = 0;
    size_t end;

    while ((end = str.find(delimiter, start)) != std::string::npos)
    {
        if (end != start)
            result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
    }

    if (start < str.length())
        result.push_back(str.substr(start));

    return result;
}

std::string get_line_size(std::string str, size_t size)
{
	std::string sub;

	if(size > str.size())
		return str;
	sub = str.substr(0, size);
		return sub;
}