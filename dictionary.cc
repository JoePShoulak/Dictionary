#include <sys/socket.h>  // socket def
#include <sys/types.h>   // socket types
#include <arpa/inet.h>   // inet funtions
#include <unistd.h>      // UNIX functions
#include <string.h>      // strings
#include <stdlib.h>      // standard
#include <iostream>      // for file I/O
#include <stdio.h>       // for std I/O
#include <fstream>       // also for files
#include <sstream>       // i to s converion

using std::string;
using std::ifstream;
using std::stringstream;
using namespace std;

// Wrappers 
string IntToString(int to_string)  // converts an int to a c++ string
{
  stringstream stream;
  stream << to_string;
  string number( stream.str() );
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

int Send(int socket, string word)  // wrapper for write
{
  return write(socket, ( "%s", word.data() ), word.length() );
}

// User-defined functions

string ErrorLookup(int errorNum)  // looks up errors based off num code
{
  switch(errorNum)
  {
    case 10:                      // ------ sockety errors ------ //
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
    case 20:                      // -------- CLI errors -------- //
      return "Argument";
    case 21:
      return "Dictionary read";
    case 30:                      // ----- dictionary error ----- //
      return "No definition";
    default:                      // ------- other errors ------- //
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
    exit(EXIT_FAILURE);
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
        string wordType(rest.substr(0,pos2));      // parse for type
        string wordDef(rest.substr(pos2+4));       // parse for definition
        string word = "  " + input + " ";          // word
        string theRest = "("  + wordType + "): ";  // word type
        theRest += wordDef + "\n";                 // word definition
        Send(sock, ( Bold(word) + theRest ) );
      }
    }
    if (count == 0)  // If no matches have been found...
    { 
      Send(sock, Error(30, 0, 0) );  // no definition
    }
    myfile.close();
  }
  else
  {
    Error(21, 1);  // dictionary
  }
}


// main

int main(int argc, char *argv[]) {
  int       list_s;                // listening socket
  int       conn_s;                // connection socket
  short int port;                  // port number
  struct    sockaddr_in servaddr;  // socket add structure
  char     *endptr;                // for strtol()

  // Get port number from the command line, and set
  // to default port if no arguments were supplied

  if ( argc == 2 )
  {
    port = strtol(argv[1], &endptr, 0);
    if ( *endptr ) 
    {
      Error(10);  // port number
    }
  }
  else if ( argc < 2 )
  {
    port = 1123;
  }
  else
  {
    Error(20);  // argument
  }

  // Create the listening socke

  if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
  {
    Error(11);  // listening socket
  }

  // Set all bytes in socket address structure to
  // zero, and fill in the relevant data member

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(port);

  // Bind our socket addresss to the
  // listening socket, and call listen()

  int on = 1;
  setsockopt(list_s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 )
  {
    Error(12);  // socket bind
  }

  if ( listen(list_s, (1024)) < 0 )
  {
    Error(13);
  }
    
  // Enter an infinite loop to respond
  // to client requests and echo input

  char word[1024];
  while (1)
  {

    // Wait for a connection, then accept() it

    if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 )
    {
      Error(14);  // socket accept
    }

    memset(&word, 0, sizeof(word));  // empty out word
    read(conn_s, word, 1024);  // write the input to 'word'
    
    Define(word, conn_s);  // main lookup and print 

    if ( close(conn_s) < 0 )
    {
      Error(15);  // socket close
    }
  }
}
