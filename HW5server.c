#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "Socket.h"
#include "HW5shared.h"

#define MAXFILENAME 50
#define MAXARGSIZE 100

ServerSocket welcome_socket;
Socket connect_socket;
char filename[MAXFILENAME];

void read_line(char *line);
void tokenize(char *line, char **argv);
void output_results();

int main(int argc, char *argv[])
{
  int ID;
  char line[MAX_LINE];
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

  /* Open a connect socket through the welcom socket*/
  connect_socket = ServerSocket_accept(welcome_socket);
  if (connect_socket < 0)
  {
    printf("Failed connect socket\n");
    exit(-1);
  }

  Socket_close(welcome_socket);  //Close the welcome socket as it is unused

  pid_t childPID;
  char *argcv[MAXARGSIZE];
  int child_status;

  /* Run until exited*/
  while ( 1 )
  {
    ID = (int)getpid();                    // Get PID of the Current Process
    sprintf(filename, "tmp%d", ID);        // Make file name from the current pid
    tmp = freopen(filename, "w", stdout);  // Redirect stdout to the tmp file

    read_line(line);
  
    childPID = fork();  //Child Process to read from client and execute
    if (childPID < 0)
    {
      printf("Fork error\n");
      exit(-1);
    }

    if (childPID == 0)  //Child process
    {
      tokenize(line, argv);  //Tokenize the input line into arguments
      errno = 0;
      // Check if the file name is valid
      if (execvp(*argv, argv) == -1 ) //Run the command in the arguments list
      {
	printf("Execvp error: %d\n", errno);
        exit(-1);
      }      
    }
    else if (childPID > 0)
    {
      int flag = 0;

      //Wait until the child process finishes
      if (waitpid(-1, &child_status, 0) == -1) {
	printf("Server: wait error\n");
      }
      //Check signals for child process
      if (WIFSIGNALED(child_status)) {
	printf("Server: WIFSIGNALED ERROR: %d\n", WTERMSIG(child_status));
      }
      else if (WIFSTOPPED(child_status)) {
	printf("Server: WIFSTOPPED ERROR: %d\n", WSTOPSIG(child_status));
      }
 
      fclose(tmp);  //Close the redirected temp file

      output_results();  //Ouput thte reuslts back into socket
    }
  }
}

void output_results()
{
  FILE *output_file;
  int c, rc;

  if ((output_file = fopen(filename, "r")) == NULL)
    {
      printf("Server Error opening output file\n");
      exit(-1);
    }

  do
    {
      c = getc(output_file);
      
      if (c == EOF)
	break;

      Socket_putc(c, connect_socket);
    }
  while (c != EOF);

  Socket_putc('\0', connect_socket);
  //Socket_putc(EOF, connect_socket);

  fclose(output_file);
  remove(filename);
}

void read_line(char *line)
{  
  int i, c;
  /* Read from the socket */
  for (i = 0; i < MAX_LINE; ++i)
  {
    c = Socket_getc(connect_socket);  //Assume get c worked
    if (c == EOF)                     //EOF is found on read exit
      {
	remove(filename);                  //Delete the tmp file
	exit(-1);
      }

    line[i] = c;  //Put the character into the line

    if (c == '\0')    //Every line is null terminated so this is how we break out of loop
      {
        return;
      }
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
