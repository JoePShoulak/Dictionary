echoserv: echoserv.o helper.o
	gcc -o echoserv echoserv.o helper.o -Wall

echoserv.o: echoserv.c helper.h
	gcc -o echoserv.o echoserv.c -c -ansi -Wall

helper.o: helper.c helper.h
	gcc -o helper.o helper.c -c -ansi -Wall









