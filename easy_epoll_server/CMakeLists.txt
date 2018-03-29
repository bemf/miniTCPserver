cmake_minimum_required(VERSION 3.9)
project(easy_epoll_server)

set(CMAKE_CXX_STANDARD 11)

add_executable(easy_epoll_server main.cpp common.cpp common.h)