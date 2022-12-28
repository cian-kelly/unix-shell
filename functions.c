/* 
 Name: Cian Kelly
 Student Number: 20429616
 Email: cian.kelly2@ucdconnect.ie
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include "functions.h"

/* Preprocessor definitions */
#define BUF_SIZE 1024                    /* Preprocessor defintion for buffer size */
#define SHELL_TOKENIZER_BUF_SIZE 64      /* Preprocessor definition for size of buffer for shell tokenizing */

int f;                                   /* integer to check if file opened properly for redirect builtin */
int old_out;                             /* integer that stores value of existing stdout */
int redirect=0;                          /* integer that keeps track of output redirection - 0 means no redirection, 1 means output needs to be redirected and so stdout should go back to default, 2 means stdout has gone back to normal and so needs to be closed */

/* Function to handle signals that are received (^C) */
void SIGHANDLER(int signo) {
 time_t t;                               /* Variable declaration */
 struct tm *info;                        /* Variable declaration - tm structure */
 char timeBuffer[80];                    /* Variable declaration - String to hold the formatted date and time */
 time(&t);                               /* pointing t to time */
 info = localtime(&t);                   /* Using the time pointed by t to fill the tm structure */
 strftime(timeBuffer,sizeof(timeBuffer),"[%d/%m %H:%M]", info);  /* Using strftime to display the time (date and hours and minutes */

 if (signo == SIGINT) {                  /* if signal received equals SIGINT which is control c (^C) */
   printf("\n%s# ", timeBuffer);         /* Printing timeBuffer string - formatted time and date */
   fflush(stdout);                       /* Flushing standard output */
  }
}

/* Function to handle the looping that a shell does (reading, parsing, executing and then repeating) */
void shellLoop() {
 char *buffer;                           /* Variable declaration - pointer for string for user input */
 char **args;                            /* Variable declaration - 2d array to store arguments for shell after they have been tokenized */
 int status=1;                           /* Variable declaration - variable to keep track of shell set to one. shell should only run if status equals 1 as ther is a problem if not */

 time_t t;                               /* Variable declaration */
 struct tm *info;                        /* Variable declaration - tm structure */
 char timeBuffer[80];                    /* Variable declaration - String to hold the formatted date and time */

 while (status) {                        /* loop while status equals 1 */
   time(&t);                             /* pointing t to time */
   info = localtime(&t);                 /* Using the time pointed by t to fill the tm structure */
   strftime(timeBuffer,sizeof(timeBuffer),"[%d/%m %H:%M]", info); /* Using strftime to display the time (date and hours and minutes */
   printf("%s# ", timeBuffer);           /* Printing timeBuffer string - formatted date and time */

   if (signal(SIGINT, SIGHANDLER) == SIG_ERR) {                   /* if ^C signal is picked up, call SIGHANDLER function. if error; */
     fprintf(stderr, "Error catching SIGINT\n");                  /* if error print to let user know */
   }

   buffer = readShellLine();             /* let buffer equal to string returned by readShellLine - the user's input */
   args = splitShellLine(buffer);        /* let args equal to 2d array of tokens returned by splitShellLine. pass buffer as argument */
   status = shellExecute(args);          /* let status equal to value returned by shellExecute / shellLaunch - should be 1 unless problem */

   free(buffer);                         /* free pointer */
   free(args);                           /* free pointer*/
 }
 if (redirect == 2) {                    /* if redirect has a value of two */
  close(old_out);                        /* close original stdout */
 }
}

/* function to read user input */
char * readShellLine(void) {
 char *buffer;                           /* Variable declaration - string to hold user input */
 size_t bufferSize=0;                    /* Variable declaration - setting size of buffer to zero so getLine can allocate */

 buffer = (char *)malloc(BUF_SIZE * sizeof(char));  /* dynamically allocating memory for buffer of size BUF_SIZE (1024) characters */

 if (buffer == NULL) {                   /* if buffer is null there was a problem allocating memory */
  fprintf(stderr, "Allocation Error\n"); /* print to stderr to let user know */
  exit(EXIT_FAILURE);                    /* exit due to unsuccesful execution of memory allocation */
 }

 if (getline(&buffer,&bufferSize,stdin) == -1) {    /* if the end of stdin was reached */
  if (feof(stdin)) {                     /* if EOF was received */
    exit(EXIT_SUCCESS);                  /* successful execution so exit */
  } else {                               /* otherwise (no EOF received) */
     fprintf(stderr, "Readline Error\n");           /* print to stderr to let user know */
     exit(EXIT_FAILURE);                 /* exit due to unsuccessful execution function */
    }
 }
 return buffer;                          /* return the string that was scanned in */
}

