#pragma once
#include "../Program.h"
#include "../ProgramState.h"


class UpdateFunctions
{
public:
	static void emptyUpdate(ProgramState* program, int keyCode1, int keyCode2) {}

	static void mainExitButtonUpdate(ProgramState* program, int keyCode1, int keyCode2)
	{
		if (keyCode1 == 13)
			Program::setCurrentState("Exit");
	}

	static void usersMenuExitButtonUpdate(ProgramState* program, int keyCode1, int keyCode2)
	{
		if (keyCode1 != 13)
			return;

		for (int i = 1; i < program->getControlsCount(); ++i)
		{
			program->allControls["class Button"].erase(dynamic_cast<Button*>(program->controls[i])->getText());
			delete program->controls[i];
		}

		program->controls.erase(program->controls.begin() + 1, program->controls.end());

		Client::terminate();
		Program::setCurrentState("StartMenu");
	}

	static void exitChatButtonUpdate(ProgramState* program, int keyCode1, int keyCode2)
	{
		if (keyCode1 != 13)
			return;
		
		for (int i = 0; i < program->controls.size() - 2; ++i)
		{
			delete program->controls[i];
		}
		program->controls.erase(program->controls.begin(), program->controls.end() - 2);


		program->firstPrintableControl = 0;
		program->currentControl = 0;

		program->getControl<InputField>("MessageInput")->clearInput();
		Program::setCurrentState("UsersMenu");
	}

	static void continueToUsersMenuButtonUpdate(ProgramState* program, int keyCode1, int keyCode2)
	{
		if (keyCode1 == 13)
			Program::setCurrentState("UsersMenu");
	}

	static void loginInputUpdate(ProgramState* program, int keyCode1, int keyCode2)
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

			loginInput->clearError();
			loginInput->clearInput();
			program->currentControl = 0;

			Client::sendData(65, name.c_str());
			Client::setClientName(name);
			Client::enableAuthorized(true);

			Program::setCurrentState("UsersMenu");

			return;
		}
		default: loginInput->standartUpdate(keyCode1, keyCode2); break;
		}
	}

	static void messageInputUpdate(ProgramState* program, int keyCode1, int keyCode2)
	{
		auto messageInput = program->getControl<InputField>("MessageInput");
		
		if (keyCode1 == 13)
		{
			if (messageInput->getInput() == "")
				return;

			Client::sendData(70, Client::getClientName() + ' ' + Client::getRecipientName() + ' ' + messageInput->getInput());
			auto messageButton = new Button(program, "You: " + messageInput->getInput(), Console::Color::blackBlue, Console::Color::whiteBlue, UpdateFunctions::emptyUpdate);
			program->addControl(messageButton, program->getControlsCount() - 2);

			messageInput->clearInput();

			program->currentControl++;
			if (program->currentControl + 1 > program->firstPrintableControl + Console::getHeight() - 1)
				program->firstPrintableControl++;
		}
		else 
			messageInput->standartUpdate(keyCode1, keyCode2);
	}

	static void toChatButton(ProgramState* program, int keyCode1, int keyCode2)
	{
		if (keyCode1 == 13)
		{
			std::string name = dynamic_cast<Button*>(program->controls[program->currentControl])->getText();
			
			Client::sendData(68, Client::getClientName() + ' ' + name + ' ');
			Client::setRecipientName(name);

			Program::setCurrentState("Chat");
		}
	}



	static void startMenuServerUpdate(ProgramState* program, unsigned short functionCode, std::string& data) { }

	static void usersMenuServerUpdate(ProgramState* program, unsigned short functionCode, std::string& data)
	{
		switch (functionCode)
		{
		case 67:
		{
			bool online = data[0] == '1';
			data = data.substr(1);
			auto clientButton = program->getControl<Button>(data);

			if (clientButton == nullptr)
			{
				Button* newClientButton;

				if (online)
					newClientButton = new Button(program, data, Console::Color::blackDarkGreen, Console::Color::whiteDarkGreen, UpdateFunctions::toChatButton);
				else
					newClientButton = new Button(program, data, Console::Color::blackGrey, Console::Color::whiteGrey, UpdateFunctions::toChatButton);

				program->addControl(newClientButton, program->getControlsCount(), data);
			}
			else
			{
				if (online)
					clientButton->setColor(Console::Color::blackDarkGreen, Console::Color::whiteDarkGreen);
				else
					clientButton->setColor(Console::Color::blackGrey, Console::Color::whiteGrey);
			}
			break;
		}
		}
	}

	static void chatServerUpdate(ProgramState* program, unsigned short functionCode, std::string& data)
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

			Button* messageButton;
			if (sender == Client::getClientName())
				messageButton = new Button(program, "You: " + text, Console::Color::blackBlue, Console::Color::whiteBlue, UpdateFunctions::emptyUpdate);
			else
				messageButton = new Button(program, "     " + text, Console::Color::blackGrey, Console::Color::whiteGrey, UpdateFunctions::emptyUpdate);

			program->addControl(messageButton, program->getControlsCount() - 2);

			break;
		}
	}
};