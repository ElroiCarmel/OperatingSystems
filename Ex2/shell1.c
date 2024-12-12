#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>

int main() {
int i, amper, retid, status;
char *argv[10];
char command[1024];
char *token;
char promptMessg[100] = "hello";

while (1) {
    printf("%s: ", promptMessg);
    fgets(command, 1024, stdin);
    command[strlen(command) - 1] = '\0'; // replace \n with \0
    if (strcmp(command, "!!") != 0) {
    /* parse command line */
    i = 0;
    token = strtok (command," ");
    while (token != NULL)
    {
        argv[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    argv[i] = NULL;

    /* Is command empty */ 
    if (argv[0] == NULL)
        continue;

    /* Does command line end with & */ 
    if (! strcmp(argv[i - 1], "&")) {
        amper = 1;
        argv[i - 1] = NULL;
        }
    else 
        amper = 0; 
    }
    /* for commands not part of the shell command language */ 
    if (strcmp(argv[0], "prompt") == 0) {
        strcpy(promptMessg, argv[2]);
        status = 0;
        continue;
    }
    if (strcmp(argv[0], "status") == 0) {
        printf("%d\n", status);
        continue;
    }
    if (strcmp(argv[0], "quit") == 0) {
        return 0;
    }

    if (strcmp(argv[0], "cd") == 0) {
        if (chdir(argv[1]) != 0) {
            perror("Error occured!");
        }
        continue;
    }
    

    if (fork() == 0) { 
        if (strcmp(argv[0], "enviorment") == 0) { 
        execlp("env","env", NULL);
        }  else execvp(argv[0], argv);
    }
    /* parent continues here */
    if (amper == 0)
        wait(&status);
}
}
