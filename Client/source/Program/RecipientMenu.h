#pragma once
#include "ProgramState.h"
#include "Chat.h"
#include "../Client.h"

class RecipientMenu : public ProgramState
{
	std::vector<std::string> chatsUsers;
	std::vector<std::pair<std::string, Button*>> toChat;
	Button* exit;
public:
	RecipientMenu(ProgramState* prevState) : ProgramState(prevState)
	{
		currentState = "";
		exit = new Button(console::getConsoleString("Exit"));
		nextStates.emplace(std::make_pair("Chat", new Chat(this)));
	}

	void update(int keyCode1, int keyCode2) override
	{
		if (!currentState.empty())
		{
			nextStates[currentState]->update(keyCode1, keyCode2);
			return;
		}

		if (keyCode1 == 13)
		{
			if (currentControl == 0)
			{
				for (int i = 0; i < toChat.size(); ++i)
				{
					toChat[i].first.clear();
					delete toChat[i].second;
					toChat.clear();
				}
				client::terminate();
				prevState->setCurrentState("");
			}
			else
			{
				std::string packet;
				std::string data = client::clientName + ' ' + toChat[currentControl - 1].first + ' ';
				modbus::makePacket(68, data.c_str(), packet);
				send(client::clientSocket, packet.c_str(), packet.length(), NULL);
				
				currentState = "Chat";
				((Chat*)(nextStates["Chat"]))->setRecipeint(toChat[currentControl - 1].first);

				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}

		if (keyCode1 == 224)
		{
			switch (keyCode2)
			{
			case 72:
				if (currentControl > 0)
					currentControl--;
				return;
			case 80:
				if (currentControl < toChat.size())
					currentControl++;
				return;
			}
		}
	}

	void update(int functionCode, std::string data) override
	{
		switch (functionCode)
		{
			case 67:
			{
			//	std::cout << data << std::endl;
			//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				Button* button = new Button(console::getConsoleString(data), 8, 248);
				toChat.emplace_back(std::make_pair(data, button));
				break;
			}
			case 69:
			{
			//	std::cout << data << std::endl;
			//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			
			//	currentState = "Chat";
			//	nextStates.emplace(std::make_pair("Chat", new Chat(this)));

				int separator = data.find_first_of(' ');
				std::string sender = data.substr(0, separator);

				if (currentState == "Chat" && (sender == ((Chat*)(nextStates["Chat"]))->getRecipeint() || sender == client::clientName))
					nextStates[currentState]->update(functionCode, data);
			}
		}
	}

	void print()
	{
		if (!currentState.empty())
		{
			nextStates[currentState]->print();
			return;
		}

		system("cls");
		exit->print(currentControl == 0);

		for (auto i = 0; i < toChat.size(); ++i)
		{
			toChat[i].second->print(i + 1 == currentControl);
		}
	}

	void fromChatToMenu()
	{
		delete nextStates[currentState];
		currentState = "";
	}
};