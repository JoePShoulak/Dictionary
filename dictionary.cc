#include <sys/socket.h>  // socket def
#include <sys/types.h>   // socket types
#include <arpa/inet.h>   // inet funtions
#include <unistd.h>      // UNIX functions
#include <string.h>      // strings
#include <stdlib.h>      // standard
#include <stdio.h>       // printf
#include <math.h>        // pow
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
string Longest(string word1, string word2) {
  return (word1.length() > word2.length()) ? word1 : word2;
}


int Alphabetize(string word1, string word2) {
  if (word1 == word2) {
    return 0;
  }
  int max = Longest(word1, word2).length();
  for (int i=0; i<max; ++i) {
    int first, second;
    if (word1.length() >= i) {
      first = int(word1[i]);
    }
    if (word2.length() >= i) {
      second = int(word2[i]);
    }
    if (first == second) {
    } else if (first < second) {
      return 1;
    } else {
      return 2;
    }
  }
  return 0;
}

string ErrorLookup(int error_word) {
  switch(error_word) {
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
    case 30: return "Not in dict";
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
string Error(int error_word, int send, int fatal) {
  string error_type = ErrorLookup(error_word);
  string message = error_type;
  message += " error";
  if (fatal) { message += " (fatal)"; }
  message += ". (code ";
  message += string(IntToString(error_word));
  message += ")\n";
  if (send) { fprintf(stdout, "%s", message.data()); }
  if (fatal) { exit(EXIT_FAILURE); }
  return message;
}

struct node {
  string word;
  string type;
  string def;
  int index; // adding index, please don't crash
  node *next;
};

node Load() {
  node* end = new node;
  end->word = "";
  end->type = "";
  end->def  = "";
  end->next = NULL;
  string line;
  int it = 0;
  ifstream my_file("definitions.txt");
  if (my_file.is_open()) {
    while (my_file.good()) {
      getline(my_file, line);
      if (line.length()) {
        int pos1 = line.find("!@#$");
        string rest = line.substr(pos1+4);
        int pos2 = rest.find("!@#$");
        node* new_item  = new node;
        new_item->word  = line.substr(0, pos1);
        new_item->type  = rest.substr(0, pos2);
        new_item->def   = rest.substr(pos2+4 );
        new_item->next  = end;
        new_item->index = it++;
        end = new_item;
      }
    }
  }
  return *end;
};

node* NthNode(node head, int n) {
  int fail = 0;
  while (head.index != n) {
    head = *head.next;
  }
  return &head;
}

// takes the input, scans the 117,000+
// definitions, checking each one,
// returning the bolded word,
// its type, and definition
void Define(node *entry, char to_define[], int sock) {
  int count = 0;
  int exponent = 16;
  int position = pow(2,16);
  node* head = entry;
  entry = NthNode(*head, position);
  string input(to_define);
  if (input.length() < 3) { // User just pressed enter
    Send(sock, Error(31, 0, 0)); // input, keep, benign
  } else {  // so, if the word is actually a word...
    bool done = false;
    while (!done) {
      cout << entry->index << " ";
      cout << entry->word << " ";
      int direction = Alphabetize(entry->word, Strip(input, 2));
      if (direction == 0) {
        count++;
        string output = "  " + Bold(entry->word) + " (" + entry->type + "): ";
        output += entry->def + "\n";
        Send(sock, output);
        done = true;
      } else if (direction == 1) {
        position += pow(2, exponent);
        cout << "+ ";
      } else if (direction == 2) {
        position -= pow(2, exponent);
        cout << "- ";
      }
      if (exponent != 0) {
        exponent--;
      } else { exponent = 0; }
      cout << exponent << endl;
      entry = NthNode(*head, position);
    }
  if (!count) { Send(sock, Error(30, 0, 0)); } // not in dict, keep, benign
  }
}

// main

// creates the sockets, handles
// the requests, calls all other
// functions.

int main(int argc, char *argv[]) {
  cout << "Dictionary loading..." << endl;
  node head = Load();
  cout << "Dictionary loaded" << endl;

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
  cout << "Listening socket opened" << endl;
  // Enter an infinite loop to respond
  // to client requests and echo input
  char word[1024];
  while (1) {
    cout << "Waiting for a connection..." << endl;
    // Wait for a connection, then accept() it
    if ((conn_s = accept(list_s, NULL, NULL)) < 0) {
      Error(14, 1, 1);  // socket accept, send, fatal
    }
    cout << "  Connection established" << endl;
    memset(&word, 0, sizeof(word));  // empty out word
    read(conn_s, word, 1024);  // write the input to 'word'
    string request = word;
    request = Strip(request, 2);
    cout << "  User requested definition of \"" << request << "\"" << endl;
    cout << "  Definining \"" << request << "\"..." << endl;
    Define(&head, word, conn_s);  // main lookup and print 
    cout << "  Word defined, and definition sent" << endl;
    if(close(conn_s) < 0) { Error(15, 1, 1); }  // socket close, send, fatal
    cout << "  Connection closed" << endl;
  }
}
