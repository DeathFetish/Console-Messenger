#pragma once
#include "ProgramState.h"
#include "Chat.h"
#include "../Client.h"

class RecipientMenu : public ProgramState
{
	std::vector<Button*> controls;
	unsigned short firstPrintableControl = 0;

public:
	RecipientMenu(ProgramState* prevState) : ProgramState(prevState)
	{
		currentState = "";
		controls.push_back(new Button(std::string("Exit")));

		nextStates.emplace(std::make_pair("Chat", new Chat(this)));
	}

	~RecipientMenu()
	{
		for (auto it = controls.begin(); it != controls.end(); ++it)
			delete *it;
		controls.clear();
	}

	void update(int keyCode1, int keyCode2) override
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

		if (keyCode1 == 13)
		{
			if (currentControl == 0)
			{
				for (int i = 1; i < controls.size(); ++i)
					delete controls[i];
				controls.erase(controls.begin() + 1, controls.end());

				client::terminate();
				prevState->setCurrentState("");
			}
			else
			{
				std::string packet;
				std::string data = client::clientName + ' ' + controls[currentControl]->getText() + ' ';
				modbus::makePacket(68, data.c_str(), packet);
				send(client::clientSocket, packet.c_str(), packet.length(), NULL);
				
				currentState = "Chat";
				((Chat*)(nextStates["Chat"]))->setRecipeint(controls[currentControl]->getText());
			
			//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}
	}

	void update(int functionCode, std::string data) override
	{
		switch (functionCode)
		{
			case 67:
			{
				Button* button = new Button(std::string(data), 8, 248);
				controls.push_back(button);
				break;
			}
			case 69:
			{
				size_t separator = data.find_first_of(' ');
				std::string sender = data.substr(0, separator);

				if (currentState == "Chat" && (sender == (dynamic_cast<Chat*>(nextStates["Chat"]))->getRecipeint() || sender == client::clientName)) // переделать!!!
					nextStates[currentState]->update(functionCode, data);
			}
		}
	}

	void print()
	{
		if (!currentState.empty())
		{
			nextStates[currentState]->print();
			return;
		}

		system("cls");
		for (auto i = firstPrintableControl; i < controls.size() && i  - firstPrintableControl < Console::getHeight(); ++i)
		{
			controls[i]->print(i == currentControl, i != firstPrintableControl);
		}
	}
};