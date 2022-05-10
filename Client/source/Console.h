#pragma once
#include <string>
#include <Windows.h>

class Console
{
private:
	static unsigned short width;
	static unsigned short height;
	static std::string printfParametr;

public:
	enum class Color
	{
		blackDarkGreen	= 2,
		blackGrey		= 8,
		blackBlue		= 11,
		blackRed		= 12,
		blackWhite		= 15,
		whiteBlack		= 240,
		whiteDarkGreen	= 242,
		whiteBlue		= 243,
		whiteGrey		= 248
	};

	static const HANDLE handle;

	static void init(unsigned short consoleWidth = 80, unsigned short consoleHeight = 25)
	{
		width = consoleWidth;
		height = consoleHeight;

		printfParametr = "%-" + std::to_string(width) + "s";
		std::string comand = "mode con cols=" + std::to_string(consoleWidth) + "lines=" + std::to_string(consoleHeight);
		system(comand.c_str());

		HWND consoleWindow = GetConsoleWindow();
		SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

		CONSOLE_CURSOR_INFO structCursorInfo;
		GetConsoleCursorInfo(handle, &structCursorInfo);
		structCursorInfo.bVisible = FALSE;
		SetConsoleCursorInfo(handle, &structCursorInfo);
	}

	static const unsigned short getWidth() { return width; }
	static const unsigned short getHeight() { return height; }
	static const std::string& getPrintfParametr() { return printfParametr; }
};

unsigned short Console::width;
unsigned short Console::height;
std::string	   Console::printfParametr;
const HANDLE   Console::handle = GetStdHandle(STD_OUTPUT_HANDLE);;