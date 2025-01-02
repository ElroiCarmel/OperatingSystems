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

void executeCommand(char *command, int isPiped);
void executePipeline(char *pipeCommands[], int pipec);
int parseCommand(char *command, char *argv[]);
int parsePipeline(char *raw, char *pipes[]);
void redirectCommand(char *argv[], int argc);
void handler();

int main()
{
    char command[1024];
    char *pipes[15];
    int pipec;

    signal(SIGINT, handler);

    while (1)
    {
        printf("hello: ");
        fflush(stdout);
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0';

        pipec = parsePipeline(command, pipes);
        if (pipec > 1)
        {
            executePipeline(pipes, pipec);
            continue;
        }

        executeCommand(command, 0);
    }
}

void executeCommand(char *command, int isPiped)
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
    if (!isPiped)
    {
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
    else
    {
        redirectCommand(argv, argc);
        execvp(argv[0], argv);
        perror("execvp");
        exit(1);
    }
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

int parsePipeline(char *raw, char *pipes[])
{
    int count = 0;
    char *token;
    token = strtok(raw, "|");
    while (token != NULL)
    {
        pipes[count] = token;
        token = strtok(NULL, "|");
        count++;
    }
    return count;
}

void executePipeline(char *pipeCommands[], int pipec)
{
    int pfd[2], i;
    int prev_fd = -1; // Previous pipe's read-end

    for (i = 0; i < pipec; i++)
    {
        if (pipe(pfd) < 0)
        {
            perror("pipe");
            exit(1);
        }

        pid_t cpid = fork();
        if (cpid == 0)
        {
            // Child process
            if (prev_fd != -1)
            {
                dup2(prev_fd, STDIN_FILENO); // Redirect stdin to the previous pipe
                close(prev_fd);
            }
            if (i != pipec - 1)
            {
                dup2(pfd[1], STDOUT_FILENO); // Redirect stdout to the current pipe
            }
            close(pfd[0]); // Close unused read-end of the current pipe
            close(pfd[1]);

            executeCommand(pipeCommands[i], 1);
        }

        waitpid(cpid, NULL, 0);
        // Parent process
        close(pfd[1]); // Close unused write-end of the current pipe
        if (prev_fd != -1)
        {
            close(prev_fd); // Close the previous pipe's read-end
        }
        prev_fd = pfd[0]; // Update prev_fd to the current pipe's read-end
    }
}

void handler()
{
    printf("You typed Control-C!\n");
}
