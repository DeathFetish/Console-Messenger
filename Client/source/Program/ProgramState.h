#pragma once
#include <map>
#include <vector>
#include <string>
#include "Controls/Controls.h"

class ProgramState
{
protected:
	ProgramState* prevState;
	std::map<std::string, ProgramState*> nextStates;
	
	std::vector<Control*> printable;
	std::vector<Control*> focusable;

	std::string currentState;

	unsigned short currentControl;

public:
	ProgramState(ProgramState* prevState) : prevState(prevState), currentControl(0) {}

	virtual void print() = 0;
	virtual void update(int keyCode1, int keyCode2) = 0;
	virtual void update(int functionCode, std::string data) = 0;

	void setCurrentState(std::string state) { currentState = state; }
};