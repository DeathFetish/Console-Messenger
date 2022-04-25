#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <filesystem>
#include <winsock2.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <map>

#include <thread>
#include <mutex>

#pragma comment (lib, "ws2_32.lib")

#include "../../ModbusPacket/ModbusPacket.h"

std::string directoryPath;
std::string clientsFilePath;

bool isFileExist(std::string filePath)
{
	bool isExist = false;
	std::ifstream fin(filePath.c_str());

	if (fin.is_open())
		isExist = true;

	fin.close();
	return isExist;
}

void setExecutablePath(const std::string& executablePath)
{
	directoryPath = executablePath;
	for (int i = 0; i < 3; ++i)
	{
		size_t found = directoryPath.find_last_of("/\\");
		directoryPath = directoryPath.substr(0, found);
	}
	directoryPath += "/";
}

namespace server
{
	struct Client
	{
		SOCKET clientSocket;
		SOCKADDR clientAddr;

		Client(SOCKET clientSocket, SOCKADDR clientAddr)
			: clientSocket(clientSocket), clientAddr(clientAddr) {}
	};

	struct MessageInfo
	{
		std::string sender;
		std::string message;

		MessageInfo(std::string& sender, std::string& message)
			: sender(sender), message(message) {}
	};

	std::string getKey(std::string& loginFirst, std::string& loginSecond)
	{
		return (loginFirst > loginSecond) ? loginFirst + " " + loginSecond + " " : loginSecond + " " + loginFirst + " ";
	}
	
	std::string getChatFilePath(std::string& login1, std::string& login2)
	{
		return std::string(directoryPath + "resources/chats" + "\\" + getKey(login1, login2) + ".txt");
	}

	void findLoginsInString(std::string& source, std::string& outLoginFirst, std::string& outLoginSecond)
	{
		size_t separator1 = source.find_first_of(' ');
		size_t separator2 = source.find_first_of(' ', separator1 + 1);
		outLoginFirst = source.substr(0, separator1);
		outLoginSecond = source.substr(separator1 + 1, separator2 - separator1 - 1);
	}

	SOCKET serverSocket;
	WSADATA wsaData;
	sockaddr_in serverAddr;

	std::mutex sendMutex;
	std::mutex fileStreamMutex;

	std::vector<std::thread> threads;
	std::map<std::string, std::vector<MessageInfo>> chats;
	std::map<std::string, Client> onlineClients;
	std::map<std::string, bool> clients;

	void safeSend(SOCKET clientSocket, std::string& packet)
	{
		sendMutex.lock();
		send(clientSocket, packet.c_str(), packet.length(), NULL);
		sendMutex.unlock();
	}

	void safeWriteInFile(std::string& filePath, MessageInfo message)
	{
		fileStreamMutex.lock();

		std::fstream file(filePath, std::ios::app);
		if (file.is_open())
		{
			file << message.sender << " " << message.message << std::endl;
		}
		file.close();

		fileStreamMutex.unlock();
	}

	bool init()
	{
		std::cout << "========================================" << std::endl;
		
		std::string l1 = "sasha";
		std::string l2 = "andrey";
		std::cout << getKey(l1, l2) << std::endl << getKey(l2, l1) << std::endl;
		
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			std::cout << "Error! Server init failed. Error code: " << WSAGetLastError() << std::endl;
			return false;
		}

