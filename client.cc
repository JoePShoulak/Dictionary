#include <sys/types.h> 
#include <sys/socket.h>
#include <iostream>

using std::cout;
 
int main(int argc, char* argv[])
{
  if (argc == 2)
    cout << argv[1] << "\n";
  else
    cout << "Usage: dictionary <word>\n";
  
  return 0;
}
