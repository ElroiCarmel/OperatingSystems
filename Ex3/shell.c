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

void readRedirect(char **args, int argc, unsigned char *flag, char **outfile);
void execRedirect(unsigned char flag, char *fname);
int parseCommand(char *command, char **argv);
int parsePipes(char *input, char**pipes);

int main()
{
    char command[1024];
    char *token;
    char *file_redirect;
    int i, fd, amper, retid, status, pipec;
    unsigned char redirect;
    char *argv[10], *pipes[16];

    while (1)
    {
        printf("hello: ");
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0';

        pipec = parsePipes(command, pipes);

        /* parse command line */
        i = parseCommand(command, argv);
        

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

        redirect = 0;
        readRedirect(argv, i, &redirect, &file_redirect);
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
            execRedirect(redirect, file_redirect);
            execvp(argv[0], argv);
        }
        /* parent continues here */
        if (amper == 0)
            retid = wait(&status);
    }
}

void readRedirect(char **argv, int argc, unsigned char *flag, char **file_name)
{
    if (argv[argc - 2] && !strcmp(argv[argc - 2], ">"))
    {
        *flag |= RED_STDOUT;
        argv[argc - 2] = NULL;
        *file_name = argv[argc - 1];
    }
    else if (argv[argc - 2] && !strcmp(argv[argc - 2], "2>"))
    {
        *flag |= RED_STDERROR;
        argv[argc - 2] = NULL;
        *file_name = argv[argc - 1];
    }
    else if (argv[argc - 2] && !strcmp(argv[argc - 2], "<"))
    {
        *flag |= RED_STDIN;
        argv[argc - 2] = NULL;
        *file_name = argv[argc - 1];
    }
    else if (argv[argc - 2] && !strcmp(argv[argc - 2], ">>"))
    {
        *flag |= RED_STDOUT_APP;
        argv[argc - 2] = NULL;
        *file_name = argv[argc - 1];
    }
}

void execRedirect(unsigned char flag, char *fname)
{
    int fd = -1;
    if (flag & RED_STDOUT)
    {
        fd = creat(fname, 0660);
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

        /* stdout is now redirected */
    }
    else if (flag & RED_STDERROR)
    {
        fd = creat(fname, 0660);
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

        /* stderror is now redirected */
    }
    else if (flag & RED_STDIN)
    {
        fd = open(fname, O_RDONLY);
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

        /* stdin is now redirected */
    }
    else if (flag & RED_STDOUT_APP)
    {
        fd = open(fname, O_CREAT | O_APPEND | O_WRONLY, 0660);
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
    }
    if (flag && fd && close(fd) < 0)
    {
        perror("close");
        exit(1);
    }
}

int parseCommand(char *command, char **argv)
{
    int i = 0;
    char *token;
    token = strtok(command, " ");
    while (token != NULL)
    {
        argv[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    argv[i] = NULL;
    return i;
}

int parsePipes(char *input, char**pipes)
{
    int i = 0;
    char *token;
    token = strtok(input, "|");
    while (token != NULL)
    {
        pipes[i] = token;
        token = strtok(NULL, "|");
        i++;
    }
    return i;
}
