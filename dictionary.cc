#include <sys/socket.h> /* socket definitions */
#include <sys/types.h>  /* socket types       */
#include <arpa/inet.h>  /* inet (3) funtions  */
#include <unistd.h>     /* UNIX functions     */ // TODO: Find out what these are
#include <string.h>     /* strings            */
#include <stdlib.h>     /* exit functions     */
#include <stdio.h>      /* fprintf            */ // TODO: Find out iostream vs stdio
#include <iostream>
#include <fstream>

#ifndef HELPER_H
#define HELPER_H

void Crash(char errorType[]);

#endif

#include "helper.h"

using std::string;
using std::ifstream;
using namespace std;


#define DEFAULT_PORT          (1123)

int main(int argc, char *argv[]) {
  int       list_s;                /* listening socket          */
  int       conn_s;                /* connection socket         */
  short int port;                  /* port number               */
  struct    sockaddr_in servaddr;  /* socket address structure  */
  char     *endptr;                /* for strtol()              */

  /* Get port number from the command line, and
     set to default port if no arguments were supplied */

  if ( argc == 2 )
  {
    port = strtol(argv[1], &endptr, 0);
    if ( *endptr ) 
    {
      Crash("port number");
    }
  }
  else if ( argc < 2 )
  {
    port = DEFAULT_PORT;
  }
  else
  {
    Crash("argument");
  }
	
  /* Create the listening socket */

  if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
  {
    Crash("listening socket");
  }

  /* Set all bytes in socket address structure to
     zero, and fill in the relevant data members */

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(port);

  /* Bind our socket addresss to the
     listening socket, and call listen() */

  int on = 1;
  setsockopt(list_s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 )
  {
    Crash("socket bind");
  }

  if ( listen(list_s, (1024)) < 0 )
  {
    Crash("socket listen");
  }
    
  /* Enter an infinite loop to respond
     to client requests and echo input */

  char lolcat[1024];
  while (1)
  {

    /* Wait for a connection, then accept() it */

    if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 )
    {
      Crash("socket accept");
    }

    memset(&lolcat, 0, sizeof(lolcat));   /* So THAT'S what memset is for...            */

    int amt = read(conn_s, lolcat, 1024); /* write the data from the client to 'lolcat' */

    /* TODO: move the following code to an external file,
       maybe even one for each letter in the alphabet.
       Also, incorporate a dictionary library             */

    string line;
    ifstream myfile ("definitions.txt");
    if (myfile.is_open())
    {
      while ( myfile.good() )
      {
        getline (myfile,line);
        cout << line << endl;
      }
      myfile.close();
    }
    else Crash("dictionary");

    if ( close(conn_s) < 0 )
    {
      Crash("socket close");
    }
  }
}
