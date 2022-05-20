#pragma once
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>

#include "Controls/Controls.h"
#include "Controls/InputField.h"
#include "Controls/Button.h"

class ProgramState
{
protected:
	friend class UpdateFunctions;
	friend class Program;
	
	std::map<std::string, std::map<std::string, Control*>> allControls;
	std::vector<Control*> controls;

	unsigned short currentControl = 0;
	unsigned short firstPrintableControl = 0;

	std::function<void(ProgramState*, unsigned short, std::string&)> serverUpdate;

	ProgramState() {}
	~ProgramState()
	{
		for (auto i = 0; i != controls.size(); ++i)
			delete controls[i];

		controls.clear();
		allControls.clear();
	}

public:

	void print()
	{
		system("cls");

		for (auto i = firstPrintableControl; i < controls.size() && i - firstPrintableControl < Console::getHeight(); ++i)
			controls[i]->print(currentControl == i, i != firstPrintableControl);
	}

	virtual void update(int keyCode1, int keyCode2)
	{
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

	virtual void update(int functionCode, std::string& data)
	{
		serverUpdate(this, functionCode, data);
	}

	template <typename T>
	void addControl(T* control, int position, std::string name = "")
	{
		position = std::clamp(position, 0, (int)controls.size());
		controls.insert(controls.begin() + position, control);

		if (!name.empty())
			allControls[typeid(T).name()].emplace(std::make_pair(name, control));
	}

	template <typename T>
	T* const getControl(const std::string name)
	{
		if (allControls.find(typeid(T).name()) == allControls.end())
			return nullptr;

		auto& typeNameMap = allControls[typeid(T).name()];
		return typeNameMap.find(name) != typeNameMap.end() ? static_cast<T*>(typeNameMap[name]) : nullptr;
	}

	void setServerUpdate(std::function<void(ProgramState*, unsigned short, std::string&)> serverUpdate)
	{
		this->serverUpdate = serverUpdate;
	}

	const unsigned short getControlsCount() { return controls.size(); }
};