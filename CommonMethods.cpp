#include <iostream>
#include <string>
#include <algorithm> // to use std::all_of

#include "CommonMethods.hpp"

namespace cmds
{
	int numberInput() // gets number input
	{
		int num;
		std::cin >> num;

		while (!std::cin) // make sure input isn't nothing or a string
		{
			std::cout << "\nInvalid value, please enter a number: "; // error message
			clearInput();											 // clears the input
			std::cin >> num;
		}

		return num;
	};

	bool sii(std::string str) // checks if string is a positive integer (eg: "24" = true)
	{
		return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
	}

	bool compareStrings(std::string str1, std::string str2)
	{ // compare 2 strings with each other
		if (str1.length() == str2.length())
		{ // are they the same length?
			for (int i = 0; i < str1.length(); i++)
			{ // loop through the entire string
				if (!(str1[i] == str2[i]))
				{ // does a character not match?
					return false;
				}
			}

			return true;
		}

		return false;
	};

	void clearInput() // clears the input
	{
		std::cin.clear();
		std::cin.ignore(200, '\n'); //moves 200 lines then goes to the next line
	}

	void clearInput(int characters)
	{
		std::cin.clear();
		std::cin.ignore(characters, '\n');
	}

	std::string skinStr(std::string str)
	{
		while (str.size())
		{
			//the number 33 refers to ascii character 33... the first printable character
			if (str[0] < 33)
				str.erase(str.begin()); //remove all leading whitespace
			else if (str[str.size() - 1] < 33)
				str.erase(str.size() - 1); //remove all trailing whitespace
			else
				break;
		}

		return str;
	}
}