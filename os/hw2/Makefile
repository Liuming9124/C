# 編譯環境
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
# 目標
TARGETS = server client
# 依賴檔案
HEADERS = Socket.h

all: $(TARGETS)

server: server.o
	$(CXX) $(CXXFLAGS) -o server server.o

client: client.o
	$(CXX) $(CXXFLAGS) -o client client.o

server.o: server.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c server.cpp

client.o: client.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c client.cpp

clean:
	rm -f *.o $(TARGETS)
