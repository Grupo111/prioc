#pragma once
#include <string>
#include <regex>

//MACRO PRINTS
#define LOG_WRONGPATH_ERROR(path) std::cout << "Falha ao abrir arquivo " << path << std::endl
#define LOG_LEXICAL_ERROR(lexeme) std::cout << "\n-->Erro Lexico, Lexema: " << lexeme << std::endl
#define LOG_SYNTACTIC_ERROR(state) std::cout << "\n-->Erro Sintatico, Estado: " << state << std::endl
#define LOG_SEMANTIC_ERROR(keyword, value) std::cout << "\n-->Erro Semantico " << keyword << " Invalido:  " << value << std::endl
#define LOG_SEMANTICVAR_ERROR(value) std::cout << "\n-->Erro Semantico, Identificador duplicado:  " << value << std::endl
#define LOG_WARN(x) std::cout << x << std::endl

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
	std::string value;
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

bool isValidLiteral(const std::string& lexeme)
{
	if (!isValidNumber(lexeme) && !isValidString(lexeme) && !isValidChar(lexeme) && !isValidBool(lexeme))
	{
		return false;
	}
	else
		return true;
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
