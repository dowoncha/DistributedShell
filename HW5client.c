#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "Socket.h"
#include "ToUpper.h"

int main(int argc, char *argv[])
{
  //Argument 1 is dns Server
  //Argument 2 is welcoming port

  char line[MAX_LINE];
  int count = 0;

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

  //Run until done
  printf("%% ");
  while ( fgets(line, sizeof(line), stdin) != NULL ))
    {
      count = strlen(line) + 1;

      int i, c, rc;
      for ( i = 0; i < count; ++i )
      {
        c = line_data[i];
        rc = Socket_putc(c, connect_socket);
        if (rc == EOF)
        {
          printf("Socket_putc EOF or error\n");
          Socket_close(connect_socket);
          exit(EXIT_FAILURE);
        }
      }
    }

  Socket_close(connect_socket);
  exit(0);
}
