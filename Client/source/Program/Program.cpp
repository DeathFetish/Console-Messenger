//#pragma once
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#include <winsock2.h>
//#pragma comment (lib, "ws2_32.lib")
//
//#include "Program.h"
//
//#include "ProgramState.h"
//#include "Controls/UpdateFunctions.h"
//#include "../Client.h"
//#include "../../../ModbusPacket/ModbusPacket.h"
//
//
//void Program::inputHandler()
//{
//	while (true)
//	{
//		int first = _getch();
//		int second = 0;
//
//		if (first == 224)
//			second = _getch();
//
//		inputs.push(std::make_pair(first, second));
//	}
//}
//
//void Program::serverHandler()
//{
//	while (true)
//	{
//		while (Client::isAuthorized())
//		{
//			char buffer[MAXBYTE] = { 0 };
//
//			if (recv(Client::getSocket(), buffer, MAXBYTE, NULL) == -1);
//			{
//				std::string outData;
//				unsigned short functionCode;
//				modbus::parsePacket(buffer, &functionCode, outData);
//				packets.push(std::make_pair(functionCode, outData));
//			}
//		}
//	}
//}
//
//void Program::init()
//{
//	Console::init();
//
//	auto startMenu = addProgramState("StartMenu");
//	startMenu->setServerUpdate(UpdateFunctions::startMenuServerUpdate);
//	startMenu->addControl(new InputField(startMenu, "Login: ", 20, Console::Color::blackWhite, Console::Color::whiteBlack, UpdateFunctions::loginInputUpdate), "LoginInput");
//	startMenu->addControl(new Button(startMenu, "Exit", Console::Color::blackWhite, Console::Color::whiteBlack, UpdateFunctions::mainExitButtonUpdate), "ExitButton");
//
//	auto usersMenu = addProgramState("UsersMenu");
//	usersMenu->setServerUpdate(UpdateFunctions::usersMenuServerUpdate);
//	usersMenu->addControl(new Button(usersMenu, "Exit", Console::Color::blackWhite, Console::Color::whiteBlack, UpdateFunctions::usersMenuExitButtonUpdate), "ExitButton");
//
//	auto chat = addProgramState("Chat");
//	chat->setServerUpdate(UpdateFunctions::chatServerUpdate);
//	chat->addControl(new InputField(chat, "You: ", 50, Console::Color::blackWhite, Console::Color::whiteBlack, UpdateFunctions::messageInputUpdate), "MessageInput");
//	chat->addControl(new Button(chat, "Exit", Console::Color::blackWhite, Console::Color::whiteBlack, UpdateFunctions::exitChatButtonUpdate), "ExitButton");
//
//	std::thread inputThread(inputHandler);
//	inputThread.detach();
//
//	std::thread serverThread(serverHandler);
//	serverThread.detach();
//}
//
//ProgramState* Program::addProgramState(const std::string stateName)
//{
//	ProgramState* programState = new ProgramState();
//	programStatesMap.emplace(std::make_pair(stateName, programState));
//	return programState;
//}
//
//void Program::deleteProgramState(const std::string stateName)
//{
//	programStatesMap.erase(programStatesMap.find(stateName));
//}
//
//void Program::setCurrentState(const std::string stateName)
//{
//	if (stateName == "Exit")
//		terminate();
//
//	if (programStatesMap.find(stateName) != programStatesMap.end())
//		currentState = stateName;
//}
//
//void Program::update()
//{
//	bool needPrint = false;
//
//	while (packets.size() > 0)
//	{
//		auto pair = packets.front();
//		packets.pop();
//
//		programStatesMap[currentState]->update(pair.first, pair.second);
//		needPrint = true;
//	}
//
//	if (inputs.size() != 0)
//	{
//		auto pairCode = inputs.front();
//		inputs.pop();
//
//		programStatesMap[currentState]->update(pairCode.first, pairCode.second);
//		needPrint = true;
//	}
//
//	if (needPrint)
//		programStatesMap[currentState]->print();
//
//	Client::timeUpdate();
//}
//
//void Program::terminate()
//{
//	for (auto it = programStatesMap.begin(); it != programStatesMap.end(); ++it)
//		delete it->second;
//	exit(0);
//}
//
//std::map<std::string, ProgramState*> Program::programStatesMap;
//std::string Program::currentState;