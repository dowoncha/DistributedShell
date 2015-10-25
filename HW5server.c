#include <sys/types.h>
#include <unistd.h>

#include "Socket.h"
#include "ToUpper.h"

ServerSocket welcome_socket;
Socket connect_socket;

void run(char *line);
void tokenize(char *line, char **argv);

int main(int argc, char *argv[])
{
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

      char filename[50];
      pid_t current_pid, spid;
      current_pid = getpid();
      sprintf(filename, "tmp%d", current_pid);
      freopen(filename, "w", stdout);

      spid = fork();  /* create child == service process */
      if (spid == -1)
         {
           perror("fork");
           exit(EXIT_FAILURE);
         }
      if (spid == 0)
         {/* code for the service process */
          toupper_service();
          Socket_close(connect_socket);
          exit(EXIT_SUCCESS);
         } /* end service process */
      else /* daemon process closes its connect socket */
         {
          Socket_close(connect_socket);
          /* reap a zombie every time through the loop, avoid blocking*/
          //term_pid = waitpid(-1, &chld_status, WNOHANG);
         }
     } /* end of infinite loop for daemon process */
    }
}

/**
 * This function will fork a child process and execute it
 * This particular child process will tokenize the input line and run the file at the location
 * can be a command (e.g. ls, pwd)
 **/
void run(char *line)
{
  pid_t childPID;
  char *argv[MAXARGSIZE];

  errno = 0;
  // Fork a child process
  childPID = fork();

  if ( childPID < 0)
    {
      perror("Fork");
      exit(EXIT_FAILURE);
      //printf("Fork failed with error: %s\n", strerror(errno)); //report an error
    }
  else if (childPID == 0)
    {
      // Tokenize the input line into argv array
      // The first element in argv will hold the path name
      tokenize(line, argv);
      // Check if the file name is valid

      /**
       * Do extra credit here to go through the path variable and find the right directory
       * that holds the command in it. TO DO
       **/

      if (execvp(*argv, argv) == -1 ) //Run the command in the arguments list
	{
	  perror("Execvp");
	  exit(EXIT_FAILURE);
	}
    }
  else //Parent process
    {
      int status;
      errno = 0;
      childPID = wait(&status); //Get the status of the child process

      //If the status of the childPID is less than 0 then there is an error
      if (childPID < 0)
	{
	  perror("Wait");
	  exit(EXIT_FAILURE);
	}

      wait(NULL);  //Otherwise wait for child process to finish
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
  *argv++ = strtok(line, " \t\n\0");
  //This loop will run until we are out of arguments to add from the line
  while ( *argv )
    {
      /* strtok requires every call after the first one to read NULL since the line is already stored*/
      *argv++ = strtok(NULL, " \t\n\0");
    }
}
