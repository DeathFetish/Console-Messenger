#pragma once
#include "ProgramState.h"
#include "Controls/Controls.h"
#include "RecipientMenu.h"
#include "../Console.h"

class Program : public ProgramState
{
private:
	InputField* login;
	Button* continueButton;
	Button* exitButton;
	Label* errorProvider;

	std::vector<Control*> controls;
	bool isEnd = false;
public:

	Program(ProgramState* prevState) : ProgramState(prevState)
	{
		currentState = "";
		std::string inputFieldText("Login: ");
		login = new InputField(inputFieldText);

		continueButton = new Button(console::getConsoleString("Continue"));
		exitButton = new Button(console::getConsoleString("Exit"));
		errorProvider = new Label("", 12U);

		nextStates.emplace(std::make_pair("RecipientMenu", new RecipientMenu(this)));

		controls.push_back(login);
		controls.push_back(continueButton);
		controls.push_back(exitButton);
	}

	void update(int keyCode1, int keyCode2) override
	{
		if (!currentState.empty())
		{
			nextStates[currentState]->update(keyCode1, keyCode2);
			return;
		}

		if (keyCode1 == 13)
		{
			if (currentControl == 2)
				isEnd = true;

			if (currentControl == 1)
			{
				std::string error(client::init());
				
				if (error != "")
				{
					errorProvider->setText(error);
					return;
				}
				else
				{
					std::string packet = "";
					errorProvider->setText(packet);

					modbus::makePacket(65, login->getInput().c_str(), packet);
					
					client::clientName = login->getInput();
					client::isAuthorized = true;

					currentState = "RecipientMenu";
					login->clearInput();

					send(client::clientSocket, packet.c_str(), packet.length(), NULL);

				//	std::cout << login->getInput() << std::endl;
				}
				
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

		if (currentControl == 0 && keyCode1 != ' ')
			login->update(keyCode1, keyCode2);
	}

	void update(int functionCode, std::string data) override
	{
		switch (functionCode)
		{
		case 67:
			nextStates["RecipientMenu"]->update(functionCode, data);
			break;
		case 69:
			nextStates["RecipientMenu"]->update(functionCode, data);
			break;
		}
	}

	void print() override
	{
		if (!currentState.empty())
		{
			nextStates[currentState]->print();
			return;
		}

		system("cls");
		for (auto i = 0; i < controls.size(); ++i)
		{
			controls[i]->print(currentControl == i);
		}
		errorProvider->print(false);
	}

	bool shouldClose() const { return isEnd; }
};