		if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			std::cout << "Error! Could not create socket: " << WSAGetLastError() << std::endl;
			return false;
		}

		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = PF_INET;
		serverAddr.sin_addr.s_addr = inet_addr("25.71.104.195");
		serverAddr.sin_port = htons(502);
		bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR));

		listen(serverSocket, 20);

		clientsFilePath = directoryPath + "resources/clients.txt";

		if (isFileExist(clientsFilePath))
		{
			std::string str;
			std::ifstream file(clientsFilePath);

			while (getline(file, str))
			{
				clients[str] = false;
			}

			file.close();
		}
		else
		{
			std::ofstream file;
			file.open(clientsFilePath);
			file.close();
		}

		std::string pathToChats = directoryPath + "resources/chats";

		for (auto& chatIt : std::filesystem::directory_iterator(pathToChats))
		{
			std::string chatName = chatIt.path().u8string();
			chatName.erase(chatName.begin(), chatName.begin() + pathToChats.length() + 1);
			chatName.erase(chatName.end() - 4, chatName.end());
			std::string firstLogin, secondLogin;

			findLoginsInString(chatName, firstLogin, secondLogin);

		//	std::cout << firstLogin << std::endl;
		//	std::cout << secondLogin << std::endl;

			std::vector<MessageInfo> messages;

			std::string message;
			std::ifstream file(chatIt);
			
			while (getline(file, message))
			{
				int messageSeparator = message.find_first_of(' ');
				std::string sender = message.substr(0, messageSeparator);
				std::string messageStr = message.substr(messageSeparator + 1, sender.length() - messageSeparator - 1);
				std::cout << sender << "\t" << messageStr << std::endl;
				messages.emplace_back(sender, messageStr);
			}

			chats.emplace(getKey(firstLogin, secondLogin), messages);
			std::cout << "________________________________________" << std::endl;

		}
		std::cout << "========================================" << std::endl;

		return true;
	}

	void terminate()
	{
		closesocket(serverSocket);
		WSACleanup();
	}
	
	void update(SOCKET clientSocket, SOCKADDR clientAddr)
	{
		std::chrono::steady_clock::time_point updateTimePoint;
		
		std::string clientName;
		double maxPeriod = 10000;

		while (true)
		{
			char buffer[MAXBYTE] = { 0 };
			int recvResult;
			if ((recvResult = recv(clientSocket, buffer, MAXBYTE, NULL)) != -1)
			{
				std::string data;
				unsigned short functionCode = 0;
				modbus::parsePacket(buffer, &functionCode, data);
				
			//	std::cout << recvResult << "\t" << functionCode << std::endl;

				switch (functionCode)
				{
					case 65:
					{
						clientName = data;
						onlineClients.emplace(std::make_pair(clientName, Client(clientSocket, clientAddr)));
						updateTimePoint = std::chrono::high_resolution_clock::now();

						if (clients.find(clientName) == clients.end())
						{
							std::fstream file(clientsFilePath, std::ios::app);
							if (file.is_open())
							{
								file << clientName << std::endl;
							}
							file.close();
							clients[clientName] = true;
						}
						
						for (auto it = clients.begin(); it != clients.end(); ++it)
						{
							std::string packet;
							std::cout << it->first.c_str() << std::endl;
						//	std::this_thread::sleep_for(std::chrono::milliseconds(50));

							modbus::makePacket(67, it->first.c_str(), packet);
							safeSend(clientSocket, packet);
						//	send(clientSocket, packet.c_str(), packet.length(), NULL);
						}

						break;
					}
					case 66:
						updateTimePoint = std::chrono::high_resolution_clock::now();
						break;
					case 68:
					{
						std::string firstLogin, secondLogin;
						findLoginsInString(data, firstLogin, secondLogin);

						std::cout << firstLogin << "\t" << secondLogin << std::endl;

						auto chatIt = chats.find(getKey(firstLogin, secondLogin));
						
						if (chatIt != chats.end())
						{
							for (int i = 0; i < chatIt->second.size(); ++i)
							{
								auto& messageInfo = chatIt->second[i];
								std::string packet;
								std::string data = messageInfo.sender + ' ' + messageInfo.message;
								std::cout << data << std::endl;
							//	std::this_thread::sleep_for(std::chrono::milliseconds(50));

								modbus::makePacket(69, data.c_str(), packet);
								
								safeSend(clientSocket, packet);
							//	send(clientSocket, packet.c_str(), packet.length(), NULL);
							}
						}
						break;
					}
					case 70:
					{
						std::string firstLogin, secondLogin;
						server::findLoginsInString(data, firstLogin, secondLogin);

						std::cout << firstLogin << "\t" << secondLogin << std::endl;

						data.erase(data.begin(), data.begin() + firstLogin.length() + 1);
						data.erase(data.begin(), data.begin() + secondLogin.length() + 1);

						MessageInfo message(firstLogin, data);
						safeWriteInFile(server::getChatFilePath(firstLogin, secondLogin), message);
						chats[getKey(firstLogin, secondLogin)].push_back(message);

						auto recipinet = onlineClients.find(secondLogin);
						std::cout << firstLogin << "\t" << secondLogin << std::endl;

						if (recipinet != onlineClients.end() && firstLogin != secondLogin)
						{
							std::string packet;
							std::string data = message.sender + ' ' + message.message;

							modbus::makePacket(69, data.c_str(), packet);
							safeSend(recipinet->second.clientSocket, packet);

						}
					}
				}
			}
			
			std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
			double deltaTime = std::chrono::duration<double, std::milli>(currentTime - updateTimePoint).count();

			if (deltaTime > maxPeriod)
				return;
		}

	}

	void threadKiller()
	{
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		//	std::cout << threads.size() << std::endl;
		}
	}
}