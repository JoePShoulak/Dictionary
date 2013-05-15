#include <sys/socket.h> /* socket defs    */
#include <sys/types.h>  /* socket types   */
#include <arpa/inet.h>  /* inet funtions  */
#include <unistd.h>     /* UNIX functions */
#include <string.h>     /* strings        */
#include <stdlib.h>     /* exit functions */
#include <stdio.h>      /* fprintf        */
#include <iostream>     /* file reading   */
 
#ifndef HELPER_H
  #define HELPER_H

  void Crash(char errorType[]);
  void Define(char toDefine[], int sock);

#endif

#include "helper.h"

using std::string;
using std::ifstream;
using namespace std;

#define DEFAULT_PORT          (1123)

int main(int argc, char *argv[]) {
  int       list_s;                /* listening socket      */
  int       conn_s;                /* connection socket     */
  short int port;                  /* port number           */
  struct    sockaddr_in servaddr;  /* socket add structure  */
  char     *endptr;                /* for strtol()          */

  /* Get port number from the command line, and
     set to default port if no arguments were supplied */

  if ( argc == 2 )
  {
    port = strtol(argv[1], &endptr, 0);
    if ( *endptr ) 
    {
      char error[] = "port number";
      Crash(error);
    }
  }
  else if ( argc < 2 )
  {
    port = DEFAULT_PORT;
  }
  else
  {
    char error[] = "argument";
    Crash(error);
  }
	
  /* Create the listening socket */

  if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
  {
    char error[] = "listening socket";
    Crash(error);
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
    char error[] = "socket bind";
    Crash(error);
  }

  if ( listen(list_s, (1024)) < 0 )
  {
    char error[] = "socket listen";
    Crash(error);
  }
    
  /* Enter an infinite loop to respond
     to client requests and echo input */

  char lolcat[1024];
  while (1)
  {

    /* Wait for a connection, then accept() it */

    if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 )
    {
      char error[] = "socket accept";
      Crash(error);
    }

    memset(&lolcat, 0, sizeof(lolcat));   /* So THAT'S what memset is for...            */

    int amt = read(conn_s, lolcat, 1024); /* write the data from the client to 'lolcat' */
    
    /* TODO: Turn the following code into a Lookup
             function, and move to an external file. */
    
    Define(lolcat, conn_s);

    if ( close(conn_s) < 0 )
    {
      char error[] = "socket close";
      Crash(error);
    }
  }
}
