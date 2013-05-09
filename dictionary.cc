/*

  dictionary.cc
  Server for dictionary

*/

/* Main TODOs */
/* TODO: Get sockets to close after finished working   */
/* TODO: Create a wrapper function for error handling, 
         rough draft in helper.cc                      */

#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions TODO: Find out what these are         */
#include <string.h>           /*  strings TODO: Is this needed? Is strcmp dependant on this? */
#include "asciiart.cc"        /*  Include my easter egg (Justine, no peeking ;) ) */

#include <stdlib.h>           /*  exit functions            */
#include <stdio.h>            /*  fprintf, I assume TODO: Find out */

/*  Global constants  */

#define ECHO_PORT          (2002)
#define MAX_LINE           (1000)

int main(int argc, char *argv[]) {
  int       list_s;                /*  listening socket          */
  int       conn_s;                /*  connection socket         */
  short int port;                  /*  port number               */
  struct    sockaddr_in servaddr;  /*  socket address structure  */
  char     *endptr;                /*  for strtol() TODO: Find out what this is */

  /*  Get port number from the command line, and
      set to default port if no arguments were supplied  */

  if ( argc == 2 )
  {
    port = strtol(argv[1], &endptr, 0); /* TODO: What is happenning here? */
    if ( *endptr )                      /* TODO: And here?                */
    {
      fprintf(stderr, "DICTIONARY: Invalid port number.\n");
      exit(EXIT_FAILURE);
    }
  }
  else if ( argc < 2 )
  {
    port = ECHO_PORT;
  }
  else 
  {
    fprintf(stderr, "DICTIONARY: Invalid arguments.\n");
    exit(EXIT_FAILURE);
  }

	
  /*  Create the listening socket  */

  if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
  {
    fprintf(stderr, "DICTIONARY: Error creating listening socket.\n");
    exit(EXIT_FAILURE);
  }


  /*  Set all bytes in socket address structure to
      zero, and fill in the relevant data members   */

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(port);


  /*  Bind our socket addresss to the 
      listening socket, and call listen()  */

  int on = 1;
  setsockopt(list_s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) 
  {
    fprintf(stderr, "DICTIONARY: Error calling bind()\n");
    exit(EXIT_FAILURE);
  }

  if ( listen(list_s, (1024)) < 0 ) 
  {
    fprintf(stderr, "DICTIONARY: Error calling listen()\n");
    exit(EXIT_FAILURE);
  }

    
  /*  Enter an infinite loop to respond
      to client requests and echo input  */

  char lolcat[1024];
  while (1)
  {

    /*  Wait for a connection, then accept() it  */

    if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 )
    {
      fprintf(stderr, "DICTIONARY: Error calling accept()\n");
      exit(EXIT_FAILURE);
    }

    memset(&lolcat, 0, sizeof(lolcat)); /* So THAT'S what memset is for... */

    int amt = read(conn_s, lolcat, 1024); /* write the data from the client to 'lolcat' */

    /* TODO: move the following code to an external file,
       maybe even one for each letter in the alphabet. 
       Also, incorporate a dictionary library             */

    if ( strcmp(lolcat, "antidisestablishmentarianism\r\n") == 0)
    {
      write(conn_s, "A political position that originated in 19th-century Britain in opposition to proposals for the disestablishment of the Church of England.\n", 140);
    }
    else if ( strcmp(lolcat, "hello\r\n") == 0)
    {
      write(conn_s, "A common formal greeting in the English language.\n", 51);
    }
    else if ( strcmp(lolcat, "thanks\r\n") == 0)
    {
      write(conn_s, EASTEREGG, 1768);
    }
    else
    {
      write(conn_s, "That word is not in this dictionary yet.\n", 43);
    }

    if ( close(conn_s) < 0 )
    {
      fprintf(stderr, "DICTIONARY: Error calling close()\n"); // This is what was in memory overwriting my null chars
      exit(EXIT_FAILURE);
    }
  }
}
