#pragma once
#include <string>
#include <regex>

enum class TOKEN
{
	IDENTIFIER,		// names the programmer chooses (x, color, UP)
	KEYWORD,		// names already in the programming language (if, while, return)
	SEPARATOR,		// punctuation characters and paired-delimiters (}, (, ;)
	OPERATOR,		// symbols that operate on arguments and produce results (+, <, =)
	LITERAL,		// numeric, logical, textual, reference literals (true, 6.02e23, "music")
	COMMENT,		//  line, block (/* Retrieves user data */)
};

struct element
{
	std::string lexeme;
	TOKEN token;
	int id;
};

struct var
{
	std::string keyword;
	std::string identifier;
	std::string literal;
	int id;
};

bool isValidNumber(const std::string& lexeme)
{
	std::regex re("[\-\+]?([0-9]*[.])?[0-9]+([f]|.)?");
	return std::regex_match(lexeme, re);
}

bool isValidInt(const std::string& lexeme)
{
	std::regex re("[\-\+]?([0-9]*)");
	return std::regex_match(lexeme, re);
}

bool isValidString(const std::string& lexeme)
{
	std::regex re("\"(\\.|[^\"])*\"");
	return std::regex_match(lexeme, re);
}

bool isValidChar(const std::string& lexeme)
{
	std::regex re("\'(\\.|[^\"]){1}\'");
	return std::regex_match(lexeme, re);
}

bool isValidBool(const std::string& lexeme)
{
	if ((lexeme == "true") || (lexeme == "false")) return true;
	else return false;
}

int getNextValidID(const std::vector<element>& table)
{
	int validID = -1;
	for (auto& e : table)
	{
		if (e.id > validID) 
			validID = e.id;
	}

	validID++;
	return validID;
}
