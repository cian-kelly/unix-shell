/* 
 Name: Cian Kelly
 Student Number: 20429616
 Email: cian.kelly2@ucdconnect.ie
*/

/* include guards */
#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTION_H_INCLUDED

/* Declaring functions - function prototypes */
void shellLoop(void);
char * readShellLine(void);
char ** splitShellLine(char *);
void SIGHANDLER(int signo);
int shellLaunch(char **args);
int shellExecute(char **args);

#endif
