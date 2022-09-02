cmake_minimum_required(VERSION 3.10)

set(This TCP_chat_server)
project(${This} CXX)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(src)

enable_testing()

add_subdirectory(tests)

add_executable(${This} main.cpp)
target_link_libraries(${This} Chat_funcs)
target_link_libraries(${This} pthread)