#pragma once
#include <map>
#include <vector>
#include <string>
#include <functional>

#include "Controls/Controls.h"
#include "Controls/InputField.h"
#include "Controls/Button.h"

class ProgramState
{
protected:
	friend class UpdateFunctions;
	
	std::map<std::string, Control*> controlsMap;
	std::vector<Control*> controls;
	unsigned short currentControl = 0;
	unsigned short firstPrintableControl = 0;

	ProgramState* prevState;
	std::string currentState;
	std::map<std::string, ProgramState*> nextStates;
	bool needStopRunning = false;

	std::function<void(ProgramState*, unsigned short, std::string&)> serverUpdate;

	void setCurrentState(std::string state) { currentState = state; }

public:
	ProgramState(ProgramState* prevState) : prevState(prevState) {}
	~ProgramState()
	{
		for (auto it = nextStates.begin(); it != nextStates.end(); ++it)
			delete it->second;
		nextStates.clear();

		for (auto i = 0; i != controls.size(); ++i)
			delete controls[i];

		controls.clear();
		controlsMap.clear();
	}

	void print()
	{
		if (!currentState.empty())
		{
			nextStates[currentState]->print();
			return;
		}

		system("cls");

		for (auto i = firstPrintableControl; i < controls.size() && i - firstPrintableControl < Console::getHeight(); ++i)
			controls[i]->print(currentControl == i, i != firstPrintableControl);
	}

	virtual void update(int keyCode1, int keyCode2)
	{
		if (!currentState.empty())
		{
			nextStates[currentState]->update(keyCode1, keyCode2);
			return;
		}

		if (keyCode1 == 224)
		{
			switch (keyCode2)
			{
			case 72:
				if (currentControl > 0)
				{
					currentControl--;
					if (currentControl < firstPrintableControl)
						firstPrintableControl--;
				}
				return;
			case 80:
				if (currentControl < controls.size() - 1)
				{
					currentControl++;
					if (currentControl > firstPrintableControl + Console::getHeight() - 1)
						firstPrintableControl++;
				}
				return;
			}
		}

		controls[currentControl]->update(keyCode1, keyCode2);
	}

	virtual void update(int functionCode, std::string data)
	{
		serverUpdate(this, functionCode, data);
	}

	Button* addButton(
		std::string name,
		std::string text,
		Console::Color basicColor,
		Console::Color activeColor,
		std::function<void(ProgramState*, int, int)> callback,
		unsigned short endOffset = 0)
	{
		Button* button = new Button(this, text, basicColor, activeColor, callback);
		controls.insert(controls.end() - endOffset, button);

		if (!name.empty())
			controlsMap.emplace(std::make_pair(name, button));

		return button;
	}

	InputField* addInputField(
		std::string name,
		std::string text,
		unsigned short maxLength,
		Console::Color basicColor,
		Console::Color activeColor,
		std::function<void(ProgramState*, int, int)> callback)
	{
		InputField* inputField = new InputField(this, text, maxLength, basicColor, activeColor, callback);
		controls.push_back(inputField);

		if (!name.empty())
			controlsMap.emplace(std::make_pair(name, inputField));

		return inputField;
	}

	ProgramState* addProgramState(std::string stateName)
	{
		ProgramState* nextState = new ProgramState(this);
		nextStates.emplace(std::make_pair(stateName, nextState));
		return nextState;
	}

	Control* getControl(std::string name)
	{
		if (controlsMap.find(name) == controlsMap.end())
			return nullptr;
		else
			return controlsMap[name];
	}

	bool shouldClose() const { return needStopRunning; }

	void setServerUpdate(std::function<void(ProgramState*, unsigned short, std::string&)> serverUpdate)
	{
		this->serverUpdate = serverUpdate;
	}
};