CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -g

SERVER_OBJS := server.o Epoll.o Channel.o Socket.o InetAddress.o util.o
CLIENT_OBJS := client.o Socket.o InetAddress.o util.o

DEPS := $(SERVER_OBJS:.o=.d) $(CLIENT_OBJS:.o=.d)

.PHONY: all clean

all: server client

server: $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

client: $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	$(RM) server client *.o *.d