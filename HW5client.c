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

  while ( 1 )
    {
      printf("%% ");
      if (fgets(line, sizeof(line), stdin) != NULL)
	     {
	        fprintf(stderr, "Input too long, or EOF\n");
	        exit(-1);
	     }

      //Length of loop
      count = strlen(line) + 1;

      for ( i = 0; i < count; ++i )
      {
	       //Get a character from the line and put into the socket
        c = line[i];
        rc = Socket_putc(c, connect_socket);
	      //If EOF then we quit
        if (rc == EOF)
        {
          fprintf(stderr, "Socket_putc EOF or error\n");
          Socket_close(connect_socket);
          exit(-1);
        }
      }

      for ( i = 0; i < MAX_LINE; ++i)
	    {
	      c = Socket_getc(connect_socket);
	      if (c == EOF)
	      {
          printf("Socket_getc EOF or error\n");
	        Socket_close(connect_socket);
	        exit(-1);
	      }
	      else
	      {
	        line[i] = c;
	        if (line[i] == '\0')
		        break;
	      }
      }
      
      //Make sure string is null terminated
      if (i == MAX_LINE)
	      line[i-1] == '\0';
      //Output string to tsdout
      printf("%s", line);
    }

  Socket_close(connect_socket);
  exit(0);
}
