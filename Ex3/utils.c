#include "utils.h"

void executeCommand(char *argv[], int argc)
{
    redirectCommand(argv, argc);
    execvp(argv[0], argv);
    perror("execvp");
    exit(1);
}

int parseCommand(char *command, char *argv[], int *amperFlag)
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
    if (amperFlag)
        *amperFlag = isAmper(argv, i);
    return i;
}

int isAmper(char *argv[], int argc)
{
    if (!strcmp(argv[argc - 1], "&"))
    {
        argv[argc - 1] = NULL;
        return 1;
    }
    return 0;
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
    // make an exception for the 'echo' command
    char first[20] = {0};
    if(strlen(raw) >= 5) strncpy(first, raw, 4);
    if (strcmp(first, "echo") == 0) return 1;
    
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
    int pfd[2], i, argc, prev_fd = -1;
    char *argv[10];

    for (i = 0; i < pipec; i++)
    {
        // No need for amper-flag, parent waits for all childs i pipe
        argc = parseCommand(pipeCommands[i], argv, NULL);

        if (argv[0] == NULL)
            continue;

        if (pipe(pfd) < 0)
        {
            perror("pipe");
            exit(1);
        }

        pid_t cpid = fork();
        if (cpid < 0)
        {
            perror("fork");
            exit(1);
        }
        else if (cpid == 0)
        {
            // Child process
            if (prev_fd != -1)
            {
                if (dup2(prev_fd, STDIN_FILENO) < 0) // Redirect stdin to the previous pipe
                {
                    perror("dup2");
                    exit(1);
                }
                if (close(prev_fd) < 0)
                {
                    perror("close");
                    exit(1);
                }
            }
            if (i != pipec - 1)
            {
                if (dup2(pfd[1], STDOUT_FILENO) < 0) // Redirect stdout to the current pipe
                {
                    perror("dup2");
                    exit(1);
                }
            }
            if (close(pfd[0]) < 0) // Close unused read-end of the current pipe
            {
                perror("close");
                exit(1);
            }
            if (close(pfd[1]) < 0)
            {
                perror("close");
                exit(1);
            }

            executeCommand(argv, argc);
        }

        waitpid(cpid, NULL, 0);
        // Parent process
        if (close(pfd[1]) < 0) // Close unused write-end of the current pipe
        {
            perror("close");
            exit(1);
        }
        if (prev_fd != -1)
        {
            if (close(prev_fd) < 0) // Close the previous pipe's read-end
            {
                perror("close");
                exit(1);
            }
        }
        prev_fd = pfd[0]; // Update prev_fd to the current pipe's read-end
    }
}

void handler()
{
    printf("You typed Control-C!\n");
}
