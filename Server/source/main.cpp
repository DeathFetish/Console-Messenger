#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <chrono>
#include <thread>
#pragma comment (lib, "ws2_32.lib")

#include "Server.h"
#include "../../ModbusPacket/ModbusPacket.h"

//	Принять клиента -> Разослать всем клиентам информацию о подключении первого
//	Клиент переходит к переписке -> Отослать клиенту информацию о всех клиентах (Онлайн-Oфлайн)
//	Клиент выходит -> Отослать информацию о том что клиент оффлайн
//	Клиент заходит в переписку -> Отослать все сообщения (Если они были) 
//	Клиент отправляет сообщение -> Сохранить сообщение на сервере, (Если собеседник в сети, отправить его)

//	Файл с пользователями
//	Папка с файлами всех переписок

//	Таймер на каждого пользователя, если пакт не приходит в течении 10 сек, то пользователь офлайн

int main(int argc, char** argv)
{	
	setExecutablePath(argv[0]);

	if (!server::init())
		server::terminate();

	std::thread threadsHandler(server::threadKiller);
	threadsHandler.detach();

	SOCKET clientSocket;
	SOCKADDR clientAddr;
	int nSize = sizeof(SOCKADDR);

	while (true)
	{
		clientSocket = accept(server::serverSocket, (SOCKADDR*)&clientAddr, &nSize);
		server::threads.push_back(std::thread(server::update, clientSocket, clientAddr));
		server::threads.back().detach();
	}
	
	server::terminate();
	return 0;
}