/* function to tokenize string entered from stdin */
char **splitShellLine(char *buffer) {
 int bufsize = SHELL_TOKENIZER_BUF_SIZE;                /* creating a buffer size for memory allocation */
 int i = 0;                                             /* integer used for indexing in tokens array */
 char **tokens = malloc(bufsize * sizeof(char*));       /* array of strings which will contain tokenised string entered from stdin */
 char *token;                                           /* string current token before it enters tokens array */
 char delimit[]=" \t\r\n\a\v\f\"";                      /* delimiters for strtok - all posix whitespaces plus quote marks */
 char *filename;                                        /* string to store filename */

 if (strstr(buffer, ">") != NULL) {                     /* if the character > is in the string buffer */
  redirect=1;                                           /* redirection must occur so set redirect to 1 */
  filename = strtok(buffer, ">");                       /* filename is second token when tokenising using delimiter > */
  filename = strtok(NULL, ">");                         /* letting filename equal second token */
  filename++;                                           /* incrementing pointer so it removes whitespace from after redirect symbol */
  filename[strcspn(filename, "\n")] = '\0';             /* removing the newline character and replacing it with termination of string */
  f = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666);   /* opening filename specified by user. letting integer f equal the result of this operation */

  if (f == -1) {                                        /* if f is -1, there was a problem */
    fprintf(stderr, "error opening file\n");            /* inform user of problem by printing to stderr */
    exit(EXIT_FAILURE);                                 /* exit due to unsuccessful execution */
  }

  old_out = dup(fileno(stdout));                        /* save integer of old file descriptor - typically 1 but we dont want to hardcode */

  if (dup2(f, fileno(stdout)) == -1) {                  /* changing file descriptor of stdout - if unsuccessful */
    fprintf(stderr, "error redirecting stdout\n");      /* inform user by printing to stderr */
    exit(EXIT_FAILURE);                                 /* exit due to unsuccessful execution */
  }
 }

 if (tokens == NULL) {                                  /* if tokens is null */
  fprintf(stderr, "Allocation Error\n");                /* there was a problem allocating memory. inform user of the problem by printing to stderr */
  exit(EXIT_FAILURE);                                   /* exit due to unsuccessful execution */
 }

 token = strtok(buffer, delimit);                       /* tokenizing buffer using delimiters. set token equal to first token */

 while (token != NULL) {                                /* loop while token is not null*/
  tokens[i] = token;                                    /* set first element of array tokens equal to token string */
  i++;                                                  /* increment i */

  if (i >= bufsize) {                                   /* if i is greater than or equal to bufsize, we need to allocate more memory using realloc */
   bufsize *= 2;                                        /* double bufsize */
   tokens = realloc(tokens, bufsize * sizeof(char*));   /* reallocate memory */
   if (tokens == NULL) {                                /* if tokens is null there was a problem reallocating memory */
     fprintf(stderr, "Allocation Error\n");             /* inform user of the problem */
     exit(EXIT_FAILURE);                                /* exit due to unsuccessful execution */
   }
  }
  token = strtok(NULL, delimit);                        /* set token equal to the next token from strtok*/
 }
 tokens[i] = NULL;                                      /* set index i of tokens array to null*/
 return tokens;                                         /* return the array of arguments tokens */
}

/* function to launch */
int shellLaunch(char **args) {
 pid_t pid = fork();       /* Variable declaration - contains process id of newly created child process (used later for system call to ensure child changes state before parent process executes) */
 int status;               /* Variable declaration - integer to keep track of program status */

 if (pid < 0) {            /* if error forking - creation of a child process was unsuccessful */
  fprintf(stderr, "Error forking");  /* inform user by printing to stderr that there was an error forking */
  exit(EXIT_FAILURE);                /* exit due to unsuccessful fork so shell can keep running */
 } else if (pid == 0) {              /* if this is the child process */
    if (execvp(args[0], args) == -1) { /* run program whose name is found in args[0] using execvp. execvp will find the program from the name provided. second argument args is the array of arguments for program args[0] */
      fprintf(stderr, "Error with execvp in Shell\n");  /* if execvp is -1 there was a problem and we should inform user. print error message to stderr to let user know */
    }
    exit(EXIT_FAILURE);               /* exit due to unsuccessful execution of execvp so shell can keep running */
   } else {                           /* otherwise (>0) - this is the parent process upon successful creation of child process */
      do {
        waitpid(pid, &status, WUNTRACED);      /* system call for suspending execution of the parent process until the child with has changed state */
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));   /* while child process hasnt terminated normally (WIFEXITED) or been killed (WIFSIGNALED) */
     }

  if (redirect) {                     /* if redirect has a value of 1 */
   fflush(stdout);                    /* flush stdout */
   close(f);                          /* close f (file that output was redirected to) */
   dup2(old_out, fileno(stdout));     /* replace existing stdout with default */
   redirect=2;                        /* set redirect to 2 */
  }

 return 1;                            /* return 1 for status so shell can keep looping */
}

/* function to execute the arguments that have been tokenised */
int shellExecute(char **args) {
 if (args[0] == NULL) {              /* if first element of args array is null, return 1 as no command was entered */
  return 1;                          /* returning value one to keep shell looping */
 }

 if (strcmp(args[0], "cd") == 0) {   /* if first element of args array is equal to command cd */
  if (args[1] == NULL) {             /* if second element of array is null */
    fprintf(stderr, "No argument after \"cd\"\n"); /* inform user. print to stderr that there was no path specified */
  } else if (chdir(args[1]) != 0) {  /* system call to change working directory to path specified in second element of args array */
       fprintf(stderr, "Error changing directory");  /* if anything other than zero was returned then there was a problem changing directory */
    }
  return 1;                          /* return 1 to keep shell looping */
 }
 return shellLaunch(args);           /* return function call - call function and return what is returned by function */
}
