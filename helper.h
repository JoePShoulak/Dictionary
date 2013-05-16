#include <string>    /* for strings          */
#include <stdio.h>   /* for I/O              */
#include <stdlib.h>  /* for good measure     */
#include <iostream>  /* for file reading     */
#include <fstream>   /* TODO: Find out       */
#include <sstream>   /* TODO: Find out       */

/* TODO: Get to work, then include in server */

using std::ifstream;
using std::string;
using std::cout;

int Crash(int errorNum)
{
  string errorType;
  if (errorNum == 10)                 // Normally I'd use braces,
    errorType = "port number";        // but here, it just clutters
  else if (errorNum == 11)            // the code. I'll try to get
    errorType = "listening socket";   // a switch statement working  
  else if (errorNum == 12)            // since this is a perfect
    errorType = "socket bind";        // example of a use for it.
  else if (errorNum == 13)
    errorType = "socket listen";
  else if (errorNum == 14)
    errorType = "socket accept";
  else if (errorNum == 15)
    errorType = "socket close";
  else if (errorNum == 20)
    errorType = "argument";
  else if (errorNum == 21)
    errorType = "dictionary read";
  else
    errorType = "unknown";
  cout << "Fatal " << errorType << " error. (code " << errorNum << ")\n";  
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
    int count = 0;
    while ( myfile.good() )
    {
      getline(myfile,line);
      unsigned pos = line.find("!@#$");
      string word(line.substr(0,pos));
      if (input.compare(word) == 0)
      {
        count++;
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
    if (count == 0)
    {
      write(conn_s, "Word not yet in dictionary.\n", 29);
    }
    myfile.close();
  }
  else
  {
    Crash(21); // dictionary
  }
}

