/*
* PRIC
* VINÍCIUS REIS
* 
*/

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <regex>

enum class TOKEN
{
	IDENTIFIER,		// names the programmer chooses (x, color, UP)
	KEYWORD,		// names already in the programming language (if, while, return)
	SEPARATOR,		// punctuation characters and paired-delimiters (}, (, ;)
	OPERATOR,		// symbols that operate on arguments and produce results (+, <, =)
	LITERAL,		// numeric, logical, textual, reference literals (true, 6.02e23, "music")
	COMMENT			//  line, block (/* Retrieves user data */)
};

struct element
{
	std::string lexeme;
	TOKEN token;
};

std::vector<element> table;


/*----------------------------------------------------------------------------------------------------------------------------*/


void addToTable(const std::string& frag, TOKEN token)
{
	element e;
	e.lexeme = frag;
	e.token = token;

	table.push_back(e);
}

void printTable()
{
	std::cout << "\n\n--------------------TABLE--------------------" << std::endl;
	std::string token;
	for (auto& e : table)
	{
		switch (e.token)
		{
		case TOKEN::IDENTIFIER:
			token = "Identifier";
			break;
		case TOKEN::KEYWORD:
			token = "Keyword";
			break;
		case TOKEN::SEPARATOR:
			token = "Separator";
			break;
		case TOKEN::OPERATOR:
			token = "Operator";
			break;
		case TOKEN::LITERAL:
			token = "Literal";
			break;
		case TOKEN::COMMENT:
			token = "Comment";
			break;
		}

		std::cout << e.lexeme + " || " + token << std::endl;
	}
}

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


/*----------------------------------------------------------------------------------------------------------------------------*/

void lexical(const std::string& line)
{
	std::string frag;
	bool open = false;

	for (const char& c : line)
	{
		// SEPARATOR
		if (c == '(' || c == ')' || c == ';' || c == '"' || c == '\'')
		{
			if ((c == '"' || c == '\'') && !open)
			{
				open = true;
				frag += c;
			}
			else if ((c == '"' || c == '\'') && open) // LITERAL
			{
				open = false;
				frag += c;
				addToTable(frag, TOKEN::LITERAL);
				frag = "";
			}
			else if (c == ';') // IDENTIFIER OR LITERAL
			{
				if (!frag.empty())
				{
					if (!table.empty() && table.back().token == TOKEN::KEYWORD) addToTable(frag, TOKEN::IDENTIFIER);
					else addToTable(frag, TOKEN::LITERAL);
				}

				frag = "";
				frag += c;				
				addToTable(frag, TOKEN::SEPARATOR);
				frag = "";
			}
		}
		// OPERATOR
		else if (c == '=' || c == '+')
		{
			frag = c;
			addToTable(frag, TOKEN::OPERATOR);
			frag = "";
		}
		// SPACE
		else if (c == ' ' && !open)
		{
			if (frag == "int" || frag == "String" || frag == "char" || frag == "boolean" || frag == "float" || frag == "double") // KEYWORD
			{
				addToTable(frag, TOKEN::KEYWORD);
			}
			else if(!frag.empty()) // IDENTIFIER
			{
				addToTable(frag, TOKEN::IDENTIFIER);
			}

			frag = "";
		}
		//
		else
		{
			frag += c;
		}
	}

	if (!frag.empty())
	{
		if(table.back().token == TOKEN::KEYWORD) addToTable(frag, TOKEN::IDENTIFIER);
		else if (table.back().token == TOKEN::OPERATOR) addToTable(frag, TOKEN::LITERAL);
	}

	for (auto& e : table)
	{
		if (e.token == TOKEN::LITERAL)
		{
			if (!isValidNumber(e.lexeme) && !isValidString(e.lexeme) && !isValidChar(e.lexeme) && !isValidBool(e.lexeme))
			{
				std::cout << "\n-->Erro Lexico, Lexema:  " + e.lexeme << std::endl;
			}
		}
	}
}

void syntactic()
{
	int aux = 0;
	for (auto& e : table)
	{
		if (aux == 0)
		{
			if (e.token == TOKEN::KEYWORD)
			{
				aux = 1;
			}
			else break;
		}
		else if (aux == 1)
		{
			if (e.token == TOKEN::IDENTIFIER)
			{
				aux = 2;
			}
			else break;
		}
		else if (aux == 2)
		{
			if (e.token == TOKEN::OPERATOR)
			{
				aux = 3;
			}
			else if (e.token == TOKEN::SEPARATOR && e.lexeme == ";")
			{
				aux = 10;
			}
			else break;
		}
		else if (aux == 3)
		{
			if (e.token == TOKEN::LITERAL)
			{
				aux = 4;
			}
			else break;
		}
		else if (aux == 4)
		{
			if (e.token == TOKEN::SEPARATOR && e.lexeme == ";")
			{
				aux = 10;
			}
			else break;
		}
	}

	if(aux != 10) std::cout << "\n-->Erro Sintatico, Estado:  " + std::to_string(aux) << std::endl;
}

void semantic()
{
	std::string keyword, literal;
	for (auto& e : table)
	{
		if (e.token == TOKEN::KEYWORD) keyword = e.lexeme;
		else if (e.token == TOKEN::LITERAL) literal = e.lexeme;
	}

	if (!literal.empty())
	{
		if ((keyword == "String") && !isValidString(literal))	    std::cout << "\n-->Erro Semantico, String Invalida:  " + literal << std::endl;
		else if ((keyword == "boolean") && !isValidBool(literal))   std::cout << "\n-->Erro Semantico, boolean Invalido:  " + literal << std::endl;
		else if ((keyword == "char") && !isValidChar(literal))      std::cout << "\n-->Erro Semantico, char Invalido:  " + literal << std::endl;
		else if ((keyword == "int") && !isValidInt(literal))	    std::cout << "\n-->Erro Semantico, int Invalido:  " + literal << std::endl;
		else if ((keyword == "double") && !isValidNumber(literal))  std::cout << "\n-->Erro Semantico, double Invalido:  " + literal << std::endl;
		else if ((keyword == "float") && !isValidNumber(literal))   std::cout << "\n-->Erro Semantico, float Invalido:  " + literal << std::endl;
	}
}


/*----------------------------------------------------------------------------------------------------------------------------*/


void readData(const std::string& path)
{
	std::fstream reader;

	reader.open(path);

	if (reader)
	{
		std::string line;

		while (std::getline(reader, line))
		{
			lexical(line);
			syntactic();
			semantic();
			printTable();
		}
	}
	else std::cout << "Falha ao abrir arquivo " + path << std::endl;

	reader.close();
}

int main()
{
	readData("../Main.java");
	
	return 0;
}
