#pragma once
#define ushort unsigned short
#define uchar unsigned char

#include <string>
#include <algorithm>
#include <iostream>

/*
	╔══════════════════════════════════════════════════════════════════════════════╦═══════════════════════════════╗
	║                                   Header                                     ║            PDU				   ║
	╠══════════════════╦══════════════════╦═════════════════════════╦══════════════╬══════════════════════╦════════╣
	║  id транзакции   ║   id протокола   ║ длина остального пакета ║ адрес слейва ║    код функции		  ║  Data  ║
	╠══════════════════╬══════════════════╬═════════════════════════╬══════════════╬══════════════════════╬════════╣
	║     2 байта      ║     2 байта      ║         2 байта         ║    1 байт    ║       1 байт         ║    N   ║
	╚══════════════════╩══════════════════╩═════════════════════════╩══════════════╩══════════════════════╩════════╝
*/

// + 65 - Отправка имени пользователя	
// + 66 - Отправка пустого пакета, для проверки онлайна на сервере
// + 67 - Получение с сервера информацию об именах пользователей (создание кнопок для перехода к переписке)
// + 68 - Отправка на сервер двух имён клиентов для получения переписки
// + 69 - Получение с сервера переписки между двумя клиентами (клиент получает эти пакеты, после отправки пакет с кодом 68)
//   70 - Отправка сообщения на сервер (если собеседник онлайн -> отправить)

namespace modbus
{
	const ushort infoLength = 8U;
	const ushort maxDataLength = 247U;

	void makePacket(const uchar functionCode, const char* data, char* outPacket)
	{
		ushort length = strlen(data) <= maxDataLength ? strlen(data) : maxDataLength;
		ushort offset = 0;

		ushort transactionId = 0;
		memcpy(outPacket + offset, &transactionId, sizeof(transactionId));
		offset += sizeof(transactionId);

		ushort protocolId = 0;
		memcpy(outPacket + offset, &protocolId, sizeof(protocolId));
		offset += sizeof(protocolId);

		memcpy(outPacket + offset, &length, sizeof(length));
		offset += sizeof(length);

		uchar unitID = 0;
		memcpy(outPacket + offset, &unitID, sizeof(unitID));
		offset += sizeof(unitID);

		memcpy(outPacket + offset, &functionCode, sizeof(functionCode));
		offset += sizeof(functionCode);

		memcpy(outPacket + offset, data, length);

		outPacket[infoLength + length] = '\0';
	}

	void parsePacket(const char* packet, ushort* outFunctionCode, char*& data)
	{
		ushort length;
		memcpy(&length, packet + 4, sizeof(length));

		uchar functionCode;
		memcpy(&functionCode, packet + 7, sizeof(functionCode));

		*outFunctionCode = static_cast<ushort>(functionCode);

		data = new char[length + 1];
		memcpy(data, packet + infoLength, sizeof(char) * length);
		data[length] = '\0';
	}

	void parsePacket(const char* packet, ushort* outFunctionCode, std::string& outData)
	{
		ushort length;
		memcpy(&length, packet + 4, sizeof(length));

		uchar functionCode;
		memcpy(&functionCode, packet + 7, sizeof(functionCode));

		*outFunctionCode = static_cast<ushort>(functionCode);

		char* data = new char[length];
		memcpy(data, packet + infoLength, sizeof(char) * length);

		outData = std::string(data, length);
	}

	void makePacket(const uchar functionCode, const char* data, std::string& packet)
	{
		char outPacket[MAXBYTE];

		ushort length = strlen(data) <= maxDataLength ? strlen(data) : maxDataLength;
		ushort offset = 0;

		ushort transactionId = 0;
		memcpy(outPacket + offset, &transactionId, sizeof(transactionId));
		offset += sizeof(transactionId);

		ushort protocolId = 0;
		memcpy(outPacket + offset, &protocolId, sizeof(protocolId));
		offset += sizeof(protocolId);

		memcpy(outPacket + offset, &length, sizeof(length));
		offset += sizeof(length);

		uchar unitID = 0;
		memcpy(outPacket + offset, &unitID, sizeof(unitID));
		offset += sizeof(unitID);

		memcpy(outPacket + offset, &functionCode, sizeof(functionCode));
		offset += sizeof(functionCode);

		memcpy(outPacket + offset, data, length);

		packet = std::string(outPacket, infoLength + length);
	}
}