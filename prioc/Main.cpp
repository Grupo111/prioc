/*
* PRIOC
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

	for (int i = 0; i < sourceCode.size(); i++)
	{
		// SEPARATOR
		if (sourceCode[i] == '(' || sourceCode[i] == ')' || sourceCode[i] == ';' || sourceCode[i] == '"' || sourceCode[i] == '\'' || sourceCode[i] == ',' || sourceCode[i] == '{' || sourceCode[i] == '}')
		{
			if (frag == "System.console" || frag == "System.console().readLine" ||
				frag == "System.console(" || frag == "System.console().readLine(")
			{
				frag += sourceCode[i];
			}
			else if ((sourceCode[i] == '"' || sourceCode[i] == '\'') && !openQuotes)
			{
				openQuotes = true;
				frag += sourceCode[i];
			}
			else if ((sourceCode[i] == '"' || sourceCode[i] == '\'') && openQuotes) // LITERAL
			{
				openQuotes = false;
				frag += sourceCode[i];
				if (table.back().lexeme == "=")
					addToTable(frag, TOKEN::LITERAL, currentID);
				else
					addToTable(frag, TOKEN::LITERAL);
				frag = "";
			}
			else if (sourceCode[i] == ';' && !openQuotes) // IDENTIFIER OR LITERAL
			{
				if (!frag.empty())
				{
					int id = getID(table, frag);

					if (frag == "System.console().readLine()")
						addToTable(frag, TOKEN::KEYWORD, currentID);

					else if (!table.empty() && table.back().token == TOKEN::KEYWORD)
						addToTable(frag, TOKEN::IDENTIFIER, currentID);

					else if (id >= 0)
						addToTable(frag, TOKEN::IDENTIFIER, id);

					else if (table.back().lexeme == "=")
						addToTable(frag, TOKEN::LITERAL, currentID);

					else if (isValidLiteral(frag))
						addToTable(frag, TOKEN::LITERAL);

					else
						addToTable(frag, TOKEN::IDENTIFIER, id);

				}

				frag = "";
				frag += sourceCode[i];
				addToTable(frag, TOKEN::SEPARATOR);
				frag = "";
			}
			else if (sourceCode[i] == ',' && !openQuotes)
			{
				if (!frag.empty())
				{
					if (!table.empty() && table.back().token == TOKEN::OPERATOR)
						addToTable(frag, TOKEN::LITERAL, currentID);
					else
						addToTable(frag, TOKEN::IDENTIFIER, currentID);
				}

				frag = "";
				frag += sourceCode[i];
				addToTable(frag, TOKEN::SEPARATOR);
				frag = "";

				currentID = getNextValidID(table);
				addToTable(lastKeyword, TOKEN::KEYWORD, currentID);
			}
			else if (sourceCode[i] == '(' && !openQuotes)
			{
				openParameter = true;

				// KEYWORD
				if (frag == "System.out.println" || frag == "System.out.print" || frag == "if" || frag == "while")
				{
					addToTable(frag, TOKEN::KEYWORD);
					frag = "";
					frag += sourceCode[i];

					addToTable(frag, TOKEN::SEPARATOR);
					frag = "";
				}
				else
				{
					frag += sourceCode[i];
					addToTable(frag, TOKEN::SEPARATOR);
					frag = "";
				}
			}
			else if (sourceCode[i] == ')' && !openQuotes)
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
				frag += sourceCode[i];
				addToTable(frag, TOKEN::SEPARATOR);
				frag = "";
			}
			else if (sourceCode[i] == '{' || sourceCode[i] == '}')
			{
				if (frag == "if" || frag == "else")
				{
					addToTable(frag, TOKEN::KEYWORD);
				}

				frag = "";
				frag += sourceCode[i];
				addToTable(frag, TOKEN::SEPARATOR);
				frag = "";
			}
		}
		// OPERATOR
		else if ((sourceCode[i] == '=' || sourceCode[i] == '+' || sourceCode[i] == '-' || sourceCode[i] == '*' ||
			sourceCode[i] == '/' || sourceCode[i] == '>' || sourceCode[i] == '<' || sourceCode[i] == '!') && !openQuotes)
		{
			if (!frag.empty())
			{
				int id = getID(table, frag);

				if (!table.empty() && table.back().token == TOKEN::KEYWORD)
					addToTable(frag, TOKEN::IDENTIFIER, currentID);

				else if (id >= 0)
					addToTable(frag, TOKEN::IDENTIFIER, id);

				else if (table.back().lexeme == "=")
					addToTable(frag, TOKEN::LITERAL, currentID);

				else if (isValidLiteral(frag))
					addToTable(frag, TOKEN::LITERAL);

				else
					addToTable(frag, TOKEN::IDENTIFIER, id);
			}

			// ==  <= >= ++ --
			if (sourceCode[i + 1] == '=' || (sourceCode[i + 1] == '+' && sourceCode[i] == '+') || 
											(sourceCode[i + 1] == '-' && sourceCode[i] == '-'))
			{
				frag = sourceCode[i];
				frag += sourceCode[i + 1];
				addToTable(frag, TOKEN::OPERATOR);
				frag = "";

				i += 1;
			}
			else
			{
				frag = sourceCode[i];
				addToTable(frag, TOKEN::OPERATOR);
				frag = "";
			}
		}
		// SPACE
		else if (sourceCode[i] == ' ' && !openQuotes && !openParameter)
		{
			if (frag == "int" || frag == "Integer" ||
				frag == "String" || frag == "char" ||
				frag == "boolean" || frag == "Boolean" ||
				frag == "float" || frag == "Float" ||
				frag == "double" || frag == "Double") // KEYWORD
			{
				currentID = getNextValidID(table);
				addToTable(frag, TOKEN::KEYWORD, currentID);
				lastKeyword = frag;
			}
			else if (frag == "System.out.println" || frag == "System.out.print" || frag == "System.console().readLine()")
			{
				addToTable(frag, TOKEN::KEYWORD, currentID);
			}
			else if (frag == "if" || frag == "else" || frag == "while")
			{
				addToTable(frag, TOKEN::KEYWORD);
			}
			else if (!frag.empty()) // IDENTIFIER OR LITERAL
			{
				int id = getID(table, frag);

				if (!table.empty() && table.back().token == TOKEN::KEYWORD)
					addToTable(frag, TOKEN::IDENTIFIER, currentID);

				else if (id >= 0)
					addToTable(frag, TOKEN::IDENTIFIER, id);

				else if (table.back().lexeme == "=")
					addToTable(frag, TOKEN::LITERAL, currentID);

				else if (isValidLiteral(frag))
					addToTable(frag, TOKEN::LITERAL);

				else
					addToTable(frag, TOKEN::IDENTIFIER, id);

			}

			frag = "";
		}
		//
		else
		{
			if (openQuotes)
				frag += sourceCode[i];
			else if (sourceCode[i] != ' ')
				frag += sourceCode[i];
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
	*	CHECA SE LITERALS E IDENTIFIERS SÃO VALIDOS
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
	bool openBracket = false;
	std::string lastLexeme;

	for (const auto& e : table)
	{
		if (state == 0)
		{
			if (e.token == TOKEN::KEYWORD)
			{
				if (e.lexeme != "else" || !openBracket)
				{
					state = 1;
				}
			}
			else if (e.token == TOKEN::IDENTIFIER)
			{
				state = 2;
			}
			else if (openBracket && e.token == TOKEN::SEPARATOR && e.lexeme == "}")
			{
				openBracket = false;
				if (&table.back() != &e)
				{
					state = 0;
				}
				else
					state = 10;
			}
			else break;
		}
		else if (state == 1)
		{
			if (e.token == TOKEN::IDENTIFIER)
			{
				state = 2;
			}
			else if (e.token == TOKEN::SEPARATOR && e.lexeme == "(")
			{
				open = true;
				state = 3;
			}
			else if (e.token == TOKEN::SEPARATOR && e.lexeme == "{")
			{
				openBracket = true;
				state = 0;
			}
			else break;
		}
		else if (state == 2)
		{
			if (e.token == TOKEN::OPERATOR)
			{
				if (e.lexeme == "++" || e.lexeme == "--")
					state = 4;

				else
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
			else if (e.token == TOKEN::KEYWORD && e.lexeme == "System.console().readLine()")
			{
				state = 4;
			}
			else if (e.token == TOKEN::OPERATOR && e.lexeme == "!")
			{
				state = 3;
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
					if (&table.back() != &e || openBracket)
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
				else if (e.lexeme == "{")
				{
					openBracket = true;
					state = 0;
				}
			}
			else if (e.token == TOKEN::OPERATOR &&
				(e.lexeme == "+" || e.lexeme == "-" || e.lexeme == "/" || e.lexeme == "*" || e.lexeme == "==" ||
					e.lexeme == ">=" || e.lexeme == "<=" || e.lexeme == ">" || e.lexeme == "<") || e.lexeme == "!=")
			{
				state = 3;
			}
			else break;
		}

		lastLexeme = e.lexeme;
	}

	/*
	*	 CHECA SE ESTADO É DIFERENTE DO ESTADO OK (10)
	*/

	if (state != 10)
	{
		if (openBracket)
		{
			LOG_NOTCLOSED_BRACKET(state);
		}
		else if (open)
		{
			LOG_NOTCLOSED_PAREN(lastLexeme);
		}
		else
			LOG_SYNTACTIC_ERROR(state, lastLexeme);


		errors++;
	}

	return errors;
}

