#pragma once
#include <functional>
#include "Controls.h"

class InputField : public Control
{
private:
	unsigned short maxLength;
	unsigned short cursorPos = 0;
	
	std::string input;
	std::string text;
	std::string errorProvider = "";

	std::function<void(ProgramState*, int, int)> callback;

	void secondaryPrint(bool inFocus)
	{
		printf("%s", text.c_str());

		if (inFocus && input.length() == 0)
		{
			printf("%c", '_');
			printf("%s", std::string(Console::getWidth() - text.length() - 1 - input.length(), ' ').c_str());
			return;
		}
		else
		{
			for (unsigned short i = 0; i < input.length(); ++i)
			{
				if (inFocus && cursorPos == i)
					printf("%c", '_');
				printf("%c", input[i]);
			}
		}

		if (inFocus && cursorPos == input.length())
			printf("%c", '_');

		printf("%s", std::string(Console::getWidth() - text.length() - 1 - input.length(), ' ').c_str());
	}
	

public:
	InputField() = delete;
	InputField(const InputField&) = delete;
	InputField& operator = (const InputField&) = delete;
	InputField(ProgramState* program,
		std::string text,
		unsigned short maxLength,
		Console::Color basicColor,
		Console::Color activeColor,
		std::function<void(ProgramState*, int, int)> callback)
		: text(text), maxLength(maxLength), Control(basicColor, activeColor, program), callback(callback) {}

	void print(bool inFocus, bool isNewLine) override
	{
		SetConsoleTextAttribute(Console::handle, inFocus ? activeColor : basicColor);

		if (isNewLine) printf("\n");
		secondaryPrint(inFocus);

		if (errorProvider != "")
		{
			SetConsoleTextAttribute(Console::handle, (WORD)Console::Color::blackRed);
			printf("\n");
			printf(Console::getPrintfParametr().c_str(), errorProvider.c_str());
		}
		SetConsoleTextAttribute(Console::handle, (WORD)Console::Color::blackWhite);
	}

	void update(int keyCode1, int keyCode2)
	{
		callback(program, keyCode1, keyCode2);
	}

	void standartUpdate(int keyCode1, int keyCode2)
	{
		if (keyCode1 == 224)
		{
			switch (keyCode2)
			{
			case 75:
				if (cursorPos > 0)
					cursorPos--;
				return;
			case 77:
				if (cursorPos < input.length())
					cursorPos++;
				return;
			case 83:
				if (cursorPos < input.length())
					input.erase(input.begin() + cursorPos);
				return;
			}
		}

		switch (keyCode1)
		{
		case 9: case 13: return;
		case 8:
			if (cursorPos > 0)
			{
				input.erase(input.begin() + cursorPos - 1);
				cursorPos--;
			}
			return;
		}

		if (input.length() >= maxLength)
			return;

		input.insert(input.begin() + cursorPos, keyCode1);
		cursorPos++;
	}

	const std::string& getInput() { return input; }
	void clearInput() { input = ""; cursorPos = 0; }
	
	void setError(std::string error) { errorProvider = error; }
	void clearError() { errorProvider = ""; }
};