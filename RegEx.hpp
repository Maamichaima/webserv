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

// Element 1: content = g, quantifier = 0, type = 1
// Element 2: content = g, quantifier = 1, type = 1
// Element 3: content = b, quantifier = 0, type = 1
// Element 4: content = h, quantifier = 0, type = 1
// Element 5: content = s, quantifier = 0, type = 1
// Element 6: content = d, quantifier = 2, type = 1