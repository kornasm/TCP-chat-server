# TCP-chat-server

Very simple telnet chat server with overcomplicated design.

Formally in cpp, but apart from usage of std::vector the whole project is written in c.

Project created for teaching purposes using posix signals, threads and network sockets, unit tests (google test) and cmake.

## Compilation
```
cmake -B build/
cmake --build build/
```
## Running
```
./build/TCP_chat_server <port_number>
```

## Connecting clients

Use any telnet client to connect to the server.

You can set your name that other people will see by sending `'imie <your name>'` message.

Messages containing `'blockme'` word won't be sent to others.