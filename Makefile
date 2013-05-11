dictionary: dictionary.o helper.cc helper.h
	g++ -g -O2 -Wall -std=c++11 -o dictionary dictionary.o
