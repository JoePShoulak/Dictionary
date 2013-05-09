echoserv: echoserv.o helper.o
	g++ -g -O2 -Wall -std=c++11 -o echoserv echoserv.o helper.o
