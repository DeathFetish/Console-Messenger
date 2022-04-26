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
#include "Program/ProgramState.h"
#include "Program/Program.h"
#include "../../ModbusPacket/ModbusPacket.h"

std::queue<std::pair<int, std::string>> packets;
std::queue<std::pair<int, int>> inputs;
Program* program;

void inputHandler()
{
	while (true)
	{
		int first = _getch();
		int second = 0;

		//	std::cout << first;
		//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		if (first == 224)
		{
			second = _getch();
		//	std::cout << second;
		//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		inputs.push(std::make_pair(first, second));
	}
}

void serverHandler()
{

	while (true)
	{
		while (client::isAuthorized)
		{
			char buffer[MAXBYTE] = { 0 };

			if (recv(client::clientSocket, buffer, MAXBYTE, NULL) == -1);
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
	program = new Program(nullptr);

	std::thread inputThread(inputHandler);
	inputThread.detach();

	std::thread serverThread(serverHandler);
	serverThread.detach();
	
	program->print();

	while (!program->shouldClose())
	{
		bool needPrint = false;

		while (packets.size() > 0)
		{
			auto pair = packets.front();
			packets.pop();

			program->update(pair.first, pair.second);
			needPrint = true;
		}

		if (inputs.size() != 0)
		{
			auto pairCode = inputs.front();
			inputs.pop();

			program->update(pairCode.first, pairCode.second);
			needPrint = true;
		}

		if (needPrint)
			program->print();

		if (client::isAuthorized)
			client::timeUpdate();
	}

	client::terminate();
	return 0;
}