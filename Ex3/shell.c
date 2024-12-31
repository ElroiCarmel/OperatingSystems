#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>

#define RED_STDIN 1
#define RED_STDOUT 2
#define RED_STDERROR 4
#define RED_STDOUT_APP 8

int main()
{
    char command[1024];
    char *token;
    char *outfile, *infile;
    int i, fd, amper, retid, status;
    unsigned char redirect = 0;
    char *argv[10];

    while (1)
    {
        printf("hello: ");
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0';

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
        if (!strcmp(argv[i - 1], "&"))
        {
            amper = 1;
            argv[i - 1] = NULL;
        }
        else
            amper = 0;

        if (argv[i - 2] && !strcmp(argv[i - 2], ">"))
        {
            redirect |= RED_STDOUT;
            argv[i - 2] = NULL;
            outfile = argv[i - 1];
        }
        else
            redirect &= ~RED_STDOUT;

        if (argv[i - 2] && !strcmp(argv[i - 2], "2>"))
        {
            redirect |= RED_STDERROR;
            argv[i - 2] = NULL;
            outfile = argv[i - 1];
        }
        else
            redirect &= ~RED_STDERROR;

        if (argv[i - 2] && !strcmp(argv[i - 2], "<"))
        {
            redirect |= RED_STDIN;
            argv[i - 2] = NULL;
            infile = argv[i - 1];
        }
        else
            redirect &= ~RED_STDIN;

        if (argv[i - 2] && !strcmp(argv[i - 2], ">>"))
        {
            redirect |= RED_STDOUT_APP;
            argv[i - 2] = NULL;
            outfile = argv[i - 1];
        }
        else
            redirect &= ~RED_STDOUT_APP;

        /* for commands not part of the shell command language */

        pid_t childPid = fork();
        if (childPid < 0)
        {
            perror("fork");
            exit(1);
        }
        if (childPid == 0)
        {
            /* redirection of IO ? */
            if (redirect & RED_STDOUT)
            {
                fd = creat(outfile, 0660);
                if (fd < 0)
                {
                    perror("creat");
                    exit(1);
                }
                if (dup2(fd, STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    exit(1);
                }
                if (close(fd) < 0)
                {
                    perror("close");
                    exit(1);
                }
                /* stdout is now redirected */
            }
            if (redirect & RED_STDERROR)
            {
                fd = creat(outfile, 0660);
                if (fd < 0)
                {
                    perror("creat");
                    exit(1);
                }
                if (dup2(fd, STDERR_FILENO) < 0)
                {
                    perror("dup2");
                    exit(1);
                }
                if (close(fd) < 0)
                {
                    perror("close");
                    exit(1);
                }
                /* stderror is now redirected */
            }
            if (redirect & RED_STDIN)
            {
                fd = open(infile, O_RDONLY);
                if (fd < 0)
                {
                    perror("open");
                    exit(1);
                }
                if (dup2(fd, STDIN_FILENO) < 0)
                {
                    perror("dup2");
                    exit(1);
                }
                if (close(fd) < 0)
                {
                    perror("close");
                    exit(1);
                }
                /* stdin is now redirected */
            }
            if (redirect & RED_STDOUT_APP)
            {
                fd = open(outfile, O_CREAT|O_APPEND|O_WRONLY, 0660);
                if (fd < 0)
                {
                    perror("open");
                    exit(1);
                }
                if (dup2(fd, STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    exit(1);
                }
                if (close(fd) < 0)
                {
                    perror("close");
                    exit(1);
                }
                /* stdout is now redirected */
            }
            
            execvp(argv[0], argv);
        }
        /* parent continues here */
        if (amper == 0)
            retid = wait(&status);
    }
}
