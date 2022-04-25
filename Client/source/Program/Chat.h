#pragma once
#include "ProgramState.h"
#include "Controls/Controls.h"
#include "../Client.h"
#include "../Console.h"

class Chat : public ProgramState
{
private:
	std::string recipient;

	Button* exitButton;
	std::vector<Button*> messages;
	InputField* inputField;
	int chatHeight;

	int messageIndex = 0;
	int firstPrintableControl = 0;
	
	std::vector<Control*> controls;

public:

	Chat(ProgramState* prevState) : ProgramState(prevState)
	{
		currentState = "";
		exitButton = new Button(console::getConsoleString("Exit"));
		inputField = new InputField("You: ");
		chatHeight = console::consoleHeight - 2;
		
		controls.push_back(inputField);
		controls.push_back(exitButton);
	}
	
	~Chat() {}

	void update(int keyCode1, int keyCode2) override
	{
		if (keyCode1 == 13)
		{
			if (currentControl == controls.size() - 2 && inputField->getInput() != "")
			{
				std::string packet;
				std::string data = client::clientName + ' ' + recipient + ' ' + inputField->getInput();
				
				controls.insert(controls.end() - 2, new Button(console::getConsoleString("You: " + inputField->getInput()), 11, 243));
				inputField->clearInput();

				modbus::makePacket(70, data.c_str(), packet);
				send(client::clientSocket, packet.c_str(), packet.length(), NULL);

				currentControl++;
			}

			if (currentControl == controls.size() - 1)
			{
				for (int i = 0; i < controls.size() - 2; ++i)
				{
					delete controls[i];
				}
				controls.erase(controls.begin(), controls.end() - 2);
				
				messageIndex = 0;
				firstPrintableControl = 0;
				currentControl = 0;

				inputField->clearInput();
				prevState->setCurrentState("");
				return;
			}
		}

		if (keyCode1 == 224)
		{
			switch (keyCode2)
			{
			case 72:
				if (currentControl > 0)
					currentControl--;
				return;
			case 80:
				if (currentControl < controls.size() - 1)
					currentControl++;
				return;
			}
		}

		if (currentControl == controls.size() - 2)
			inputField->update(keyCode1, keyCode2);
	}

	void update(int functionCode, std::string data) override
	{
		switch (functionCode)
		{
		case 69:
			int separotorIndex = data.find_first_of(' ');
			std::string sender = data.substr(0, separotorIndex);
			std::string text = data.substr(separotorIndex + 1, sender.length() - separotorIndex - 1);

			if (sender == client::clientName)
			{
			//	messages.push_back(new Button(console::getConsoleString("You: " + text), 11, 243));
				controls.insert(controls.end() - 2, new Button(console::getConsoleString("You: " + text), 11, 243));
			}
			else
			{
			//	messages.push_back(new Button(console::getConsoleString("     " + text), 8, 248));
				controls.insert(controls.end() - 2, new Button(console::getConsoleString("     " + text), 8, 248));
			}

			/*if (messages.size() > chatHeight && messageIndex + chatHeight + 1 != messages.size())
			{
				messageIndex++;
			}*/
			
			/*if (messages.size() - messageIndex <= chatHeight)
				currentControl++;*/

			break;
		}
	}

	void print() override
	{
		system("cls");

		for (int i = firstPrintableControl; i < firstPrintableControl + console::consoleHeight && i < controls.size(); ++i)
		{
			controls[i]->print(currentControl == i);
		}
		
	//	inputField->print(currentControl == messages.size());
	//	exitButton->print(currentControl == messages.size() + 1);
	}

	void setRecipeint(const std::string& currentRecipint) { recipient = currentRecipint; }
	const std::string& getRecipeint() { return recipient; }
};