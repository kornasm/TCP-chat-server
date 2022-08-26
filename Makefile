FLAGS = -Wall -Wextra -Wshadow -Wconversion -Wpedantic
SOURCES = string_functions.cpp client_thread.cpp server_thread.cpp
OBJECTS = ${SOURCES:.cpp=.o}

all: build

build: main.cpp $(OBJECTS)
	g++ -o main.out $(FLAGS) main.cpp $(OBJECTS) -lpthread

%.o: %.cpp
	g++ $(FLAGS) -o $@ -c $< -lpthread

clean:
	rm *.o *.out

.PHONY: clean

