#include "RegEx.hpp"

void parce_staret(std::deque<RegEx> &dq)
{
	if(dq.empty())
	{
		std::cout << "Error: Empty deque" << std::endl;
		exit (1);
	}
	else if (dq.back().quantifier == EXACTLY_ONE)
	{
		dq.back().quantifier = ZERO_OR_MORE;
	}
	else
	{
		std::cout << "Error: expretion" << std::endl;
		exit (1);
	}
}

void parce_optional(std::deque<RegEx> &dq)
{
	if(dq.empty())
	{
		std::cout << "Error: Empty deque" << std::endl;
		exit (1);
	}
	else if (dq.back().quantifier == EXACTLY_ONE)
	{
		dq.back().quantifier = ZERO_OR_ONE;
	}
	else
	{
		std::cout << "Error: expretion" << std::endl;
		exit (1);
	}
}

void print(std::deque<RegEx> dq)
{
	int i = 0;
	std::deque<RegEx>::iterator it = dq.begin();

	while(it != dq.end())
	{
		std::cout << "Element " << i++ << ": content = " << it->content
                  << ", CC = " << it->cc 
                  << ", quantifier = " << it->quantifier 
                  << ", type = " << it->type << std::endl;
		it++;
	}
}

void parce_plus(std::deque<RegEx> &dq)
{
	if(dq.empty())
	{
		std::cout << "Error: Empty deque" << std::endl;
		exit (1);
	}
	else if (dq.back().quantifier == EXACTLY_ONE)
	{
		dq.push_back(RegEx(dq.back().content, dq.back().cc, dq.back().type));
		dq.back().quantifier = ZERO_OR_MORE;
	}
	else
	{
		std::cout << "Error: expretion" << std::endl;
		exit (1);
	}
}

void escapedCharacter(std::deque<RegEx>& C, char escapedChar) {
    switch (escapedChar)
    {
        case 'd':
            C.push_back(RegEx('\0', DIGIT));
            break;
        case 'D':
            C.push_back(RegEx('\0', NOTDIGIT));
            break;
        case 'w':
            C.push_back(RegEx('\0', ALPHANUM));
            break;
        case 'W':
            C.push_back(RegEx('\0', NOTALPHANUM));
            break;
        case 's':
            C.push_back(RegEx('\0', WHITESPACE));
            break;
        case 'S':
            C.push_back(RegEx('\0', NOTWHITESPACE));
            break;
        default:
            C.push_back(RegEx(escapedChar, LITTERAL));
            break;
    }
}

void parce_(std::deque<RegEx>& dq, std::string str, size_t &i)
{
	std::string s;
	i++;
	size_t pos = str.find(']', i);
	if(pos == std::string::npos)
	{
		std::cout << "Error: expretion" << std::endl;
		exit (1);
	} 
	s = str.substr(i, pos - i);
	dq.push_back(RegEx(s, CHARSET));
	i = pos;
}

std::deque<RegEx> parce_expretion(std::string str)
{
	size_t i = 0;
	std::deque<RegEx> dq;

	while(str[i])
	{
		switch (str[i])
		{
		case '.':
			dq.push_back(RegEx(str[i], ANYTHING));
			dq.back().quantifier = EXACTLY_ONE;
			i++;
			continue;
		case '*':
			parce_staret(dq);
			i++;
			continue;
		case '\\':
			if(i + 1 >= str.size())
			{
				std::cout << "eroor ?";
				exit(1);
			}
			escapedCharacter(dq, str[i + 1]);
			i = i + 2;
			continue;
		case '?':
			parce_optional(dq);
			i++;
			continue;
		case '+':
			parce_plus(dq);
			i++;
			continue;
		case '[':
			parce_(dq, str, i);
			i++;
			continue;
		default:
			dq.push_back(RegEx(str[i], LITTERAL));
			dq.back().quantifier = EXACTLY_ONE;
			i++;
			continue;
		}
		i++;
	}
	return dq;
}

void print_stack(std::stack<BacktrackElement> backtrackStack)
{
	while(!backtrackStack.empty())
	{
		std::cout << "quantifier = " << backtrackStack.top().regex.quantifier 
		<< ", content = " << backtrackStack.top().regex.content << " chartaken " << backtrackStack.top().charTaken << std::endl; 
		backtrackStack.pop();
	}
}


