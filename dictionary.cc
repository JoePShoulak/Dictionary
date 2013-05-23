#include <sys/socket.h>  // socket def
#include <sys/types.h>   // socket types
#include <arpa/inet.h>   // inet funtions
#include <unistd.h>      // UNIX functions
#include <string.h>      // strings
#include <stdlib.h>      // standard
#include <stdio.h>       // printf
#include <iostream>      // for file I/O
#include <fstream>       // also for files

using std::ifstream;
using namespace std;

// User-defined wrappers 

// converts an int to a c++ string
string IntToString(int to_convert) {
  char converted[50];
  int n  = sprintf(converted, "%d", to_convert);
  return converted;
}
// wraps a c++ string with the 
// escape code for 'bold' and
// 'normal' text. The HTML
// equivelant would be 
// <b>text</b>.
string Bold(string to_bold) {
  string bolded = "\x1b[1m";
  bolded += to_bold;
  bolded += "\x1b[0m";
  return bolded;
}
// returns a substring of a 
// c++ string, up until the
// last X characters.
string Strip(string word, int amount) {
  return word.substr(0, (word.length() - amount));
}
// wrapper for write that
// makes it handle c++ strings,
// and automatically calulates
// length.
int Send(int socket, string word) {
  return write(socket, ("%s", word.data()), word.length());
}

// User-defined functions

// takes the error code and returns
// the appropriate error message.
string ErrorLookup(int error_num) {
  switch(error_num) {
    // sockety errors
    case 10: return "Port number";
    case 11: return "Listening socket";
    case 12: return "Socket bind";
    case 13: return "Socket listen";
    case 14: return "Socket accept";
    case 15: return "Socket close";
    // CLI errors
    case 20: return "Argument";
    case 21: return "Dictionary read";
    // dictionary errors
    case 30: return "Not in dict"
    case 31: return "Input";
    // all others
    default: return "Unknown";
  }
}
// calls an error. if send is non-zero,
// the error is sent to the server. If
// it is zero, it is returned to be
// sent to the client (if you want).
// if fatal is non-zero,
// exit(EXIT_FAILURE) is called, and
// the server crashes.
string Error(int error_num, int send, int fatal) {
  string error_type = ErrorLookup(error_num);
  string message = error_type;
  message += " error";
  if (fatal) { message += " (fatal)"; }
  message += ". (code ";
  message += string(IntToString(error_num));
  message += ")\n";
  if (send) { fprintf(stdout, "%s", message.data()); }
  if (fatal) { exit(EXIT_FAILURE); }
  return message;
}

struct node {
  //string word;
  int num;
  string type;
  string def;
  node *next;
};

node Load() {
  node* end = (node*) malloc(sizeof(node));
  cout << 1 << endl;
  //end->word = "";
  end->num = 0; // passes
  cout << 2 << endl;
  end->type = "";
  cout << 3 << endl;
  end->def  = "";
  cout << 4 << endl;
  end->next = NULL;
  cout << 5 << endl;
  string line;
  ifstream my_file("definitions.txt");
  if (my_file.is_open()) {
    while (my_file.good()) {
      getline(my_file, line);
      int pos1 = line.find("!@#$");
      string rest = line.substr(pos1+4);
      int pos2 = rest.find("!@#$");
      node *new_item;
      new_item = (node*) malloc(sizeof(node));
      cout << line.substr(0, pos1);
      //new_item->word = line.substr(0, pos1);
      new_item->num = 1;
      new_item->type = rest.substr(0, pos2);
      new_item->def  = rest.substr(pos2+4 );
      new_item->next = end;
      end = new_item;
   }   
  }
  return *end;
};

node NthNode(node head, int n) {
  int fail = 0;
  for (int i=1; i<n; i++) {
    if (!fail) {
      head = *head.next;
      if (head.next == NULL) { fail = 1; }
    } else {
      cout << "Term out of range" << endl;
      exit(EXIT_FAILURE);
    }
  }
  return head;
}


// the workhorse of the program.
// takes the input, scans the 117,000+
// definitions, checking each one,
// returning the bolded word,
// its type, and definition
void Define(char to_define[], int sock) {
  string input(to_define);
  if (input.length() < 3) { // User just pressed enter
    Send(sock, Error(31, 0, 0)); // input, keep, benign
  } else {  // so, if the word is actually a word...
  }
}

// main

// creates the sockets, handles
// the requests, calls all other
// functions.
int main(int argc, char *argv[]) {
  node head = Load();
  cout << NthNode(head, 5).num << endl;
  //cout << NthNode(head, 5).word << endl;

  int       list_s;                // listening socket
  int       conn_s;                // connection socket
  short int port;                  // port number
  struct    sockaddr_in servaddr;  // socket add structure
  char     *endptr;                // for strtol()
  if (argc == 2) {
    port = strtol(argv[1], &endptr, 0);
    if (*endptr) { Error(10, 1, 1); }  // port number, send, fatal
  } else if (argc < 2) {
    port = 1123;
  } else { Error(20, 1, 1); }  // argument, send, fatal
  // Create the listening socket
  if ((list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    Error(11, 1, 1);  // listening socket, send, fatal
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
  if (bind(list_s,(struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
    Error(12, 1, 1);  // socket bind, send, fatal
  }
  if (listen(list_s, (1024)) < 0) {
    Error(13, 1, 1); // listening socket, send, fatal
  }
  // Enter an infinite loop to respond
  // to client requests and echo input
  char word[1024];
  while (1) {
    // Wait for a connection, then accept() it
    if ((conn_s = accept(list_s, NULL, NULL)) < 0) {
      Error(14, 1, 1);  // socket accept, send, fatal
    }
    memset(&word, 0, sizeof(word));  // empty out word
    read(conn_s, word, 1024);  // write the input to 'word'
    Define(word, conn_s);  // main lookup and print 
    if(close(conn_s) < 0) { Error(15, 1, 1); }  // socket close, send, fatal
  }
}
