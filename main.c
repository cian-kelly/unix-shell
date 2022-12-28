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

int main(int argc, char **argv) {
  shellLoop();                           /* command loop that takes input from stdin, tokenizes the string and then executes arguments */
  return EXIT_SUCCESS;                   /* when looping has finished, program was successfully executed so exit */
}

