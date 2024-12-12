#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glob.h>

int main() {
    int i, amper, retid, status, result;
    char *argv[50];
    char currCommand[1024], lastCommand[1024];
    char *command, *token;
    char promptMessg[100] = "hello";
    char **found;
	glob_t glob_struct;

    while (1) {

        fprintf(stdout, "%s: ", promptMessg);
        fgets(currCommand, 1024, stdin);
        currCommand[strlen(currCommand) - 1] = '\0'; // replace \n with \0

        if (strcmp(currCommand, "!!") == 0) {
            strcpy(currCommand, lastCommand);
        } else {
            strcpy(lastCommand, currCommand);
        }

        command = currCommand;
        /* parse command line */
        i = 0;
        token = strtok(command, " ");
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
        if (!strcmp(argv[i - 1], "&")) {
            amper = 1;
            argv[i - 1] = NULL;
            }
        else 
            amper = 0; 
        

        /* Deal with glob patterns */
        if (strcmp(argv[0], "echo") && !amper && (strchr(argv[i - 1], '*') || strchr(argv[i - 1], '?'))) {
            
            result = glob(argv[i - 1], 0 , NULL, &glob_struct);
            /* check for errors */
            if( result!=0 )
            {
                if( result==GLOB_NOMATCH )
                    fprintf(stderr,"No matches\n");
                else
                    fprintf(stderr,"Some error\n");
                exit(1);
            }
            
            argv[i - 1] = NULL;
            i--;
            found = glob_struct.gl_pathv;
            while (*found)
            {
                argv[i++] = *found;
                found++;
            }
            argv[i] = NULL;         
        
        }



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
            exit(0);
        }

        if (strcmp(argv[0], "cd") == 0) {
            if (chdir(argv[1]) != 0) {
                perror("Error occured!");
            }
            continue;
        }
        
        /* for commands not part of the shell command language */ 
        if (fork() == 0) { 
            if (strcmp(argv[0], "enviorment") == 0) { 
            execlp("env","env", NULL);
            }  else{
                if (execvp(argv[0], argv) != 0) {
                    perror("Command failed..\n");
                    exit(EXIT_FAILURE);
                }
            } 
        }
        /* parent continues here */
        if (amper == 0)
            wait(&status);
        

    }
}
