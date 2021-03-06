#include <iostream>
#include <string>

#include "commonMethods.hpp"

namespace cmds
{
	int inputSafe(int num)
	{
		std::cin >> num;

		while (!std::cin) // make sure input isn't nothing
		{
			std::cout << "\nInvalid value, please retype the correct value: ";
			std::cin.clear();
			std::cin.ignore(100, '\n'); //moves 200 lines then goes to the next line
			std::cin >> num;
		}

		return num;
	};

	bool compareStrings(std::string str1, std::string str2)
	{
		if (str1.length() == str2.length())
		{
			for (int i = 0; i < str1.length(); i++)
			{
				if (!(str1[i] == str2[i]))
				{
					return false;
				}
			}

			return true;
		}

		return false;
	};

	void clearInput()
	{
		std::cin.clear();
		std::cin.ignore(200, '\n'); //moves 200 lines then goes to the next line
	}

	void clearInput(int characters)
	{
		std::cin.clear();
		std::cin.ignore(characters, '\n'); //moves 200 lines then goes to the next line
	}
}