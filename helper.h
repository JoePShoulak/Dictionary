#include <string>    /* for strings          */
#include <stdio.h>   /* for I/O              */
#include <stdlib.h>  /* for good measure     */
#include <iostream>  /* for file reading     */
#include <fstream>   /* TODO: Find out       */
#include <sstream>   /* TODO: Find out       */

/* TODO: Get to work, then include in server */

using std::ifstream;
using std::string;

void Crash(char errorType[])
{
  char errorMessage[80];
  strcpy(errorMessage, "DICTIONARY: Fatal ");
  strcat(errorMessage, errorType);
  strcat(errorMessage, " error.\n");
  fprintf(stderr, errorMessage);
  exit(EXIT_FAILURE);
}

void Define(char toDefine[], int sock)
{
    string input(toDefine);
    int conn_s = sock;
    input = input.substr(0, (input.length()-2));
    string line;
    ifstream myfile ("definitions.txt");
    if (myfile.is_open())
    {
      int it = 0;
      while ( myfile.good() )
      {
        it++;
        getline(myfile,line);
        unsigned pos = line.find("!@#$");
        string word(line.substr(0,pos));
        if (input.compare(word) == 0)
        {
          string rest = line.substr(pos+4);
          int new_pos = rest.find("!@#$");
          string wordType(rest.substr(0,new_pos));
          string wordDef(rest.substr(new_pos+4));
          write(conn_s, "  ", 2);
          write(conn_s, ("%s", wordType.data()), wordType.length());
          write(conn_s, ": ", 2);
          write(conn_s, ("%s", wordDef.data()), wordDef.length());
          write(conn_s, "\n", 1);
        }
      }
      myfile.close();
    }
    else Crash("dictionary");

}

