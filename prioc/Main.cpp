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
	std::string frag;
	TOKEN token;
};

std::vector<element> table;

/*----------------------------------------------------------------------------------------------------------------------------*/

void addToTable(std::string frag, TOKEN token)
{
	element e;
	e.frag = frag;
	e.token = token;

	table.push_back(e);
}

void printTable()
{
	std::cout << "\n--------------------TABLE--------------------" << std::endl;
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

		std::cout << e.frag + " || " + token << std::endl;
	}
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
			if ((c == '"' || c == '\'') && open == false)
			{
				open = true;
				frag += c;
			}
			else if ((c == '"' || c == '\'') && open == true) // LITERAL
			{
				open = false;
				frag += c;
				addToTable(frag, TOKEN::LITERAL);
				frag = "";
			}
			else if (c == ';') // LITERAL
			{
				if(!frag.empty()) addToTable(frag, TOKEN::LITERAL);
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
		else if (c == ' ' && open == false)
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

	if (!frag.empty()) addToTable(frag, TOKEN::LITERAL);
	
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
		}
		else if (aux == 1)
		{
			if (e.token == TOKEN::IDENTIFIER)
			{
				aux = 2;
			}
		}
		else if (aux == 2)
		{
			if (e.token == TOKEN::OPERATOR)
			{
				aux = 3;
			}
		}
		else if (aux == 3)
		{
			if (e.token == TOKEN::LITERAL)
			{
				aux = 4;
			}
			else if (e.token == TOKEN::SEPARATOR)
			{

			}
		}
		else if (aux == 4)
		{
			if (e.frag == ";" && e.token == TOKEN::SEPARATOR)
			{
				aux = 10;
			}
		}
	}

	if(aux != 10) std::cout << "Erro Sintatico, Estado:  " + std::to_string(aux) << std::endl;
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
		}
	}
	else std::cout << "Falha ao abrir arquivo " + path << std::endl;

	reader.close();
}


int main()
{
	readData("../Main.java");
	printTable();

	return 0;
}