#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <chrono>
#include <string>
#pragma comment (lib, "ws2_32.lib")


class Client
{
private:
	static WSADATA wsaData;
	static SOCKET clientSocket;
	static sockaddr_in sockAddr;

	static SOCKADDR clientAddr;
	static std::string clientName;
	static std::string recipientName;

	static std::chrono::steady_clock::time_point previousTime;
	static std::chrono::steady_clock::time_point currentTime;

	static bool authorized;
	static double updatePeriod;
	static double deltaTime;

public:

	static std::string init();

	static void terminate();

	static void timeUpdate();

	static bool isAuthorized();
	static void setClientName(std::string& name);
	static void setRecipientName(std::string& name);

	static void enableAuthorized(bool enable);

	static SOCKET getSocket();
	static std::string& getClientName();
	static std::string& getRecipientName();

	static void sendData(unsigned short functionCode, std::string data);
};