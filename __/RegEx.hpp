#pragma once

#include <iostream>
#include <deque>
#include <stack>


enum TokenType
{
    ANYTHING,
    LITTERAL,

	DIGIT,
	NOTDIGIT,

	WHITESPACE,
	NOTWHITESPACE,

	ALPHANUM,
	NOTALPHANUM,
	CHARSET,
};

enum Quantifier
{
    EXACTLY_ONE,
    ZERO_OR_MORE,
    ZERO_OR_ONE
};


class RegEx
{
	public:
		TokenType type;
		Quantifier quantifier;
		char content;
		std::string cc;
		// RegEx();
		RegEx(char content, TokenType type);
		RegEx(std::string cc, TokenType type);
		RegEx(char content, std::string cc, TokenType type);
};

struct BacktrackElement
{
	bool isBacktrack;
	int charTaken;
	RegEx regex;
};
bool isMatch(std::string ex, std::string testStr);