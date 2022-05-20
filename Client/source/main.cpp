#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>

#include <chrono>
#include <vector>
#include <thread>
#include <string>
#include <fstream>
#include <queue>
#include <map>
#pragma comment (lib, "ws2_32.lib")

#include "Console.h"
#include "Client.h"
#include "Program/Program.h"
#include "Program/ProgramState.h"

#include "Program/Controls/Controls.h"
#include "Program/Controls/InputField.h"
#include "Program/Controls/Button.h"
#include "Program/Controls/UpdateFunctions.h"

#include "../../ModbusPacket/ModbusPacket.h"

std::queue<std::pair<int, std::string>> packets;
std::queue<std::pair<int, int>> inputs;

void inputHandler()
{
	while (true)
	{
		int first = _getch();
		int second = 0;

		if (first == 224)
			second = _getch();

		inputs.push(std::make_pair(first, second));
	}
}

void serverHandler()
{
	while (true)
	{
		while (Client::isAuthorized())
		{
			char buffer[MAXBYTE] = { 0 };

			if (recv(Client::getSocket(), buffer, MAXBYTE, NULL) == -1);
			{
				std::string outData;
				unsigned short functionCode;
				modbus::parsePacket(buffer, &functionCode, outData);
				packets.push(std::make_pair(functionCode, outData));
			}
		}
	}
}

int main()
{
	Console::init();

	auto startMenu = Program::addProgramState("StartMenu");
	
	startMenu->setServerUpdate(UpdateFunctions::startMenuServerUpdate);
	auto loginInput = new InputField(startMenu, "Login: ", 20, Console::Color::blackWhite, Console::Color::whiteBlack, UpdateFunctions::loginInputUpdate);
	startMenu->addControl(loginInput, startMenu->getControlsCount(), "LoginInput");
	auto exitButton = new Button(startMenu, "Exit", Console::Color::blackWhite, Console::Color::whiteBlack, UpdateFunctions::mainExitButtonUpdate);
	startMenu->addControl(exitButton, startMenu->getControlsCount(), "ExitButton");

	auto usersMenu = Program::addProgramState("UsersMenu");
	usersMenu->setServerUpdate(UpdateFunctions::usersMenuServerUpdate);
	auto exitUsersMenuButton = new Button(usersMenu, "Exit", Console::Color::blackWhite, Console::Color::whiteBlack, UpdateFunctions::usersMenuExitButtonUpdate);
	usersMenu->addControl(exitUsersMenuButton, usersMenu->getControlsCount(), "ExitButton");

	auto chat = Program::addProgramState("Chat");
	chat->setServerUpdate(UpdateFunctions::chatServerUpdate);
	auto messageInput = new InputField(chat, "You: ", 50, Console::Color::blackWhite, Console::Color::whiteBlack, UpdateFunctions::messageInputUpdate);
	chat->addControl(messageInput, chat->getControlsCount(), "MessageInput");
	auto exitChatButton = new Button(chat, "Exit", Console::Color::blackWhite, Console::Color::whiteBlack, UpdateFunctions::exitChatButtonUpdate);
	chat->addControl(exitChatButton, chat->getControlsCount(), "ExitButton");

	Program::setCurrentState("StartMenu");
	Program::print();

	std::thread inputThread(inputHandler);
	inputThread.detach();

	std::thread serverThread(serverHandler);
	serverThread.detach();

	while (true)
	{
		bool needPrint = false;

		while (packets.size() > 0)
		{
			auto pair = packets.front();
			packets.pop();

			Program::update(pair.first, pair.second);
			needPrint = true;
		}

		if (inputs.size() != 0)
		{
			auto pairCode = inputs.front();
			inputs.pop();

			Program::update(pairCode.first, pairCode.second);
			needPrint = true;
		}

		if (Program::getCurrentState() == "Exit") break;

		if (needPrint)
			Program::print();

		Client::timeUpdate();
	}

	Program::terminate();
	Client::terminate();

	return 0;
}