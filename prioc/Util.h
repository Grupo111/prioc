#pragma once
#include <stdlib.h>
#include <clocale>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <regex>

//MACRO PRINTS
#define LOG(x) std::cout << x << std::endl
#define LOG_WRONGPATH_ERROR(path) std::cout << "Falha ao abrir arquivo " << path << std::endl

#define LOG_INVALID_INDENTIFIER(identifier) std::cout << "--> Indenticador invalido: " << identifier << std::endl
#define LOG_INVALID_LITERAL(value) std::cout << "--> Valor invalido: " << value << std::endl

#define LOG_SYNTACTIC_ERROR(state, lexeme) std::cout << "--> Erro Sintatico, Estado: " << state << " em: " << lexeme << std::endl
#define LOG_NOTCLOSED_BRACKET(state) std::cout << "--> Erro: " << state << ": \"}\" Experado" << std::endl
#define LOG_NOTCLOSED_PAREN(state) std::cout << "--> Erro: " << state << ": \")\" Experado" << std::endl

#define LOG_SEMANTIC_ERROR(keyword, value) std::cout << "--> Erro: " << keyword << " Invalido:  " << value << std::endl
#define LOG_DUPLICATED_ERROR(value) std::cout << "--> Erro: Identificador duplicado: " << value << std::endl
#define LOG_IDENTIFIERNOTFOUND_ERROR(identifier) std::cout << "--> Erro: Identificador " << identifier << " nao encontrado" << std::endl;
#define LOG_WRONGELSEUSAGE_ERROR() LOG("--> Erro: else utilizado sem if");
#define LOG_UNINITILIAZED_WARN(identifier) std::cout << "--> Aviso: variavel nao inicializada: " << identifier << std::endl
#define LOG_UNINITILIAZED_ERROR(identifier) std::cout << "--> Erro: variavel nao inicializada " << identifier << " dentro de parametros" << std::endl

/*----------------------------------------------------------------------------------------------------------------------------*/

enum class TOKEN
{
	IDENTIFIER,		// names the programmer chooses (x, color, UP)
	KEYWORD,		// names already in the programming language (if, while, return)
	SEPARATOR,		// punctuation characters and paired-delimiters (}, (, ;)
	OPERATOR,		// symbols that operate on arguments and produce results (+, <, =)
	LITERAL,		// numeric, logical, textual, reference literals (true, 6.02e23, "music")
	COMMENT,		// line, block (/* Retrieves user data */)
};

struct Element
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
	bool valueRequired = false;
	bool pointsToAnother = false;
	bool userInput = false;
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

bool isValidIdentifier(const std::string& lexeme)
{
	std::regex re("^([a-zA-Z_$][a-zA-Z\\d_$]*)$");
	return std::regex_match(lexeme, re);
}

bool isTypeKeyword(const std::string& lexeme)
{
	if (lexeme == "int" || lexeme == "Integer" ||
		lexeme == "String" || lexeme == "char" ||
		lexeme == "boolean" || lexeme == "Boolean" ||
		lexeme == "float" || lexeme == "Float" ||
		lexeme == "double" || lexeme == "Double")
	{
		return true;
	}

	return false;
}

int getNextValidID(const std::vector<Element>& table)
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

int getID(const std::vector<Element>& table, const std::string& frag)
{
	int id = -1;
	for (auto& e : table)
	{
		if (e.token == TOKEN::IDENTIFIER && e.lexeme == frag)
			id = e.id;
	}

	return id;
}

std::string getValue(const std::vector<Element>& table, int ID)
{
	std::string value;
	for (auto& e : table)
	{
		if (e.token == TOKEN::LITERAL && e.id == ID)
			value = e.lexeme;
	}

	return value;
}
