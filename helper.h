#include <string>    /*  for strings       */
#include <stdio.h>   /*  for I/O           */
#include <stdlib.h>  /*  for good measure  */
#include <iostream>  /*  for file reading  */
#include <fstream>   /*  also for files    */
#include <sstream>   /*  i to s converion  */

#define CRASH exit(EXIT_FAILURE)

using std::ifstream; // file I/O
using std::stringstream; // i to s conversions
using namespace std; // strings and whatnot

string IntToString(int toS) // converts an int to a c++ string
{
  stringstream ss;
  ss << toS;
  string number( ss.str() );
  return number;
}

string Bold(string toBold)
{
  ostringstream bolded;
  bolded << "\x1b[1m" << toBold << "\x1b[0m";
  return bolded.str();
}

string Strip(string word, int amount)  // remove the last x characters
{
  return word.substr(0, (word.length() - amount) );
}

int Send(int socket, string word) // wrapper for write
{
  return write(socket, ( "%s", word.data() ), word.length() );
}

string ErrorLookup(int errorNum) // looks up errors based off num code
{
  switch(errorNum)
  {
    case 10:                       // ------ sockety errors ------ //
      return "Port number";
    case 11:
      return "Listening socket";
    case 12:
      return "Socket bind";
    case 13:
      return "Socket listen";
    case 14:
      return "Socket accept";
    case 15:
      return "Socket close";
    case 20:                       // -------- CLI errors -------- //
      return "Argument";
    case 21:
      return "Dictionary read";
    case 30:                       // ----- dictionary error ----- //
      return "No definition";
    default:                       // ------- other errors ------- //
      return "Unknown";
  }
}

string Error(int errorNum, int send=1, int fatal=1)  // reports error
{
  string errorType = ErrorLookup(errorNum);
  ostringstream message;
  message << errorType << " error";
  if (fatal)
  {
    message << " (fatal)";
  }
  message << ". (code " << string( IntToString(errorNum) ) << ")\n";
  if (send)
  {
    cout << message.str();
  }
  if (fatal)
  {
    CRASH;
  }
  return message.str();
}

void Define(char toDefine[], int sock)  // go get the definition
{
  string input(toDefine);
  input = Strip(input, 2);  // strip off the end
  string line;
  ifstream myfile ("definitions.txt");  // open the file
  if (myfile.is_open())
  {
    int count = 0;
    while ( myfile.good() )
    {
      getline(myfile,line);
      int pos1 = line.find("!@#$");
      string word(line.substr(0,pos1));  // parse for the word
      if (input.compare(word) == 0)  // if it matches the input...
      {
        count++;
        string rest = line.substr(pos1+4);
        int pos2 = rest.find("!@#$");
        string wordType(rest.substr(0,pos2));   // parse for type
        string wordDef(rest.substr(pos2+4));    // parse for definition
        string word = "  " + input + " ";       // word
        string theRest = "("  + wordType + "): ";  // word type
        theRest += wordDef + "\n";                 // word definition
        Send(sock, ( Bold(word) + theRest ) );
      }
    }
    if (count == 0)  // If no matches have been found...
    { 
      Send(sock, Error(30, 0, 0) ); // no definition
    }
    myfile.close();
  }
  else
  {
    Error(21, 1); // dictionary
  }
}
