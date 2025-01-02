#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>

void executeCommand(char *command);
int parseCommand(char *command, char *argv[]);
void redirectCommand(char *argv[], int argc);

int main()
{
    char command[1024];

    while (1)
    {
        printf("hello: ");
        fflush(stdout);
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0';

        executeCommand(command);
    }
}

void executeCommand(char *command)
{
    char *argv[10];
    int amper = 0;
    int argc = parseCommand(command, argv);
    if (argv[0] == NULL)
        return;
    if (!strcmp(argv[argc - 1], "&"))
    {
        amper = 1;
        argv[argc - 1] = NULL;
    }
    pid_t cpid = fork();
    if (cpid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (cpid == 0) // child
    {
        redirectCommand(argv, argc);
        execvp(argv[0], argv);
        perror("execvp");
        exit(1);
    }
    // parent continues here
    if (amper == 0)
        waitpid(cpid, NULL, 0);
}

int parseCommand(char *command, char *argv[])
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

void redirectCommand(char *argv[], int argc)
{
    if (argc < 3)
        return;
    char *arg = argv[argc - 2];
    char *fn = argv[argc - 1];
    int fd = -1;
    if (!strcmp(arg, ">"))
    {
        printf("stdout");
        fd = open(fn, O_CREAT | O_WRONLY | O_TRUNC, 0660);
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
    else if (!strcmp(arg, "2>"))
    {
        printf("stderror");
        fd = open(fn, O_CREAT | O_WRONLY | O_TRUNC, 0660);
        if (fd < 0)
        {
            perror("open");
            exit(1);
        }
        if (dup2(fd, STDERR_FILENO) < 0)
        {
            perror("dup2");
            exit(1);
        }
    }

    else if (!strcmp(arg, "<"))
    {
        printf("stdin");
        fd = open(fn, O_RDONLY);
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
    }
    else if (!strcmp(arg, ">>"))
    {
        printf("stdout_app");
        fd = open(fn, O_CREAT | O_WRONLY | O_APPEND, 0660);
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
    if (fd > 0)
    {
        argv[argc - 2] = NULL;
        if (close(fd) < 0)
        {
            perror("close");
            exit(1);
        }
    }
}