bool tryBacktrack(std::stack<BacktrackElement> &backtrackStack, size_t &index, std::deque<RegEx>::iterator &it)
{
	while (!backtrackStack.empty())
	{
		if (backtrackStack.top().isBacktrack && backtrackStack.top().charTaken)
		{
			backtrackStack.top().charTaken--;
			index--;
			return true;
		}
		else
		{
			index -= backtrackStack.top().charTaken;
			backtrackStack.pop();
			it--;
		}
	}
	return false;
}


int inRange(std::string str, char c)
{
	if(str.size() == 3 && str[1] == '-')
	{
		if(c < str[0] || c > str[2])
		return 0;
	}
	else
	{
		size_t pos = str.find(c);
		if(pos == std::string::npos)
		return 0;
	}
	return 1;
}

bool test(std::string str, std::deque<RegEx> dq)//. [] \d \s \w
{
    size_t i = 0;
    std::deque<RegEx>::iterator it = dq.begin();
	std::stack<BacktrackElement> backtrackStack;

    while (it != dq.end())
    {
		char c = str[i];
        if (it->type == LITTERAL)
        {
			if (it->quantifier == EXACTLY_ONE)
            {
				if (c != it->content || i >= str.size())
				{
					if (tryBacktrack(backtrackStack, i, it))
						continue ;
                    return false;
				}
				backtrackStack.push(BacktrackElement(false, 1, *it));
                i++;
                it++;
            }
            else if (it->quantifier == ZERO_OR_MORE)
            {
				int j = 0;
				while (str[i] && str[i] == it->content)
				{
					j++;
					i++;
				}
				backtrackStack.push(BacktrackElement(true, j, *it));
				it++;
            }
            else if (it->quantifier == ZERO_OR_ONE)
            {
				std::cout <<"hh\n";
				if (c == it->content && i < str.size())
				{
					backtrackStack.push(BacktrackElement(true, 1, *it));
                    i++;
				}
				else
				{
					backtrackStack.push(BacktrackElement(true, 0, *it));
				}
                it++;
            }
        }
		else if(it->type == DIGIT)
		{
			if (it->quantifier == EXACTLY_ONE)
			{
				if (!std::isdigit(c) || i >= str.size())
				{
					if (tryBacktrack(backtrackStack, i, it))
						continue;
					return false;
				}
				backtrackStack.push(BacktrackElement(false, 1, *it));
				i++;
				it++;
			}
			else if (it->quantifier == ZERO_OR_MORE)
			{
				int j = 0;
				while (str[i] && std::isdigit(str[i]))
				{
					j++;
					i++;
				}
				backtrackStack.push(BacktrackElement(true, j, *it));
				it++;
			}
			else if (it->quantifier == ZERO_OR_ONE)
			{
				if (std::isdigit(c) && i < str.size())
				{
					backtrackStack.push(BacktrackElement(true, 1, *it));
					i++;
				}
				else
					backtrackStack.push(BacktrackElement(true, 0, *it));
				it++;
			}
		}
		else if(it->type == NOTDIGIT)
		{
			if (it->quantifier == EXACTLY_ONE)
			{
				if (std::isdigit(c) || i >= str.size())
				{
					if (tryBacktrack(backtrackStack, i, it))
						continue;
					return false;
				}
				backtrackStack.push(BacktrackElement(false, 1, *it));
				i++;
				it++;
			}
			else if (it->quantifier == ZERO_OR_MORE)
			{
				int j = 0;
				while (str[i] && !std::isdigit(str[i]))
				{
					j++;
					i++;
				}
				backtrackStack.push(BacktrackElement(true, j, *it));
				it++;
			}
			else if (it->quantifier == ZERO_OR_ONE)
			{
				if (!std::isdigit(c) && i < str.size())
				{
					backtrackStack.push(BacktrackElement(true, 1, *it));
					i++;
				}
				else
					backtrackStack.push(BacktrackElement(true, 0, *it));
				it++;
			}
		}
		else if(it->type == CHARSET)// [a-z] c est a dire alphabet entre a et z ou [abc] c est a dire un char qui egale a ou b ou c
		{
			if (it->quantifier == EXACTLY_ONE)
			{
				if (!inRange(it->cc, str[i]) || i >= str.size())
				{
					if (tryBacktrack(backtrackStack, i, it))
						continue;
					return false;
				}
				backtrackStack.push(BacktrackElement(false, 1, *it));
				i++;
				it++;
			}
			else if (it->quantifier == ZERO_OR_MORE)
			{
				int j = 0;
				while (str[i] && inRange(it->cc, str[i]))
				{
					j++;
					i++;
				}
				backtrackStack.push(BacktrackElement(true, j, *it));
				it++;
			}
			else if(it->quantifier == ZERO_OR_ONE)
			{
				if (!inRange(it->cc, str[i]) && i < str.size())
				{
					backtrackStack.push(BacktrackElement(true, 1, *it));
					i++;
				}
				else
					backtrackStack.push(BacktrackElement(true, 0, *it));
				it++;
			}
		}
		else if(it->type == ANYTHING)
		{
			if (it->quantifier == EXACTLY_ONE)
			{
				if (!str[i])
				{
					if (tryBacktrack(backtrackStack, i, it))
					continue;
					return false;
				}
				backtrackStack.push(BacktrackElement(false, 1, *it));
				i++;
				it++;
			}
			else if (it->quantifier == ZERO_OR_MORE)
			{
				int j = 0;
				while (str[i])
				{
					j++;
					i++;
				}
				backtrackStack.push(BacktrackElement(true, j, *it));
				it++;
			}
			else if (it->quantifier == ZERO_OR_ONE)
			{
				if (!str[i])
				{
					backtrackStack.push(BacktrackElement(true, 1, *it));
					i++;
				}
				else
					backtrackStack.push(BacktrackElement(true, 0, *it));
				it++;
			}
		}
		else if(it->type == WHITESPACE)
		{
			if (it->quantifier == EXACTLY_ONE)
			{
				if (!std::isspace(c) || i >= str.size())
				{
					if (tryBacktrack(backtrackStack, i, it))
						continue;
					return false;
				}
				backtrackStack.push(BacktrackElement(false, 1, *it));
				i++;
				it++;
			}
			else if (it->quantifier == ZERO_OR_MORE)
			{
				int j = 0;
				while (str[i] && std::isspace(str[i]))
				{
					j++;
					i++;
				}
				backtrackStack.push(BacktrackElement(true, j, *it));
				it++;
			}
			else if (it->quantifier == ZERO_OR_ONE)
			{
				if (std::isspace(c) && i < str.size())
				{
					backtrackStack.push(BacktrackElement(true, 1, *it));
					i++;
				}
				else
				{
					backtrackStack.push(BacktrackElement(true, 0, *it));
				}
				it++;
			}
		}
		else if(it->type == NOTWHITESPACE)
		{
			if (it->quantifier == EXACTLY_ONE)
			{
				if (std::isspace(c) || i >= str.size())
				{
					if (tryBacktrack(backtrackStack, i, it))
						continue;
					return false;
				}
				backtrackStack.push(BacktrackElement(false, 1, *it));
				i++;
				it++;
			}
			else if (it->quantifier == ZERO_OR_MORE)
			{
				int j = 0;
				while (str[i] && !std::isspace(str[i]))
				{
					j++;
					i++;
				}
				backtrackStack.push(BacktrackElement(true, j, *it));
				it++;
			}
			else if (it->quantifier == ZERO_OR_ONE)
			{
				if (!std::isspace(c) && i < str.size())
				{
					backtrackStack.push(BacktrackElement(true, 1, *it));
					i++;
				}
				else
				{
					backtrackStack.push(BacktrackElement(true, 0, *it));
				}
				it++;
			}
		}

        else
        {
            std::cout << "Error: Unsupported type in RegEx" << std::endl;
            return false;
        }
    }
    return (i == str.size() && it == dq.end());
}

bool isMatch(std::string ex, std::string testStr)
{
	std::deque<RegEx> dq = parce_expretion(ex);
	if (test(testStr, dq))
		return true;
    else
		return false;
}

// int main()
// {
//     std::string ex = "\\d+.\\d+\r\n";
//     std::deque<RegEx> dq = parce_expretion(ex);

// 	// print(dq);
//     std::string testStr = "8.5\r\n";
//     if (isMatch(ex, testStr))
//         std::cout << "Matched!" << std::endl;
//     else
//         std::cout << "Not matched!" << std::endl;
//     return 0;
// }
