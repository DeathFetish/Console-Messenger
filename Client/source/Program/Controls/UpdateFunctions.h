#pragma once
#include "../ProgramState.h"

class UpdateFunctions
{
public:
	static void emptyUpdate(ProgramState* program, int keyCode1, int keyCode2) {}

	static void exitButtonUpdate(ProgramState* program, int keyCode1, int keyCode2)
	{
		if (keyCode1 == 13)
		{
			if (program->prevState == nullptr)
				program->needStopRunning = true;
			else
				program->prevState->currentState = "";
		}
	}

	static void continueToUsersMenuButtonUpdate(ProgramState* program, int keyCode1, int keyCode2)
	{
		if (keyCode1 == 13)
		{
			program->currentState = "UsersMenu";
		}
	}

	static void loginInputUpdate(ProgramState* program, int keyCode1, int keyCode2)
	{
		auto loginInput = dynamic_cast<InputField*>(program->getControl("LoginInput"));
		
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

			std::string error(client::init());
			if (error != "")
			{
				loginInput->setError(error);
				return;
			}

			std::string packet;
			modbus::makePacket(65, name.c_str(), packet);

			loginInput->clearError();
			loginInput->clearInput();
			program->currentControl = 0;

			client::clientName = name;
			client::isAuthorized = true;

			program->currentState = "UsersMenu";

			send(client::clientSocket, packet.c_str(), packet.length(), NULL);
			return;
		}
		default: loginInput->standartUpdate(keyCode1, keyCode2); break;
		}
	}

	static void messageInputUpdate(ProgramState* program, int keyCode1, int keyCode2)
	{
		auto messageInput = dynamic_cast<InputField*>(program->getControl("MessageInput"));
		
		if (keyCode1 == 13)
		{
			if (messageInput->getInput() == "")
				return;

			std::string packet;
			std::string data = client::clientName + ' ' + client::recipientName + ' ' + messageInput->getInput();

			program->addButton("", "You: " + messageInput->getInput(), Console::Color::blackBlue, Console::Color::whiteBlue, UpdateFunctions::emptyUpdate, 2);

			messageInput->clearInput();

			modbus::makePacket(70, data.c_str(), packet);
			send(client::clientSocket, packet.c_str(), packet.length(), NULL);

			program->currentControl++;
			if (program->currentControl + 1 > program->firstPrintableControl + Console::getHeight() - 1)
				program->firstPrintableControl++;
		}
		else 
			messageInput->standartUpdate(keyCode1, keyCode2);
	}

	static void exitChatButtonUpdate(ProgramState* program, int keyCode1, int keyCode2)
	{

	}

	static void toChatButton(ProgramState* program, int keyCode1, int keyCode2)
	{
		if (keyCode1 == 13)
		{
			std::string name = dynamic_cast<Button*>(program->controls[program->currentControl])->getText();
			
			std::string packet;
			std::string data = client::clientName + ' ' + name + ' ';
			modbus::makePacket(68, data.c_str(), packet);
			send(client::clientSocket, packet.c_str(), packet.length(), NULL);

			program->currentState = "Chat";
			client::recipientName = name;
		}
	}
	
	static void startMenuServerUpdate(ProgramState* program, unsigned short functionCode, std::string& data)
	{
		switch (functionCode)
		{
		case 67:
			program->nextStates["UsersMenu"]->update(functionCode, data);
			break;
		case 69:
			program->nextStates["UsersMenu"]->update(functionCode, data);
			break;
		}
	}

	static void usersMenuServerUpdate(ProgramState* program, unsigned short functionCode, std::string& data)
	{
		switch (functionCode)
		{
		case 67:
		{
			program->addButton(std::string(data), std::string(data), Console::Color::blackGrey, Console::Color::whiteGrey, UpdateFunctions::toChatButton);
			break;
		}
		case 69:
		{
			size_t separator = data.find_first_of(' ');
			std::string sender = data.substr(0, separator);

		//	if (currentState == "Chat" && (sender == (dynamic_cast<Chat*>(nextStates["Chat"]))->getRecipeint() || sender == client::clientName)) // переделать!!!
		//		nextStates[currentState]->update(functionCode, data);
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

			if (sender == client::clientName)
				program->addButton("", "You: " + text, Console::Color::blackBlue, Console::Color::whiteBlue, UpdateFunctions::emptyUpdate, 2);
			else
				program->addButton("", "     " + text, Console::Color::blackGrey, Console::Color::whiteGrey, UpdateFunctions::emptyUpdate, 2);
			break;
		}
	}
};