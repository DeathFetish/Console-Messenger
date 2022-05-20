#pragma once
#include "UpdateFunctions.h"
#include "../../Client.h"
#include "../../Console.h"
#include "../Program.h"
#include "../ProgramState.h"

void UpdateFunctions::emptyUpdate(ProgramState* program, int keyCode1, int keyCode2) {}

void UpdateFunctions::mainExitButtonUpdate(ProgramState* program, int keyCode1, int keyCode2)
{
	if (keyCode1 == 13)
		Program::setCurrentState("Exit");
}

void UpdateFunctions::usersMenuExitButtonUpdate(ProgramState* program, int keyCode1, int keyCode2)
{
	if (keyCode1 != 13)
		return;

	for (int i = 1; i < program->controls.size(); ++i)
	{
		program->controlsMap.erase(program->controlsMap.find(dynamic_cast<Button*>(program->controls[i])->text));
		delete program->controls[i];
	}

	program->controls.erase(program->controls.begin() + 1, program->controls.end());

	Client::terminate();
	Program::setCurrentState("StartMenu");
}

void UpdateFunctions::exitChatButtonUpdate(ProgramState* program, int keyCode1, int keyCode2)
	{
		if (keyCode1 != 13)
			return;

		for (int i = 0; i < program->controls.size() - 2; ++i)
			delete program->controls[i];

		program->controls.erase(program->controls.begin(), program->controls.end() - 2);

		program->firstPrintableControl = 0;
		program->currentControl = 0;

		program->getControl<InputField>("MessageInput")->clearInput();
		Program::setCurrentState("UsersMenu");
	}

void UpdateFunctions::continueToUsersMenuButtonUpdate(ProgramState* program, int keyCode1, int keyCode2)
	{
		if (keyCode1 == 13)
			Program::setCurrentState("UsersMenu");
	}

void UpdateFunctions::loginInputUpdate(ProgramState* program, int keyCode1, int keyCode2)
	{
		auto loginInput = program->getControl<InputField>("LoginInput");

		switch (keyCode1)
		{
		case ' ': return;
		case 13:
		{
			std::string name(loginInput->getInput());
			if (name.length() < 5)
			{
				loginInput->setError("Error! Login must contain at least 5 chars.");
				return;
			}

			std::string error(Client::init());
			if (error != "")
			{
				loginInput->setError(error);
				return;
			}

			Client::sendData(65, name);
			Client::setClientName(name);
			Client::enableAuthorized(true);

			loginInput->clearError();
			loginInput->clearInput();

			program->currentControl = 0;

			Program::setCurrentState("UsersMenu");
			return;
		}
		default: loginInput->standartUpdate(keyCode1, keyCode2); break;
		}
	}

void UpdateFunctions::messageInputUpdate(ProgramState* program, int keyCode1, int keyCode2)
	{
		auto messageInput = program->getControl<InputField>("MessageInput");

		if (keyCode1 == 13)
		{
			if (messageInput->getInput() == "")
				return;

			Client::sendData(70, Client::getClientName() + ' ' + Client::getRecipientName() + ' ' + messageInput->getInput());

			program->addControl(new Button(program, "You: " + messageInput->getInput(), Console::Color::blackBlue, Console::Color::whiteBlue, UpdateFunctions::emptyUpdate));

			//	program->addButton("", "You: " + messageInput->getInput(), Console::Color::blackBlue, Console::Color::whiteBlue, UpdateFunctions::emptyUpdate, 2);

			messageInput->clearInput();

			program->currentControl++;
			if (program->currentControl + 1 > program->firstPrintableControl + Console::getHeight() - 1)
				program->firstPrintableControl++;
		}
		else
			messageInput->standartUpdate(keyCode1, keyCode2);
	}

void UpdateFunctions::toChatButton(ProgramState* program, int keyCode1, int keyCode2)
	{
		if (keyCode1 == 13)
		{
			std::string name = dynamic_cast<Button*>(program->controls[program->currentControl])->getText();

			Client::sendData(68, Client::getClientName() + ' ' + name + ' ');
			Client::setRecipientName(name);

			Program::setCurrentState("Chat");
		}
	}



void UpdateFunctions::startMenuServerUpdate(ProgramState* program, unsigned short functionCode, std::string& data) { }

void UpdateFunctions::usersMenuServerUpdate(ProgramState* program, unsigned short functionCode, std::string& data)
{
	switch (functionCode)
	{
	case 67:
	{
		bool online = (data[0] == '1');
		data = data.substr(1);
		if (program->getControl<Button*>(data) == nullptr)
		{
			if (online)
				program->addControl(new Button(program, std::string(data), Console::Color::blackDarkGreen, Console::Color::whiteDarkGreen, UpdateFunctions::toChatButton), std::string(data));
			else
				program->addControl(new Button(program, std::string(data), Console::Color::blackGrey, Console::Color::whiteGrey, UpdateFunctions::toChatButton), std::string(data));
		}
		else
		{
			if (online)
				program->getControl<Button>(data)->setColor(Console::Color::blackDarkGreen, Console::Color::whiteDarkGreen);
			else
				program->getControl<Button>(data)->setColor(Console::Color::blackGrey, Console::Color::whiteGrey);
		}

		break;
	}
	}
}

void UpdateFunctions::chatServerUpdate(ProgramState* program, unsigned short functionCode, std::string& data)
{
	switch (functionCode)
	{
	case 69:
		size_t separotorIndex = data.find_first_of(' ');
		std::string sender = data.substr(0, separotorIndex);
		std::string text = data.substr(separotorIndex + 1, data.length() - separotorIndex - 1);

		if (program->currentControl >= program->controls.size() - 2)
		{
			if (program->currentControl + 2 > program->firstPrintableControl + Console::getHeight() - 1)
				program->firstPrintableControl++;

			program->currentControl++;
		}

		if (sender == Client::getClientName())
			program->addControl(new Button(program, "You: " + text, Console::Color::blackBlue, Console::Color::whiteBlue, UpdateFunctions::emptyUpdate));
		else
			program->addControl(new Button(program, "     " + text, Console::Color::blackGrey, Console::Color::whiteGrey, UpdateFunctions::emptyUpdate));
		break;
	}
}