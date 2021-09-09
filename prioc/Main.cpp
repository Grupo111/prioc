/*
* PRIC
* VINÍCIUS REIS
* 
*/

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include "Util.h"


/*----------------------------------------------------------------------------------------------------------------------------*/


std::vector<element> table;


void addToTable(const std::string& frag, TOKEN token, int id = -1)
{
	element e;
	e.lexeme = frag;
	e.token = token;
	e.id = id;

	table.push_back(e);
}

void printTable()
{
	std::cout << "\n\n--------------------TABLE--------------------" << std::endl;
	std::string token;
	for (const auto& e : table)
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

		std::string id;
		e.id >= 0 ? id = std::to_string(e.id) : id = "";
		std::cout << e.lexeme << " || " << token << " || " << id << std::endl;
	}
}

/*----------------------------------------------------------------------------------------------------------------------------*/

void lexical(const std::string& line)
{
	std::string frag;
	bool open = false;
	int currentID = -1;

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
				addToTable(frag, TOKEN::LITERAL, currentID);
				frag = "";
			}
			else if (c == ';') // IDENTIFIER OR LITERAL
			{
				if (!frag.empty())
				{
					if (!table.empty() && table.back().token == TOKEN::KEYWORD)
					{
						addToTable(frag, TOKEN::IDENTIFIER, currentID);
					}
					
					else addToTable(frag, TOKEN::LITERAL, currentID);
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
				currentID = getNextValidID(table);
				addToTable(frag, TOKEN::KEYWORD, currentID);
			}
			else if(!frag.empty()) // IDENTIFIER
			{
				addToTable(frag, TOKEN::IDENTIFIER, currentID);
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
		if (table.back().token == TOKEN::KEYWORD)
		{
			addToTable(frag, TOKEN::IDENTIFIER, currentID);
		}
		
		else if (table.back().token == TOKEN::OPERATOR) 
			addToTable(frag, TOKEN::LITERAL, currentID);
	}


	/*
	*	CHECK IF LITERALS ARE VALID BASED ON REGEX FUNCTIONS
	*/

	for (auto& e : table)
	{
		if (e.token == TOKEN::LITERAL && !isValidLiteral(e.lexeme))
			LOG_LEXICAL_ERROR(e.lexeme);
		
	}
}

void syntactic()
{
	int state = 0;

	for (const auto& e : table)
	{
		if (state == 0)
		{
			if (e.token == TOKEN::KEYWORD)
			{
				state = 1;
			}
			else break;
		}
		else if (state == 1)
		{
			if (e.token == TOKEN::IDENTIFIER)
			{
				state = 2;
			}
			else break;
		}
		else if (state == 2)
		{
			if (e.token == TOKEN::OPERATOR)
			{
				state = 3;
			}
			else if (e.token == TOKEN::SEPARATOR && e.lexeme == ";")
			{
				if (&table.back() != &e)
				{
					state = 0;
				}
				else state = 10;
			}
			else break;
		}
		else if (state == 3)
		{
			if (e.token == TOKEN::LITERAL)
			{
				state = 4;
			}
			else break;
		}
		else if (state == 4)
		{
			if (e.token == TOKEN::SEPARATOR && e.lexeme == ";")
			{
				if (&table.back() != &e)
				{
					state = 0;
				}
				else state = 10;
			}
			else break;
		}
	}

	/*
	*	 CHECKS IF STATE IS DIFFERENT FROM 10 (ok)
	*/

	if (state != 10)
		LOG_SYNTACTIC_ERROR(state);
}

void semantic()
{
	/*
	*	 BUILD VAR TABLE
	*/
	
	std::vector<var> varTable;
	for (auto& e : table)
	{
		if (e.id >= 0 && e.token == TOKEN::KEYWORD)
		{
			var v;
			v.keyword = e.lexeme;
			v.id = e.id;

			varTable.push_back(v);
		}

		else if (e.id >= 0 && e.token == TOKEN::IDENTIFIER)
		{
			for (auto& v : varTable)
			{
				if (v.id == e.id)
				{
					v.identifier = e.lexeme;
					break;
				}
			}
		}

		else if (e.id >= 0 && e.token == TOKEN::LITERAL)
		{
			for (auto& v : varTable)
			{
				if (v.id == e.id)
				{
					v.value = e.lexeme;
					break;
				}
			}
		}
	}

	/*
	*	CHECKS IF VALUE ASSIGNED TO VAR IS VALID
	* 
	*	CHECKS IF IDENTIFIER IS NOT DUPLICATED
	*/

	for (int i = 0; i < varTable.size(); i++)
	{
		if (!varTable[i].value.empty())
		{
			if ((varTable[i].keyword == "String") && !isValidString(varTable[i].value))
				LOG_SEMANTIC_ERROR("String", varTable[i].value);

			else if ((varTable[i].keyword == "boolean") && !isValidBool(varTable[i].value))
				LOG_SEMANTIC_ERROR("boolean", varTable[i].value);

			else if ((varTable[i].keyword == "char") && !isValidChar(varTable[i].value))
				LOG_SEMANTIC_ERROR("char", varTable[i].value);

			else if ((varTable[i].keyword == "int") && !isValidInt(varTable[i].value))
				LOG_SEMANTIC_ERROR("int", varTable[i].value);

			else if ((varTable[i].keyword == "double") && !isValidNumber(varTable[i].value))
				LOG_SEMANTIC_ERROR("double", varTable[i].value);

			else if ((varTable[i].keyword == "float") && !isValidNumber(varTable[i].value))
				LOG_SEMANTIC_ERROR("float", varTable[i].value);
		}

		for (int k = i + 1; k < varTable.size(); k++)
		{
			if (varTable[i].identifier == varTable[k].identifier)
				LOG_SEMANTICVAR_ERROR(varTable[i].identifier);
		}
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

		}

		syntactic();
		semantic();

		printTable();
	}

	else
		LOG_WRONGPATH_ERROR(path);

	reader.close();
}

int main()
{
	readData("../Main.java");

	return 0;
}
