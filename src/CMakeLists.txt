set(CMAKE_CXX_FLAGS_INIT "-std=c++11 -Wall -Wextra -Wshadow -Wconversion -Wpedantic")

set(This Chat_funcs)

set(Sources server_thread.cpp client_thread.cpp string_functions.cpp)
set(Headers server_thread.h client_thread.h string_functions.h)

add_library(${This} ${Sources} ${Headers})