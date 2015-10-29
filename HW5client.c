#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "Socket.h"
#include "HW5shared.h"

Socket connect_socket;

void DepositLine(char *line);
void GetOutput();
void ErrorCheck();

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
      if ((fgets(line, sizeof(line), stdin)) == NULL)  //Read lines until EOF
      {
	printf("Input too long, or EOF\n");
	break;                           //hould not happen
      }

      DepositLine(line);  //Deposit the read line into the socket

      GetOutput();        //Get the result from command from the socket and output
    }

  Socket_putc(EOF, connect_socket);  //Let the server know the client is finished

  Socket_close(connect_socket);
  exit(0);
}
/*
void ErrorCheck()
{
  int c;
  c = Socket_getc(connect_socket);

  if (c == 1)
    printf("Error on server!\n");
}
*/
void DepositLine(char *line)
{
  int i, rc, c;

  do
    {
      c = line[i++];  //Get a character from the input line

      rc = Socket_putc(c, connect_socket);  //Deposit into socket
    }
  while (c != '\0');
}

void GetOutput()
{
  int i, c;

  for (i = 0; i < MAX_LINE; ++i)
    {
      c = Socket_getc(connect_socket);  //Get a character from the read socket

      if (c == '\0')  //Null byte signals end of output
	{
	  return;
	}

      putchar(c);     //Output the caracter
    }
}

