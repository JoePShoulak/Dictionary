echoserv: echoserv.o helper.o
	g++ -o -std=c++11 echoserv echoserv.o helper.o -Wall

echoserv.o: echoserv.cc helper.h
	g++ -o -std=c++11 echoserv.o echoserv.cc -c -Wall

helper.o: helper.c helper.h
	g++ -o -std=c++11 helper.o helper.c -c -Wall









