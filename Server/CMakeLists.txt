cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

set(project_name Server)

project(${project_name})

add_executable(
	${project_name}
	
	source/main.cpp
	source/Server.h
	../../ModbusPacket/ModbusPacket.h
)