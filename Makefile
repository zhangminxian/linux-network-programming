CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra

COMMON_OBJS := Socket.o InetAddress.o util.o
SERVER_OBJS := server.o Epoll.o $(COMMON_OBJS)
CLIENT_OBJS := client.o $(COMMON_OBJS)

.PHONY: all clean

all: server client

server: $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

client: $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

server.o: Epoll.h InetAddress.h Socket.h util.h
client.o: InetAddress.h Socket.h
Epoll.o: Epoll.h util.h
Socket.o: Socket.h InetAddress.h util.h
InetAddress.o: InetAddress.h
util.o: util.h

clean:
	rm -f server client *.o
