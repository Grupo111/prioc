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

int lexical(const std::string& sourceCode)
{
	std::string frag;
	std::string lastKeyword;
	int currentID = -1;
	bool openQuotes = false;
	bool openParameter = false;

	int errors = 0;

	for (const char& c : sourceCode)
	{
		// SEPARATOR
		if (c == '(' || c == ')' || c == ';' || c == '"' || c == '\'' || c == ',')
		{
			if ((c == '"' || c == '\'') && !openQuotes)
			{
				openQuotes = true;
				frag += c;
			}
			else if ((c == '"' || c == '\'') && openQuotes) // LITERAL
			{
				openQuotes = false;
				frag += c;
				if (table.back().lexeme == "=")
					addToTable(frag, TOKEN::LITERAL, currentID);
				else
					addToTable(frag, TOKEN::LITERAL);
				frag = "";
			}
			else if (c == ';' && !openQuotes) // IDENTIFIER OR LITERAL
			{
				if (!frag.empty())
				{
					int id = getID(table, frag);

					if (!table.empty() && table.back().token == TOKEN::KEYWORD)
						addToTable(frag, TOKEN::IDENTIFIER, currentID);

					else if(id >= 0)
						addToTable(frag, TOKEN::IDENTIFIER, id);

					else
						addToTable(frag, TOKEN::LITERAL, currentID);
				}

				frag = "";
				frag += c;
				addToTable(frag, TOKEN::SEPARATOR);
				frag = "";
			}
			else if (c == ',' && !openQuotes)
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
			else if (c == '(' && !openQuotes)
			{
				openParameter = true;

				// KEYWORD
				if (frag == "System.out.println" || frag == "System.out.print")
				{
					addToTable(frag, TOKEN::KEYWORD);
					frag = "";
					frag += c;
					
					addToTable(frag, TOKEN::SEPARATOR);
					frag = "";
				}
				else
				{
					frag += c;
					addToTable(frag, TOKEN::SEPARATOR);
					frag = "";
				}
			}
			else if (c == ')' && !openQuotes)
			{
				openParameter = false;

				if (!frag.empty())
				{
					int id = getID(table, frag);
					
					if (id >= 0)
						addToTable(frag, TOKEN::IDENTIFIER, id);

					else
						addToTable(frag, TOKEN::LITERAL);
				}
					
				frag = "";
				frag += c;
				addToTable(frag, TOKEN::SEPARATOR);
				frag = "";
			}
		}
		// OPERATOR
		else if (c == '=' || c == '+' || c == '-' || c == '*' || c == '/')
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
					addToTable(frag, TOKEN::LITERAL, currentID);
			}

			frag = c;
			addToTable(frag, TOKEN::OPERATOR);
			frag = "";
		}
		// SPACE
		else if (c == ' ' && !openQuotes && !openParameter)
		{
			if (frag == "int" ||  frag == "Integer" ||
				frag == "String" || frag == "char" || 
				frag == "boolean" || frag == "Boolean" || 
				frag == "float" || frag == "Float" ||
				frag == "double" || frag == "Double") // KEYWORD
			{
				currentID = getNextValidID(table);
				addToTable(frag, TOKEN::KEYWORD, currentID);
				lastKeyword = frag;
			}
			else if (frag == "System.out.println" || frag == "System.out.print")
			{
				addToTable(frag, TOKEN::KEYWORD);
			}
			else if(!frag.empty()) // IDENTIFIER OR LITERAL
			{
				int id = getID(table, frag);

				if(id >= 0 && !table.empty() && table.back().token != TOKEN::KEYWORD)
				{
					addToTable(frag, TOKEN::IDENTIFIER, id);
					//currentID = id;
				}
				else if (!table.empty() && table.back().token == TOKEN::OPERATOR)
				{
					addToTable(frag, TOKEN::LITERAL, currentID);
				}
				else if(!table.empty() && table.back().token != TOKEN::KEYWORD)
					addToTable(frag, TOKEN::IDENTIFIER, id);
				
				else
					addToTable(frag, TOKEN::IDENTIFIER, currentID);

			}

			frag = "";
		}
		//
		else
		{
			if(openQuotes)
				frag += c;
			else if(c != ' ')
				frag += c;
		}
	}

	if (!frag.empty())
	{
		if (!table.empty() && table.back().token == TOKEN::KEYWORD)
			addToTable(frag, TOKEN::IDENTIFIER, currentID);
		
		else if (!table.empty() && table.back().lexeme == "=")
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
	bool open = false;
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
			else if (e.token == TOKEN::SEPARATOR && e.lexeme == "(")
			{
				open = true;
				state = 3;
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
			else if (e.token == TOKEN::IDENTIFIER)
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
			if (e.token == TOKEN::SEPARATOR)
			{
				if (e.lexeme == ";" && !open)
				{
					if (&table.back() != &e)
					{
						state = 0;
					}
					else state = 10;
				}
				else if (e.lexeme == "," && !open)
				{
					state = 0;
				}
				else if (e.lexeme == ")")
				{
					open = false;
					state = 4;
				}
			}
			else if (e.token == TOKEN::OPERATOR && (e.lexeme == "+" || e.lexeme == "-" || e.lexeme == "/" || e.lexeme == "*"))
			{
				state = 3;
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
		LOG_SYNTACTIC_ERROR(state, lastLexeme);
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
	
	// TODO: PEGAR DECLARACOES MATEMATICAS
	std::vector<var> varTable;
	bool openParameter = false;
	for (int i = 0; i < table.size(); i++)
	{
		// VARS NOT INIT
		if (table[i].token == TOKEN::KEYWORD && isTypeKeyword(table[i].lexeme) &&
			!(table[i + 2].token == TOKEN::OPERATOR && table[i + 2].lexeme == "="))
		{
			var var;
			var.id = table[i].id;
			var.keyword = table[i].lexeme;
			var.identifier = table[i + 1].lexeme;
			
			varTable.push_back(var);
		}

		// NORMAL VAR
		if (table[i].token == TOKEN::OPERATOR && table[i].lexeme == "=")
		{
			int id = table[i - 1].id;
			std::string identifier = table[i - 1].lexeme;
			std::string keyword, value;
			bool pointsToAnother = false;
			
			// VALUE
			if (table[i + 1].token == TOKEN::LITERAL)
			{
				value = table[i + 1].lexeme;
			}
			else if (table[i + 1].token == TOKEN::IDENTIFIER)
			{
				pointsToAnother = true;
				value = getValue(table, table[i + 1].id);
			}

			// TYPE
			if (table[i - 2].token == TOKEN::KEYWORD && table[i - 2].id == id)
			{
				keyword = table[i - 2].lexeme;

				var var;
				var.id = table[i - 2].id;
				var.identifier = identifier;
				var.keyword = keyword;
				var.pointsToAnother = pointsToAnother;
				var.value = value;
				
				varTable.push_back(var);
			}
			else
			{
				for (auto& v : varTable)
				{
					if (v.id == table[i - 1].id)
					{
						v.value = value;
						v.pointsToAnother = pointsToAnother;
					}
				}
			}
		}

		// SET VAR VALUE REQUIRED
		if (table[i].token == TOKEN::SEPARATOR && table[i].lexeme == "(" && !openParameter)
			openParameter = true;

		if (table[i].token == TOKEN::SEPARATOR && table[i].lexeme == ")" && openParameter)
			openParameter = false;

		if (table[i].token == TOKEN::IDENTIFIER && openParameter)
		{
			for (auto& v : varTable)
			{
				if (v.identifier == table[i].lexeme)
				{
					v.valueRequired = true;
				}
			}
		}
		
	}


	/*
	*	CHECKS IF VALUE ASSIGNED TO VAR IS VALID
	* 
	*	CHECKS IF VAR IS UNINITIALIZED
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
			else if (((varTable[i].keyword == "boolean") || (varTable[i].keyword == "Boolean")) && !isValidBool(varTable[i].value))
			{
				LOG_SEMANTIC_ERROR("boolean", varTable[i].value);
				errors++;
			}
			else if ((varTable[i].keyword == "char") && !isValidChar(varTable[i].value))
			{
				LOG_SEMANTIC_ERROR("char", varTable[i].value);
				errors++;
			}
			else if (((varTable[i].keyword == "int") || (varTable[i].keyword == "Integer")) && !isValidInt(varTable[i].value))
			{
				LOG_SEMANTIC_ERROR("int", varTable[i].value);
				errors++;
			}
			else if (((varTable[i].keyword == "double") || (varTable[i].keyword == "Double")) && !isValidNumber(varTable[i].value))
			{
				LOG_SEMANTIC_ERROR("double", varTable[i].value);
				errors++;
			}
			else if (((varTable[i].keyword == "float") || (varTable[i].keyword == "Float")) && !isValidNumber(varTable[i].value))
			{
				LOG_SEMANTIC_ERROR("float", varTable[i].value);
				errors++;
			}
		}
		else
		{
			if (varTable[i].valueRequired)
			{
				LOG_UNINITILIAZED_ERROR(varTable[i].identifier);
				errors++;
			}

			else
				LOG_UNINITILIAZED_WARN(varTable[i].identifier);
		}

		for (int k = i + 1; k < varTable.size(); k++)
		{
			if (varTable[i].identifier == varTable[k].identifier)
			{ 
				LOG_DUPLICATED_ERROR(varTable[i].identifier);
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
	bool outParameter = false;
	bool outEndLine = false;

	for (auto& e : table)
	{
		// SPACE
		if (!newLine && e.token != TOKEN::SEPARATOR)	
			outFile << " ";

		if (e.token == TOKEN::SEPARATOR)
		{
			// MULTIPLE DECLARATION (, ... = ...)
			if (e.lexeme == ",")
			{
				multipleDeclaration = true;
				outFile << ",";
			}
				
			else if (e.lexeme == "(" && outParameter)
				outFile << " <<";

			else if (e.lexeme == ")" && outParameter)
			{
				if (outEndLine)
				{
					outEndLine = false;
					outFile << " << std::endl";
				}
				
				outParameter = false;
			}

			else 
				outFile << e.lexeme;
		}
		else if (e.token == TOKEN::KEYWORD)
		{
			// NULL KEYWORD FOR SECOND DECLARATION
			if (multipleDeclaration)
				multipleDeclaration = false;

			// C++ TYPES
			else if (e.lexeme == "String")
				outFile << "std::string";

			else if (e.lexeme == "Integer")
				outFile << "int";

			else if (e.lexeme == "boolean" || e.lexeme == "Boolean")
				outFile << "bool";

			else if (e.lexeme == "Double")
				outFile << "double";

			else if (e.lexeme == "Float")
				outFile << "float";

			else if (e.lexeme == "System.out.println")
			{
				outFile << "std::cout";
				outParameter = true;
				outEndLine = true;
			} 

			else if (e.lexeme == "System.out.print")
			{
				outFile << "std::cout";
				outParameter = true;
			}


			else
				outFile << e.lexeme;
		}
		else if (e.token == TOKEN::OPERATOR)
		{
			// CHANGE OPERATOR + TO << WHEN OUTPUTTING MULTIPLE PARAMETERS
			if (e.lexeme == "+" && outParameter)
				outFile << "<<";

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
		LOG("\nCompilação sucedida!");
		generateCode();
	}
	else
		LOG("\n\nFalha na compilação -- ERROS: " << errors);

	printTable();
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
