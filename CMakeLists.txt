cmake_minimum_required(VERSION 3.10)
project(Server)

SET(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_CXX_STANDARD 14)

add_executable(Server main.cpp ThreadPool.h TcpSocket.cpp TcpSocket.h TcpServer.cpp TcpServer.h user_data.h MyServer.cpp MyServer.h)
