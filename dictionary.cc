#include <sys/socket.h>  /*  socket defs     */
#include <sys/types.h>   /*  socket types    */
#include <arpa/inet.h>   /*  inet funtions   */
#include <unistd.h>      /*  UNIX functions  */
#include <string.h>      /*  strings         */
 
#ifndef HELPER_H
#define HELPER_H

void Define(char toDefine[], int sock);

#endif

#include "helper.h"

using std::string;
using std::ifstream;
using namespace std;

int main(int argc, char *argv[]) {
  int       list_s;                /*  listening socket      */
  int       conn_s;                /*  connection socket     */
  short int port;                  /*  port number           */
  struct    sockaddr_in servaddr;  /*  socket add structure  */
  char     *endptr;                /*  for strtol()          */

  /* Get port number from the command line, and set
      to default port if no arguments were supplied */

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

  /* Create the listening socket */

  if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
  {
    Error(11);  // listening socket
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
    Error(12);  // socket bind
  }

  if ( listen(list_s, (1024)) < 0 )
  {
    Error(13);
  }
    
  /* Enter an infinite loop to respond
     to client requests and echo input */

  char word[1024];
  while (1)
  {

    /* Wait for a connection, then accept() it */

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
