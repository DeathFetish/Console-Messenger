#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <conio.h>
#include <stdio.h>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")

#include "../Menu.h"

class Control
{
public:
	virtual void print(HANDLE handle, bool inFocus) = 0;
	virtual void update(int keyCode) = 0;
};

class Label : public Control
{
private:
	std::string text;
public:
	Label() = delete;
	Label(const Label&) = delete;
	Label& operator = (const Label&) = delete;

	Label(std::string& text) : text(text) {}

	void print(HANDLE handle, bool inFocus) override
	{
		if (inFocus)
		{
			SetConsoleTextAttribute(handle, 240);
			std::cout << text << std::endl;
			SetConsoleTextAttribute(handle, 15);
		}
		else
			std::cout << text << std::endl;
	}

	void update(int keyCode) override {};
};

class Button : public Control
{
private:
	std::string text;
public:
	Button() = delete;
	Button(const Button&) = delete;
	Button& operator = (const Button&) = delete;

	Button(std::string& text) : text(text) {}

	void print(HANDLE handle, bool inFocus) override
	{
		if (inFocus)
		{
			SetConsoleTextAttribute(handle, 240);
			std::cout << text << std::endl;
			SetConsoleTextAttribute(handle, 15);
		}
		else
			std::cout << text << std::endl;
	}

	void update(int keyCode) override {};
};

class InputField : public Control
{
private:
	const unsigned int maxLength = 80;

	std::string text;

	unsigned short cursorPos = 0;
	std::string input;

	void print(bool inFocus)
	{
		std::cout << text;
		if (inFocus && input.length() == 0)
		{
			std::cout << '_';
			std::cout << std::string(maxLength - text.length() - 1 - input.length(), ' ') << std::endl;
			return;
		}
		else
		{
			for (unsigned short i = 0; i < input.length(); ++i)
			{
				if (inFocus && cursorPos == i)
					std::cout << '_';
				std::cout << input[i];
			}
		}
		
		if (inFocus && cursorPos == input.length())
			std::cout << '_';


		std::cout << std::string(maxLength - text.length() - 1 - input.length(), ' ') << std::endl;
	}
public:
	InputField() = delete;
	InputField(const InputField&) = delete;
	InputField& operator = (const InputField&) = delete;

	InputField(std::string text) : text(text) {}

	void print(HANDLE handle, bool inFocus) override
	{
		if (inFocus)
		{
			SetConsoleTextAttribute(handle, 240);
			print(inFocus);
			SetConsoleTextAttribute(handle, 15);
		}
		else
			print(inFocus);
	}

	void update(int keyCode) override
	{
		switch (keyCode)
		{
		case 224:
			return;
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
		case 8:
			if (cursorPos > 0)
			{
				input.erase(input.begin() + cursorPos - 1);
				cursorPos--;
			}
			return;
		}
		input.insert(input.begin() + cursorPos, keyCode);
		cursorPos++;
	};
};