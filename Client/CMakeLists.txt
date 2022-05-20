cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

set(project_name Client)

project(${project_name})

add_executable(
	${project_name}
	
	source/main.cpp
	
	source/Console.h
	source/Client.h
	source/Client.cpp

	source/Program/Controls/Controls.h
	source/Program/Controls/InputField.h
	source/Program/Controls/Button.h
	source/Program/Controls/UpdateFunctions.h


	source/Program/ProgramState.h
	source/Program/Program.h


	../../ModbusPacket/ModbusPacket.h
)