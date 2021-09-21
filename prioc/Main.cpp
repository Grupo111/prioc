/*
* PRIC
* VINÍCIUS REIS
* 
*/


#include "Util.h"


/*----------------------------------------------------------------------------------------------------------------------------*/


std::vector<Element> table;

void addToTable(const std::string& frag, TOKEN token, int id = -1)
{
	Element e;
	e.lexeme = frag;
	e.token = token;
	e.id = id;

	table.push_back(e);
}

void printTable()
{
	LOG("\n\n--------------------TABLE--------------------");
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
		LOG(e.lexeme << " || " << token << " || " << id);
	}
}

/*----------------------------------------------------------------------------------------------------------------------------*/

int lexical(const std::string& line)
{
	std::string frag;
	std::string lastKeyword;
	int currentID = -1;
	bool open = false;

	int errors = 0;

	for (const char& c : line)
	{
		// SEPARATOR
		if (c == '(' || c == ')' || c == ';' || c == '"' || c == '\'' || c == ',')
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
						addToTable(frag, TOKEN::IDENTIFIER, currentID);
					
					else 
						addToTable(frag, TOKEN::LITERAL, currentID);
				}

				frag = "";
				frag += c;				
				addToTable(frag, TOKEN::SEPARATOR);
				frag = "";
			}
			else if (c == ',')
			{
				if (!frag.empty())
				{
					if (!table.empty() && table.back().token == TOKEN::OPERATOR)
						addToTable(frag, TOKEN::LITERAL, currentID);
					else
						addToTable(frag, TOKEN::IDENTIFIER, currentID);
				}

				frag = "";
				frag += c;
				addToTable(frag, TOKEN::SEPARATOR);
				frag = "";

				currentID = getNextValidID(table);
				addToTable(lastKeyword, TOKEN::KEYWORD, currentID);
			}
		}
		// OPERATOR
		else if (c == '=' || c == '+')
		{
			if (!frag.empty())
			{
				int id = getID(table, frag);

				if (id >= 0 && !table.empty() && table.back().token != TOKEN::KEYWORD)
				{
					addToTable(frag, TOKEN::IDENTIFIER, id);
					currentID = id;
				}

				else
					addToTable(frag, TOKEN::IDENTIFIER, currentID);
			}

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
				lastKeyword = frag;
			}
			else if(!frag.empty()) // IDENTIFIER
			{
				int id = getID(table, frag);

				if(id >= 0 && !table.empty() && table.back().token != TOKEN::KEYWORD)
				{
					addToTable(frag, TOKEN::IDENTIFIER, id);
					currentID = id;
				}
				else if (!table.empty() && table.back().token == TOKEN::OPERATOR)
				{
					addToTable(frag, TOKEN::LITERAL, currentID);
				}
				else
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
		if (!table.empty() && table.back().token == TOKEN::KEYWORD)
			addToTable(frag, TOKEN::IDENTIFIER, currentID);
		
		else if (!table.empty() && table.back().token == TOKEN::OPERATOR)
			addToTable(frag, TOKEN::LITERAL, currentID);
	}


	/*
	*	CHECK IF IDENTIFIERS AND LITERALS ARE VALID BASED ON REGEX FUNCTIONS
	*/

	for (auto& e : table)
	{
		if (e.token == TOKEN::LITERAL && !isValidLiteral(e.lexeme))
		{
			LOG_INVALID_LITERAL(e.lexeme);
			errors++;
		}
		else if (e.token == TOKEN::IDENTIFIER && !isValidIdentifier(e.lexeme))
		{
			LOG_INVALID_INDENTIFIER(e.lexeme);
			errors++;
		}
	}

	return errors;
}

