#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>

void readData(const std::string& path)
{
	std::fstream reader;

	reader.open(path);

	if (reader)
	{
		std::string line;

		while (std::getline(reader, line))
		{
			std::cout << line << std::endl;
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