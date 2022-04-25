#pragma once
#include <string>
#include <Windows.h>

namespace console
{
	const unsigned short consoleWidth = 80;
	const unsigned short consoleHeight = 25;
	HANDLE handle;

	void init()
	{
		handle = GetStdHandle(STD_OUTPUT_HANDLE);

		std::string comand = "mode con cols=" + std::to_string(consoleWidth) + "lines=" + std::to_string(consoleHeight);
		system(comand.c_str());

		HWND consoleWindow = GetConsoleWindow();
		SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

		CONSOLE_CURSOR_INFO structCursorInfo;
		GetConsoleCursorInfo(handle, &structCursorInfo);
		structCursorInfo.bVisible = FALSE;
		SetConsoleCursorInfo(handle, &structCursorInfo);
	}

	std::string getConsoleString(std::string text)
	{
		std::string str = std::string(consoleWidth - text.length(), ' ');
		str.insert(str.begin(), text.begin(), text.end());
		str.copy(const_cast<char*>(text.c_str()), text.length());
		return str;
	}
}