int syntactic()
{
	int state = 0;
	int errors = 0;
	std::string lastLexeme;

	for (const auto& e : table)
	{
		if (state == 0)
		{
			if (e.token == TOKEN::KEYWORD)
			{
				state = 1;
			}
			else if (e.token == TOKEN::IDENTIFIER)
			{
				for (auto& var : table)
				{
					if (var.token == TOKEN::KEYWORD && var.id == e.id)
					{
						state = 2;
						break;
					}
				}
			}
			else break;
		}
		else if (state == 1)
		{
			if (e.token == TOKEN::IDENTIFIER)
			{
				for (auto& var : table)
				{
					if (var.token == TOKEN::KEYWORD && var.id == e.id)
					{
						state = 2;
						break;
					}
				}
			}
			else break;
		}
		else if (state == 2)
		{
			if (e.token == TOKEN::OPERATOR)
			{
				state = 3;
			}
			else if (e.token == TOKEN::SEPARATOR)
			{
				if (e.lexeme == ";")
				{
					if (&table.back() != &e)
					{
						state = 0;
					}
					else state = 10;
				}
				else if (e.lexeme == ",")
				{
					state = 0;
				}
			}
			else break;
		}
		else if (state == 3)
		{
			if (e.token == TOKEN::LITERAL)
			{
				state = 4;
			}
			else if (e.token == TOKEN::SEPARATOR && e.lexeme == ",")
			{
				state = 0;
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
			else if (e.token == TOKEN::SEPARATOR && e.lexeme == ",")
			{
				state = 0;
			}
			else break;
		}
		
		lastLexeme = e.lexeme;
	}

	/*
	*	 CHECKS IF STATE IS DIFFERENT FROM 10 (ok)
	*/

	if (state != 10)
	{
		if (state == 4 || state == 2) LOG_MISSING_SEMICOLON(lastLexeme);
		else LOG_SYNTACTIC_ERROR(state, lastLexeme);
		errors++;
	}

	return errors;
}

int semantic()
{
	int errors = 0;

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
			{
				LOG_SEMANTIC_ERROR("String", varTable[i].value);
				errors++;
			}
			else if ((varTable[i].keyword == "boolean") && !isValidBool(varTable[i].value))
			{
				LOG_SEMANTIC_ERROR("boolean", varTable[i].value);
				errors++;
			}
			else if ((varTable[i].keyword == "char") && !isValidChar(varTable[i].value))
			{
				LOG_SEMANTIC_ERROR("char", varTable[i].value);
				errors++;
			}
			else if ((varTable[i].keyword == "int") && !isValidInt(varTable[i].value))
			{
				LOG_SEMANTIC_ERROR("int", varTable[i].value);
				errors++;
			}
			else if ((varTable[i].keyword == "double") && !isValidNumber(varTable[i].value))
			{
				LOG_SEMANTIC_ERROR("double", varTable[i].value);
				errors++;
			}
			else if ((varTable[i].keyword == "float") && !isValidNumber(varTable[i].value))
			{
				LOG_SEMANTIC_ERROR("float", varTable[i].value);
				errors++;
			}
		}

		for (int k = i + 1; k < varTable.size(); k++)
		{
			if (varTable[i].identifier == varTable[k].identifier)
			{ 
				LOG_SEMANTICVAR_ERROR(varTable[i].identifier);
				errors++;
			}
		}
	}

	return errors;
}

void generateCode()
{
	std::ofstream outFile("../Main.cpp");

	bool newLine = true;
	bool multipleDeclaration = false;

	for (auto& e : table)
	{
		// SPACE
		if (!newLine && e.token != TOKEN::SEPARATOR)	
			outFile << " ";

		// MULTIPLE DECLARATION (, ... = ...)
		if (e.token == TOKEN::SEPARATOR && e.lexeme == ",")
			multipleDeclaration = true;

		if (e.token == TOKEN::KEYWORD)
		{
			// C++ STRING
			if (e.lexeme == "String")
				outFile << "std::string";

			// C++ BOOLEAN
			else if (e.lexeme == "boolean")
				outFile << "bool";

			// NULL KEYWORD FOR SECOND DECLARATION
			else if (multipleDeclaration)
				multipleDeclaration = false;

			else
				outFile << e.lexeme;
		}
		else
			outFile << e.lexeme;
		
		newLine = false;

		// LINE BREAK
		if (e.lexeme == ";")
		{
			outFile << "\n";
			newLine = true;
		}
	}

	outFile.close();
}


/*----------------------------------------------------------------------------------------------------------------------------*/


void compiler(const std::string sourceCode)
{
	int errors = 0;

	errors += lexical(sourceCode);
	errors += syntactic();
	errors += semantic();

	if (errors == 0)
	{
		LOG("Compilação sucedida!");
		generateCode();
	}
	else
		LOG("\n\nFalha na compilação -- ERROS: " << errors);

	//printTable();
}

void readData(const std::string& path)
{
	std::fstream reader;

	reader.open(path);

	if (reader)
	{
		std::string sourceCode;
		std::string line;

		while (std::getline(reader, line))
		{
			sourceCode += line;
		}

		compiler(sourceCode);
	}
	else
		LOG_WRONGPATH_ERROR(path);

	reader.close();
}

int main()
{
	// HABILITA ACENTUAÇÃO NO CONSOLE
	setlocale(LC_ALL, "pt_BR.UTF-8"); 

	readData("../Main.java");

	return 0;
}
