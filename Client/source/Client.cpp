#include "Client.h"
#include <iostream>

#include "../../ModbusPacket/ModbusPacket.h"

std::string Client::init()
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

void Client::terminate()
{
	authorized = false;
	clientName.clear();
	closesocket(clientSocket);
	WSACleanup();
}

void Client::timeUpdate()
{
	if (!authorized)
		return;

	currentTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<double, std::milli>(currentTime - previousTime).count();

	if (deltaTime >= updatePeriod)
	{
		previousTime = currentTime;
		sendData(66, "");
	}
}

void Client::enableAuthorized(bool enable) { authorized = enable; }
bool Client::isAuthorized() { return authorized; }

void Client::setClientName(std::string& name) { clientName = name; }
void Client::setRecipientName(std::string& name) { recipientName = name; }
std::string& Client::getClientName() { return clientName; }
std::string& Client::getRecipientName() { return recipientName; }

SOCKET Client::getSocket() { return clientSocket; }

void Client::sendData(unsigned short functionCode, std::string data)
{
	std::string packet;
	modbus::makePacket(functionCode, data.c_str(), packet);
	send(clientSocket, packet.c_str(), packet.length(), NULL);
}

WSADATA Client::wsaData;
SOCKET Client::clientSocket;
sockaddr_in Client::sockAddr;
SOCKADDR Client::clientAddr;

std::string Client::clientName;
std::string Client::recipientName;

std::chrono::steady_clock::time_point Client::previousTime;
std::chrono::steady_clock::time_point Client::currentTime;

bool Client::authorized = false;
double Client::updatePeriod = 2500.0;
double Client::deltaTime;