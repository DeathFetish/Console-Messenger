#pragma once
#include <iostream>
#include <chrono>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <vector>
#include <thread>
#include <conio.h>
#include <stdio.h>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")

#include "Controls/Controls.h"

class Menu
{
private:
	HANDLE handle;

	std::vector<Control*> controls;
	Control* controlInFocus;

	USHORT currentControl = 0;
	std::vector<std::string> menuItems;

public:
	Menu()
	{
		handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, 15);
	}

	void addButton(std::string text)
	{
		std::string str = std::string(80 - text.length(), ' ');
		str.insert(str.begin(), text.begin(), text.end());
		str.copy(const_cast<char*>(text.c_str()), text.length());
		controls.emplace_back(new Button(str));
	}

	void addLabel(std::string text)
	{
		std::string str = std::string(80 - text.length(), ' ');
		str.insert(str.begin(), text.begin(), text.end());
		str.copy(const_cast<char*>(text.c_str()), text.length());
		controls.emplace_back(new Button(str));
	}

	void addInputField(std::string text)
	{
		controls.emplace_back(new InputField(text));
	}

	void print()
	{
		system("cls");
		for (auto i = 0; i < controls.size(); ++i)
		{
			controls[i]->print(handle, currentControl == i);
		}
	}

	void update(int keyCode)
	{
		//	std::cout << keyCode << "|" << std::endl;
		switch (keyCode)
		{
		case 72:
			if (currentControl > 0)
				currentControl--;
			break;
		case 80:
			if (currentControl < controls.size() - 1)
				currentControl++;
			break;
		}

		controls[currentControl]->update(keyCode);
	}

	const Control* getControlInFocus() { return controls[currentControl]; }

	const HANDLE getHandle() { return handle; }
};