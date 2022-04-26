#pragma once
#include <iostream>
#include <string>
#include <conio.h>
#include <stdio.h>

#include "../../Console.h"

class Control
{
public:
	virtual void print(bool inFocus, bool isNewLine) = 0;
};

class Label : public Control
{
private:
	std::string text;
	unsigned short textColor;
public:
	Label() = delete;
	Label(const Label&) = delete;
	Label& operator = (const Label&) = delete;

	Label(const std::string& text, const unsigned short textColor = 15) : text(text), textColor(textColor) {}

	void print(bool inFocus, bool isNewLine) override
	{
		if (isNewLine) printf("\n");
		SetConsoleTextAttribute(Console::handle, textColor);
		printf(Console::getPrintfParametr().c_str(), text.c_str());
		SetConsoleTextAttribute(Console::handle, Console::color);
	}

	void setText(const std::string& text) { this->text = text; }

	const std::string& getText() { return text; }
};

class Button : public Control
{
private:
	std::string text;
	unsigned short basicColor;
	unsigned short activeColor;

public:
	Button() = delete;
	Button(const Button&) = delete;
	Button& operator = (const Button&) = delete;

	Button(std::string& text, unsigned short basicColor = 15, unsigned short activeColor = 240)
		: text(text), basicColor(basicColor), activeColor(activeColor) {}

	void print(bool inFocus, bool isNewLine) override
	{
		if (isNewLine) printf("\n");

		if (inFocus)
			SetConsoleTextAttribute(Console::handle, activeColor);
		else
			SetConsoleTextAttribute(Console::handle, basicColor);

		printf(Console::getPrintfParametr().c_str(), text.c_str());
		SetConsoleTextAttribute(Console::handle, Console::color);
	}

	const std::string& getText() { return text; }
};

class InputField : public Control
{
private:
	unsigned short maxLength;
	unsigned short cursorPos = 0;
	std::string input;
	std::string text;

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
	InputField(std::string text, unsigned short maxLength = 40) : text(text), maxLength(maxLength) {}

	void print(bool inFocus, bool isNewLine) override
	{
		if (isNewLine) printf("\n");

		if (inFocus)
		{
			SetConsoleTextAttribute(Console::handle, 240);
			secondaryPrint(inFocus);
			SetConsoleTextAttribute(Console::handle, Console::color);
		}
		else
			secondaryPrint(inFocus);
	}

	void update(int keyCode1, int keyCode2)
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
	};

	const std::string& getInput() { return input; }
	void clearInput() { input = ""; cursorPos = 0; }
};