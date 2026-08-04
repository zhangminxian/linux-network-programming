CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -g -Isrc

SERVER_SRCS := server.cpp \
               src/Server.cpp \
               src/EventLoop.cpp \
               src/Epoll.cpp \
               src/Channel.cpp \
               src/Socket.cpp \
               src/InetAddress.cpp \
               src/util.cpp \
               src/Acceptor.cpp \
               src/Connection.cpp

CLIENT_SRCS := client.cpp \
               src/Socket.cpp \
               src/InetAddress.cpp \
               src/util.cpp

.PHONY: all clean

all: server client

server: $(SERVER_SRCS)
	$(CXX) $(CXXFLAGS) $(SERVER_SRCS) -o server

client: $(CLIENT_SRCS)
	$(CXX) $(CXXFLAGS) $(CLIENT_SRCS) -o client

clean:
	$(RM) server client