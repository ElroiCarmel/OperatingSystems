#ifndef EX3
#define EX3 0

#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h>


void executeCommand(char *argv[], int argc);
void executePipeline(char *pipeCommands[], int pipec);
/**
 * @brief Convert from one string to substrings and put in argv.
 * 
 * @param command raw string of command from user input
 * @param argv array of pointers to substrings of command 
 * @param amperFlag assign whether parent should wait for child process
 * @return int: the amount of arguments in command
 */
int parseCommand(char *command, char *argv[], int *amperFlag);
/**
 * @brief Convert form one string with pipe signs ('|')
 *        to substrings of commands and put in pipes.
 * 
 * @param raw raw string from user input
 * @param pipes array of pointers to substrigs of raw-commands
 * @return int: the amount of commands in pipe
 */
int parsePipeline(char *raw, char *pipes[]);
void redirectCommand(char *argv[], int argc);
int isAmper(char *argv[], int argc);
/**
 * decide how to behave upon receiving the SIGINT
 */
void handler();

#endif