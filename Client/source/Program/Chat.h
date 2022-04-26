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

	unsigned short firstPrintableControl = 0;
	
	std::vector<Control*> controls;

public:

	Chat(ProgramState* prevState) : ProgramState(prevState)
	{
		currentState = "";
		exitButton = new Button(std::string("Exit"));
		inputField = new InputField("You: ");
		
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
				
				controls.insert(controls.end() - 2, new Button(("You: " + inputField->getInput()), 11, 243));
				inputField->clearInput();

				modbus::makePacket(70, data.c_str(), packet);
				send(client::clientSocket, packet.c_str(), packet.length(), NULL);

				currentControl++;
				if (currentControl + 1 > firstPrintableControl + Console::getHeight() - 1)
					firstPrintableControl++;
			}

			if (currentControl == controls.size() - 1)
			{
				for (int i = 0; i < controls.size() - 2; ++i)
				{
					delete controls[i];
				}
				controls.erase(controls.begin(), controls.end() - 2);
				
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

		if (currentControl == controls.size() - 2)
			inputField->update(keyCode1, keyCode2);
	}

	void update(int functionCode, std::string data) override
	{
		switch (functionCode)
		{
		case 69:
			size_t separotorIndex = data.find_first_of(' ');
			std::string sender = data.substr(0, separotorIndex);
			std::string text = data.substr(separotorIndex + 1, data.length() - separotorIndex - 1);

			if (currentControl >= controls.size() - 2)
			{
				if (currentControl + 2 > firstPrintableControl + Console::getHeight() - 1)
					firstPrintableControl++;

				currentControl++;
			}
			
			if (sender == client::clientName)
				controls.insert(controls.end() - 2, new Button(("You: " + text), 11, 243));
			else
				controls.insert(controls.end() - 2, new Button(("     " + text), 8, 248));

			break;
		}
	}

	void print() override
	{
		system("cls");
		
		for (auto i = firstPrintableControl; i < controls.size() && i - firstPrintableControl < Console::getHeight(); ++i)
			controls[i]->print(currentControl == i, i != firstPrintableControl);
	}

	void setRecipeint(const std::string& currentRecipint) { recipient = currentRecipint; }
	
	const std::string& getRecipeint() { return recipient; }
};