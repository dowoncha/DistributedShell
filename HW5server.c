#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

#include "Socket.h"
#include "HW5shared.h"

ServerSocket welcome_socket;
Socket connect_socket;

void run();
int read_line(char *line_data);
void tokenize(char *line, char **argv);

int main(int argc, char *argv[])
{
    pid_t currentPID, childPID, term_pid;
    int chld_status;
    char filename[50];

    //Argument 1: Welcoming port
    if (argc < 2)
    {
          printf("No port specified\n");
          exit(EXIT_FAILURE);
    }

    /* Create a welcome socket at the specified port */
    welcome_socket = ServerSocket_new(atoi(argv[1]));
    if (welcome_socket < 0)
    {
      printf("Failed new server socket\n");
      exit(EXIT_FAILURE);
    }

    /* Run until exited*/
    while ( 1 )
    {
      /* Accept an incoming client connection.
       * blocks process until a connection attempt by a client.
       * Creates a new data transfer socket. */
      connect_socket = ServerSocket_accept(welcome_socket);
      if (connect_socket < 0)
      {
        printf("Failed accept on server socket\n");
        exit(EXIT_FAILURE);
      }

      currentPID = getpid();                  // Get PID of the Current Process
      sprintf(filename, "tmp%d", currentPID); // Make file name from the current pid
      freopen(filename, "w", stdout);         // Set stdout to the output file

      childPID = fork();  //Child Process to read from client and execute
      if (childPID < 0)
      {
        perror("fork");
        exit(EXIT_FAILURE);
      }
      else if (childPID == 0)
      {
        char *argv[100];  //Hold arguments after parsing input line

        while ( 1 )
        {
            char line[MAX_LINE];
            if (read_line(line) == -1)    // Get a line from socket
            {
              printf("read line error\n");
              return;
            }

            // Tokenize the input line into argv array
            // The first element in argv will hold the path name
            tokenize(line, argv);
            // Check if the file name is valid
            if (execvp(*argv, argv) == -1 ) //Run the command in the arguments list
      	     {
      	        perror("Execvp");
      	        exit(EXIT_FAILURE);
      	     }

            Socket_close(connect_socket); //Close the connection socket
            exit(EXIT_SUCCESS);
        }
      }
      else
      {
        FILE *output;                   //The file that has the server output
        output = fopen(filename, "r");

        if (output)                     //If hte file is open
        {
          int c, rc;
          while ((c = getc(output)) != EOF)   //Run until EOF is reached from File
          {
            rc = Socket_putc(c, connect_socket);  //Put character into socket
            if (rc == EOF)
            {
              printf("Socket_putc EOF error\n");
              return;
            }
          }
        }

        fclose(output);
        Socket_close(connect_socket);
        /* reap a zombie every time through the loop, avoid blocking*/
        term_pid = waitpid(-1, &chld_status, WNOHANG);
      }


    }
}

int read_line(char *line_data)
{
  int i, c, rc;
  /* Read from the socket */
  for (i = 0; i < MAX_LINE; ++i)
  {
    c = Socket_getc(connect_socket);
    if (c == EOF)
    {
      printf("Socket_getc EOF or error\n");
      return -1;
    }

    line_data[i] = c;
    if (c == '\0')
      break;
  }

  return 0;
}

/**
 * This function will take a line of input and tokenize it into arguments and return a pointer to the arguments
 * the delimitters used are space, tab, newline, and null values.
 **/
void tokenize(char *line, char **argv)
{
  assert(line != NULL);

  //Grab first token from string this should be the command
  *argv = strtok(line, " \t\n\0");
  ++argv;
  //This loop will run until we are out of arguments to add from the line
  while ( *argv )
    {
      /* strtok requires every call after the first one to read NULL since the line is already stored*/
      *argv++ = strtok(NULL, " \t\n\0");
    }
}
