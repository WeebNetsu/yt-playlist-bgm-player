#include <iostream>
#include <string>
#include <algorithm> // to use std::all_of

#include "../include/CommonMethods.hpp"
#include "../include/Color.hpp"

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

	// type = "normal"; breakline = true; by default
	void showMessage(std::string message, std::string type, bool breakLine)
	{
		Color::Modifier defColor = Color::FG_DEFAULT; // normal

		if (type == "error")
		{
			// error: this stops the application from running
			defColor = Color::FG_RED;
		}
		else if (type == "success")
		{
			// completed successfully
			defColor = Color::FG_GREEN;
		}
		else if (type == "warning")
		{
			// error, bad notice, application continues running
			defColor = Color::FG_YELLOW;
		}
		else if (type == "notice")
		{
			// notice message
			defColor = Color::FG_BLUE;
		}

		if (breakLine)
		{
			std::cout << defColor << message << std::endl;
		}
		else
		{
			std::cout << defColor << message;
		}

		//reset the console color
		defColor = Color::FG_DEFAULT;
		std::cout << defColor;
	}

	std::string trimStr(std::string str)
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