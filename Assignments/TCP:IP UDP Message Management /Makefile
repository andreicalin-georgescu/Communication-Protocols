all: build

build: server subscriber

server: server.cpp
	g++ -std=c++11 server.cpp -o server

subscriber: subscriber.cpp
	g++ -std=c++11 subscriber.cpp -o subscriber
clean:
	rm server subscriber

