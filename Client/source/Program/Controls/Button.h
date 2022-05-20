#pragma once
#include <functional>
#include "Controls.h"

class Button : public Control
{
private:
	friend class UpdateFunctions;

	std::string text;
	std::function<void(ProgramState*, int, int)> callback;

public:
	Button() = delete;
	Button(const Button&) = delete;
	Button& operator = (const Button&) = delete;

	Button(ProgramState* program,
		std::string text,
		Console::Color basicColor,
		Console::Color activeColor,
		std::function<void(ProgramState*, int, int)> callback)
		: text(text), Control(basicColor, activeColor, program), callback(callback) {}

	void print(bool inFocus, bool isNewLine) override
	{
		SetConsoleTextAttribute(Console::handle, inFocus ? activeColor : basicColor);

		if (isNewLine) printf("\n");
		printf(Console::getPrintfParametr().c_str(), text.c_str());

		SetConsoleTextAttribute(Console::handle, (WORD)Console::Color::blackWhite);
	}

	void update(int keyCode1, int keyCode2) override { callback(program, keyCode1, keyCode2); }

	const std::string& getText() { return text; }
};