#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "Socket.h"
#include "HW5shared.h"

int main(int argc, char *argv[])
{
  //Argument 1 is dns Server
  //Argument 2 is welcoming port
  char line[MAX_LINE];
  int count = 0;
  int i, c, rc;
  int forever;

  //Socket descriptor
  Socket connect_socket;

  if (argc < 3 )
  {
    printf("No host and port\n");
    exit(EXIT_FAILURE);
  }

  //Connect to the server at the speicifc host and port
  connect_socket = Socket_new(argv[1], atoi(argv[2]));
  if (connect_socket < 0)
  {
    printf("Failed to connect to server\n");
    exit(EXIT_FAILURE);
  }

  printf("%% ");
  while (fgets(line, sizeof(line), stdin) != NULL)
  {
    count = strlen(line) + 1;

    //Loop to read through the string.
    for ( i = 0; i < count; ++i)
    {
      c = line[i];
      rc = Socket_putc(c, connect_socket);  //Deposit a character into the socket
      if (rc == EOF)
      {
        printf("Client socket_putc: Don't call a closed pipe\n");
        Socket_close(connect_socket);
        exit(-1);
      }
    }

    //Get the output from the
    for ( i = 0; i < MAX_LINE; ++i)
      {
        c = Socket_getc(connect_socket);
        if (c == EOF)
        {
          printf("Client socket_getc: Don't call a closed pipe\n");
          Socket_close(connect_socket);
          exit(-1);
        }

        line[i] = c;
        if (line[i] = '\0')
          break;
      }

      if (i == MAX_LINE)
        line[i-1] = '\0';

      printf("%s", line);
  }

  Socket_close(connect_socket);
  exit(0);
}
