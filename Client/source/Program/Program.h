#pragma once
#include <map>
#include <queue>
#include <string>
#include <thread>
#include "ProgramState.h"

class Program
{
private:
	static std::map<std::string, ProgramState*> programStatesMap;
	static std::string currentState;

public:

	static ProgramState* addProgramState(const std::string stateName)
	{
		ProgramState* programState = new ProgramState();
		programStatesMap.emplace(std::make_pair(stateName, programState));
		return programState;
	}

	static ProgramState* getProgramState(const std::string stateName)
	{
		auto result = programStatesMap.find(stateName);
		return result != programStatesMap.end() ? result->second : nullptr;
	}

	static void deleteProgramState(const std::string stateName)
	{
		programStatesMap.erase(programStatesMap.find(stateName));
	}

	static void setCurrentState(const std::string stateName)
	{
		currentState = stateName;
	}

	static const std::string& getCurrentState()
	{
		return currentState;
	}

	static void update(int keyCode1, int keyCode2)
	{
		programStatesMap[currentState]->update(keyCode1, keyCode2);
	}

	static void update(int functionCode, std::string& data)
	{
		programStatesMap[currentState]->update(functionCode, data);
	}

	static void terminate()
	{
		for (auto it = programStatesMap.begin(); it != programStatesMap.end(); ++it)
			delete it->second;
	}

	static void print()
	{
		programStatesMap[currentState]->print();
	}
};

std::map<std::string, ProgramState*> Program::programStatesMap;
std::string Program::currentState;