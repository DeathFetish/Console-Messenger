cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

set(project_name Client)

project(${project_name})

add_executable(
	${project_name}
	
	source/main.cpp
	
	source/Console.h
	source/Client.h

	source/Program/ProgramState.h

	source/Program/Program.h
	source/Program/RecipientMenu.h
	source/Program/Chat.h
	source/Program/Controls/Controls.h

	../../ModbusPacket/ModbusPacket.h
)