#pragma once

#include <iostream>
#include <deque>
#include <stack>
#include <cstdlib>


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
	BacktrackElement(bool b, int c, const RegEx& r): isBacktrack(b), charTaken(c), regex(r) {}
	bool isBacktrack;
	int charTaken;
	RegEx regex;
};

// struct 
bool isMatch(std::string ex, std::string testStr);