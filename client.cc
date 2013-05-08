// client.cc

#include <sys/types.h>  //
#include <sys/socket.h> // sockets and stuff
#include <iostream>     // std::cout

using std::cout;
 
int main(int argc, char* argv[])
{
  if (argc == 2) // valid arg structure
    cout << argv[1] << "\n"; // echo
  else // invalid arg structure
    cout << "Usage: dictionary <word>\n"; // print error details
  
  return 0;
}
