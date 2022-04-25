#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <chrono>
#include <string>
#pragma comment (lib, "ws2_32.lib")

#include "../../ModbusPacket/ModbusPacket.h"

namespace client
{
	WSADATA wsaData;
	SOCKET clientSocket;
	sockaddr_in sockAddr;

	SOCKADDR clientAddr;
	std::string clientName;

	std::chrono::steady_clock::time_point previousTime;
	std::chrono::steady_clock::time_point currentTime;

	bool isAuthorized = false;
	double updatePeriod = 5000.0;
	double deltaTime;

	std::string init()
	{
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			return std::string("Error! Client initialization failed. Error code: " + std::to_string(WSAGetLastError()));

		if ((clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
			return std::string("Error! Could not create socket: " + std::to_string(WSAGetLastError()));

		memset(&sockAddr, 0, sizeof(sockAddr));
		sockAddr.sin_family = PF_INET;
		sockAddr.sin_addr.s_addr = inet_addr("25.71.104.195");
		sockAddr.sin_port = htons(502);

		if (connect(clientSocket, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == -1)
			return std::string("Error! Can not connect to server: " + std::to_string(WSAGetLastError()));
		
		previousTime = std::chrono::high_resolution_clock::now();
		return "";
	}

	void terminate()
	{
		isAuthorized = false;
		closesocket(clientSocket);
		WSACleanup();
	}

	void timeUpdate()
	{
		currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<double, std::milli>(currentTime - previousTime).count();

		if (deltaTime >= updatePeriod)
		{
			previousTime = currentTime;
			std::string packet;
			modbus::makePacket(66, "", packet);
			send(clientSocket, packet.c_str(), packet.length(), NULL);
		}
	}
}