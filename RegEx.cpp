#include "RegEx.hpp"

RegEx::RegEx(char content, TokenType type)
{
	this->content = content;
	this->type = type;
	this->quantifier = EXACTLY_ONE;
}

RegEx::RegEx(std::string cc, TokenType type)
{
	this->cc = cc;
	this->type = type;
	this->quantifier = EXACTLY_ONE;
}

RegEx::RegEx(char content, std::string cc, TokenType type)
{
	this->cc = cc;
	this->content = content;
	this->type = type;
	this->quantifier = EXACTLY_ONE;
}