int semantic()
{
	int errors = 0;

	/*
	*    CONSTROI TABELA DE VAR
	* 
	*	 CHECA SE VALOR ATRIBUIDO A VAR É VALIDO
	* 
	*	 CHECA SE VAR FOI INICIALIZADA
	*/
	std::vector<var> varTable;
	bool openParameter = false;
	for (int i = 0; i < table.size(); i++)
	{
		// VARS NÃO INICIALIZADAS
		if (table[i].token == TOKEN::KEYWORD && isTypeKeyword(table[i].lexeme) &&
			!(table[i + 2].token == TOKEN::OPERATOR && table[i + 2].lexeme == "="))
		{
			var var;
			var.id = table[i].id;
			var.keyword = table[i].lexeme;
			var.identifier = table[i + 1].lexeme;

			varTable.push_back(var);
		}

		//  VARS NORMAL
		if (table[i].token == TOKEN::OPERATOR)
		{
			if (table[i].lexeme == "=")
			{
				int id = table[i - 1].id;
				std::string identifier = table[i - 1].lexeme;
				std::string keyword, value;
				bool pointsToAnother = false;
				bool userInput = false;

				// VALOR
				if (table[i + 1].token == TOKEN::LITERAL)
				{
					value = table[i + 1].lexeme;
				}
				else if (table[i + 1].token == TOKEN::IDENTIFIER)
				{
					pointsToAnother = true;
					value = getValue(table, table[i + 1].id);

					if (value.empty())
					{
						errors++;
						LOG_UNINITILIAZEDATTRIB_ERROR(table[i + 1].lexeme, identifier);
					}
				}
				else if (table[i + 1].token == TOKEN::KEYWORD && table[i + 1].lexeme == "System.console().readLine()")
				{
					userInput = true;
				}

				// TIPO
				if (table[i - 2].token == TOKEN::KEYWORD && table[i - 2].id == id)
				{
					keyword = table[i - 2].lexeme;

					var var;
					var.id = table[i - 2].id;
					var.identifier = identifier;
					var.keyword = keyword;
					var.pointsToAnother = pointsToAnother;
					var.userInput = userInput;
					var.value = value;

					varTable.push_back(var);

					if (!isValueAssignedValid(var))
					{
						errors++;
						LOG_SEMANTIC_ERROR(var.keyword, var.value);
					}
				}
				else
				{
					for (auto& v : varTable)
					{
						if (v.id == table[i - 1].id)
						{
							v.value = value;
							v.userInput = userInput;
							v.pointsToAnother = pointsToAnother;

							if (!isValueAssignedValid(v))
							{
								errors++;
								LOG_SEMANTIC_ERROR(v.keyword, v.value);
							}
						}
					}
				}
			}
			else if (table[i].lexeme == "+" || table[i].lexeme == "-" || table[i].lexeme == ">" || table[i].lexeme == ">=" ||
					 table[i].lexeme == "/" || table[i].lexeme == "*" || table[i].lexeme == "<" || table[i].lexeme == "<=" ||
				     table[i].lexeme == "==")
			{
				std::string type, type2;

				if (table[i - 1].token == TOKEN::IDENTIFIER)
					type = getType(table, table[i - 1].id);
				else if (table[i - 1].token == TOKEN::LITERAL)
					type = getLiteralType(table[i - 1].lexeme);
				
				if (table[i + 1].token == TOKEN::IDENTIFIER)
					type2 = getType(table, table[i + 1].id);
				else if (table[i + 1].token == TOKEN::LITERAL)
					type2 = getLiteralType(table[i + 1].lexeme);
				

				if ((table[i].lexeme == "-" || table[i].lexeme == ">" || table[i].lexeme == ">=" || table[i].lexeme == "/" ||
					table[i].lexeme == "*" || table[i].lexeme == "<" || table[i].lexeme == "<=") && (type == "String" || type2 == "String"))
				{
					errors++;
					LOG_BADOPERAND_ERROR(table[i].lexeme, type, type2);
				}
				else if (type != type2)
				{
					errors++;
					LOG_INCOMPATIBLETYPE_ERROR(table[i - 1].lexeme, type, table[i + 1].lexeme, type2);
				}
			}
		}

		// COLOCA VALOR DA VAR COMO REQUERIDO
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

					if (v.value.empty())
					{
						LOG_UNINITILIAZED_ERROR(v.identifier);
						errors++;
					}
				}
			}
		}
	}

	/*
	*	 CHECA SE ELSE É UTILIZADO APENAS APOS IF
	* 
	*	 CHECA SE IDENTFIER FOI DECLARADO
	*/
	bool usedIf = false;
	for (auto& e : table)
	{
		if (e.token == TOKEN::IDENTIFIER && e.id == -1)
		{
			errors++;
			LOG_IDENTIFIERNOTFOUND_ERROR(e.lexeme);
		}

		if (e.token == TOKEN::KEYWORD && e.lexeme == "if")
			usedIf = true;

		if (e.token == TOKEN::KEYWORD && e.lexeme == "else")
		{
			if (!usedIf)
			{
				errors++;
				LOG_WRONGELSEUSAGE_ERROR();
			}

			usedIf = false;
		}
	}


	/*
	*	CHECA SE VAR FOI INICIALIZADA
	*
	*	CHECA SE IDENTIFIER FOI DUPLICADO
	*/
	for (int i = 0; i < varTable.size(); i++)
	{
		if (varTable[i].value.empty() && !varTable[i].userInput)
		{
			if (!varTable[i].valueRequired)
			{
				LOG_UNINITILIAZED_WARN(varTable[i].identifier);
			}
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
	bool bracketNewLine = false;
	bool multipleDeclaration = false;
	bool outParameter = false;
	bool outEndLine = false;
	bool increment = false;

	for (int i = 0; i < table.size(); i++)
	{
		// SPACE
		if (!newLine && !increment && table[i].token != TOKEN::SEPARATOR && table[i - 1].lexeme != "(")
			outFile << " ";

		increment = false;

		if (table[i].token == TOKEN::SEPARATOR)
		{
			// DECLARAÇÃO MULTIPLA (, ... = ...)
			if (table[i].lexeme == ",")
			{
				multipleDeclaration = true;
				outFile << ",";
			}

			else if (table[i].lexeme == "(" && outParameter)
				outFile << " <<";

			else if (table[i].lexeme == ")" && outParameter)
			{
				if (outEndLine)
				{
					outEndLine = false;
					outFile << " << std::endl";
				}

				outParameter = false;
			}

			else if (table[i].lexeme == "{")
			{
				outFile << "\n";
				outFile << table[i].lexeme;
			}

			else if (table[i].lexeme == "}")
			{
				outFile << table[i].lexeme;
				bracketNewLine = false;
			}

			else
				outFile << table[i].lexeme;
		}
		else if (table[i].token == TOKEN::KEYWORD)
		{
			// NUL KEYWORD PARA DECLARAÇÃO MULTIPLA
			if (multipleDeclaration)
				multipleDeclaration = false;

			// C++ TIPOS
			else if (table[i].lexeme == "String")
				outFile << "std::string";

			else if (table[i].lexeme == "Integer")
				outFile << "int";

			else if (table[i].lexeme == "boolean" || table[i].lexeme == "Boolean")
				outFile << "bool";

			else if (table[i].lexeme == "Double")
				outFile << "double";

			else if (table[i].lexeme == "Float")
				outFile << "float";

			else if (table[i].lexeme == "System.out.println")
			{
				outFile << "std::cout";
				outParameter = true;
				outEndLine = true;
			}

			else if (table[i].lexeme == "System.out.print")
			{
				outFile << "std::cout";
				outParameter = true;
			}

			else
				outFile << table[i].lexeme;
		}
		else if (table[i].token == TOKEN::OPERATOR)
		{
			// MUDA OPERATOR + PARA << QUANDO SAIDA DE MULTIPLO PARAMETROS
			if (table[i].lexeme == "+" && outParameter)
				outFile << "<<";

			else
				outFile << table[i].lexeme;
		}
		else if (table[i].token == TOKEN::IDENTIFIER)
		{
			// INPUT 
			if (i + 2 < table.size() && table[i + 2].token == TOKEN::KEYWORD && table[i + 2].lexeme == "System.console().readLine()")
			{
				if (table[i - 1].token == TOKEN::KEYWORD && isTypeKeyword(table[i - 1].lexeme))
				{
					outFile << table[i].lexeme;
					outFile << ";\n";
					outFile << "std::cin >> ";
					outFile << table[i].lexeme;
					outFile << ";";
					i = i + 3;
				}
				else
				{
					outFile << "std::cin >> ";
					outFile << table[i].lexeme;
					outFile << ";";
					i = i + 3;
				}
			}
			else if (i + 1 < table.size() && (table[i + 1].lexeme == "++" || table[i + 1].lexeme == "--"))
			{
				outFile << table[i].lexeme;
				increment = true;
			}
			else
				outFile << table[i].lexeme;
		}
		else
			outFile << table[i].lexeme;

		newLine = false;

		// LINE BREAK
		if (table[i].lexeme == ";" || table[i].lexeme == "}" || table[i].lexeme == "{")
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
