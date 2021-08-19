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

/*-----------------------------------------------------------------------------------------------*/

void addToTable(std::string frag, TOKEN token)
{
	element e;
	e.frag = frag;
	e.token = token;

	table.push_back(e);
}

void printTable()
{
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
			}
			else if ((c == '"' || c == '\'') && open == true) // LITERAL
			{
				open = false;
				addToTable(frag, TOKEN::LITERAL);
			}
			else if (c == ';') // LITERAL
			{
				if(!frag.empty()) addToTable(frag, TOKEN::LITERAL);
				frag = "";
			}

			frag = c;
			addToTable(frag, TOKEN::SEPARATOR);
			frag = "";
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
			if (frag == "int" || frag == "String" || frag == "boolean" || frag == "float" || frag == "double") // KEYWORD
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
}

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