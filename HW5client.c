#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "Socket.h"
#include "HW5shared.h"

Socket connect_socket;

void DepositLine(char *line);
void GetOutput();

int main(int argc, char *argv[])
{
  //Argument 1 is dns Server
  //Argument 2 is welcoming port
  char line[MAX_LINE];
  int count = 0;

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
      if ((fgets(line, sizeof(line), stdin)) == NULL)
      {
	printf("Input too long, or EOF\n");
	break;                           //hould not happen
      }

      DepositLine(line);

      GetOutput();
    }

  Socket_putc(EOF, connect_socket);  //Let the server know  the client is finished

  Socket_close(connect_socket);
  exit(0);
}

void DepositLine(char *line)
{
  int i, rc, c;

  do
    {
      c = line[i++];

      rc = Socket_putc(c, connect_socket);
    }
  while (c != '\0');
}

void GetOutput()
{
  int i, c;

  for (i = 0; i < MAX_LINE; ++i)
    {
      c = Socket_getc(connect_socket);
      if (c == EOF)
      {
	printf("Client output EOF reached\n");
        break;
      }

      if (c == '\0')
	{
	  printf("Client output null value. Success\n");
	  break;
	}

      putchar(c);
    }

  printf("Output finished\n");
}

