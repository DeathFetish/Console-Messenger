#pragma once
#include <iostream>
#include <string>
#include <conio.h>
#include <stdio.h>

#include "../../Console.h"

class ProgramState;

class Control
{
protected:
	unsigned short basicColor;
	unsigned short activeColor;
	ProgramState* program;

	std::string error;
public:
	Control(Console::Color basicColor, Console::Color activeColor, ProgramState* program)
		: basicColor((unsigned short)basicColor), activeColor((unsigned short)activeColor), program(program) {}

	virtual void print(bool inFocus, bool isNewLine) = 0;
	virtual void update(int keyCode1, int keyCode2) = 0;

	void setColor(Console::Color basicColor, Console::Color activeColor)
	{
		this->basicColor = (unsigned short)basicColor;
		this->activeColor = (unsigned short)activeColor;
	}
};