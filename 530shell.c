/**
 * Dowon Cha
 * COMP 530 HW2
 * This program is a simple Linux shell.
 * Input: Read in a string with a file name to a command, and various arguments
 * Output: Output result from the called command
 * Honor Pledge: I pledge to not copy other people's code.
 **/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#define MAXLINELENGTH 1000
#define MAXARGSIZE 80

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

int main()
{
  /* Variable declarations  */
  char line[MAXLINELENGTH];

  //Run until done
  while ( 1 )
    {
      /* Output prompt and read in a line*/
      printf("%% ");
      if (fgets(line, sizeof(line), stdin) == NULL) //Read in a line of input make sure its not NULL
	{
	  fprintf(stderr, "Input too long, or EOF\n");
	  exit(EXIT_FAILURE);
	}
      /** Fork a child process and execute it**/
      run(line);   
    }

  return 0;
}
