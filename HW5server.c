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

void run_service();
void read_line(char *line_data);
void tokenize(char *line, char **argv);

int main(int argc, char *argv[])
{
  pid_t childPID;
  int child_status, ID;
  char filename[50];
  char line[MAX_LINE];
  char *argv[100];  //Hold arguments after parsing input line

  FILE *tmp;
  FILE *output;

  //Argument 1: Welcoming port
  if (argc < 2)
  {
    printf("No port specified\n");
    exit(-1);
  }

  /* Create a welcome socket at the specified port */
  welcome_socket = ServerSocket_new(atoi(argv[1]));
  if (welcome_socket < 0)
  {
    printf("Failed new server socket\n");
    exit(-1);
  }

  connect_socket = ServerSocket_accept(welcome_socket);
  if (connect_socket < 0)
  {
    printf("Failed connect socket\n");
    exit(-1);
  }

  /* Run until exited*/
  while ( 1 )
  {
    ID = (int)getpid();                  // Get PID of the Current Process
    sprintf(filename, "tmp%d", ID);     // Make file name from the current pid
    tmp = freopen(filename, "w", stdout);         // Set stdout to the output file

    read_line(line);

    childPID = fork();  //Child Process to read from client and execute
    if (childPID < 0)
    {
      perror("fork");
      exit(-1);
    }

    if (childPID == 0)
    {
      Socket_close(welcome_socket);

      tokenize(line, argv);
      // Check if the file name is valid
      if (execvp(*argv, argv) == -1 ) //Run the command in the arguments list
      {
        perror("Execvp");
        exit(-1);
      }

      fclose(tmp);

      //Code after this point should not be Run
      printf("Execvp unreachable code\n");
    }
    else if (childPID > 0)
    {
      if (waitpid(-1, &child_status, 0) == -1) {
        perror("Wait error\n");
      }
      if (WIFSIGNALED(child_status) != 0) {
        fprintf(stderr, "Child process ended becaues of signal %d\n", WTERMSIG(child_status));
      }
      else if (WIFEXITED(child_status) != 0) {
        fprintf(stderr, "Child process ended normally, status = %d\n", WEXITSTATUS(child_status));
      }

      // Create a welcome socket at the specified port
      FILE *output;
      if ((output = fopen(filename, "r")) == NULL)
	     {
	        printf("File open error\n");
	        exit (-1);
	     }

      int c, rc;
      while ((c = getc(output)) != EOF)
      {
        rc = Socket_putc(c, connect_socket);
        if (rc == EOF)
        {
          printf("Socket_putc EOF error\n");
          break;
        }
      }

      fclose(output);
      remove(filename);
    }
  }
}

void read_line(char *line_data)
{
  int i, c;
  /* Read from the socket */
  for (i = 0; i < MAX_LINE; ++i)
  {
    if ((c = Socket_getc(connect_socket)) == EOF)  //Get a value from the buffer and check for EOF
    {
      printf("Socket_getc EOF or error\n");
      Socket_connect(connect_socket);
      exit(-1);
    }

    line_data[i] = c;
    if (c == '\0')    //Every line is null terminated so this is how we break out of loop for reading
      break;
  }
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
