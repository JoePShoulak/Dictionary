echoserv: echoserv.o helper.o
	g++ -g -O2 -Wall -std=c++11 -o echoserv echoserv.o helper.o

echoserv.o: echoserv.cc helper.h
	g++ -g -O2 -Wall -std=c++11 -o echoserv.o echoserv.cc -c

helper.o: helper.c helper.h
	g++ -g -O2 -Wall -std=c++11 -o helper.o helper.c -c









