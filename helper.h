#include <string>    /* for strings          */
#include <stdio.h>   /* for I/O              */
#include <stdlib.h>  /* for good measure     */
#include <iostream>  /* for file reading     */
#include <fstream>   /* TODO: Find out       */
#include <sstream>   /* TODO: Find out       */

using std::ifstream;
using std::string;
using std::cout;

string Strip(string word, int amount)
{
  return word.substr(0, (word.length() - amount) );
}

int Send(int socket, string word)
{
  return write(socket, ( "%s", word.data() ), word.length() );
}


string Error(int errorNum, int send=1, int crash=1)
{
  string errorType;
  if (errorNum == 10)                 // Normally I'd use braces,
    errorType = "Port number";        // but here, it just clutters
  else if (errorNum == 11)            // the code. I'll try to get
    errorType = "Listening socket";   // a switch statement working  
  else if (errorNum == 12)            // since this is a perfect
    errorType = "Socket bind";        // example of a use for it.
  else if (errorNum == 13)
    errorType = "Socket listen";
  else if (errorNum == 14)
    errorType = "Socket accept";
  else if (errorNum == 15)
    errorType = "Socket close";
  else if (errorNum == 20)
    errorType = "Argument";
  else if (errorNum == 21)
    errorType = "Dictionary read";
  else if (errorNum == 30)
    errorType = "No definition";
  else
    errorType = "Unknown";
  string message = "";
  std::stringstream ss;
  ss << errorNum;
  string number(ss.str());
  message += errorType;
  message += " error";
  if (crash)
  {
    message += " (fatal)";
  }
  message += ". (code ";
  message += string( ss.str() );
  message += ")\n";
  if (send)
  {
    cout << message;
  }
  if (crash)
  {
    exit(EXIT_FAILURE);
  }
  return message;
}

void Define(char toDefine[], int sock)
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
        string word = "  " + input + " ";          // word
        string theRest = "("  + wordType + "): ";  // word type &
        theRest += wordDef + "\n";                 // definition
        string bold("\x1b[1m");
        string unbold("\x1b[0m");
        Send(sock, bold);
        Send(sock, word);
        Send(sock, unbold);
        Send(sock, theRest);
      }
    }
    if (count == 0) // If no matches have been found...
    { 
      Send(sock, Error(30, 0, 0) );
      // write(sock, "Word not yet in dictionary.\n", 29);
    }
    myfile.close();
  }
  else
  {
    Error(21, 1); // dictionary
  }
}

