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

		continueButton = new Button(std::string("Continue"));
		exitButton = new Button(std::string("Exit"));
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
				std::string name(login->getInput());
				if (name.length() < 5)
				{
					errorProvider->setText("Error! Login must contain at least 4 chars.");
					return;
				}

				std::string error(client::init());
				if (error != "")
				{
					errorProvider->setText(error);
					return;
				}

				std::string packet;
				modbus::makePacket(65, name.c_str(), packet);

				errorProvider->setText("");
				login->clearInput();
				currentControl = 0;

				client::clientName = name;
				client::isAuthorized = true;

				currentState = "RecipientMenu";

				send(client::clientSocket, packet.c_str(), packet.length(), NULL);
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
			controls[i]->print(currentControl == i, i != 0);
		}
		errorProvider->print(false, false);
	}

	bool shouldClose() const { return isEnd; }
};