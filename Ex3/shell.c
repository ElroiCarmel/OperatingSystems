#include "utils.h"

int main()
{
    char command[1024];
    char *pipes[15], *argv[15];
    int pipec, amper, argc;
    pid_t cpid;

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
        }
        else
        {
            argc = parseCommand(command, argv, &amper);
            
            if (argv[0] == NULL)
                continue;

            cpid = fork();

            if (cpid < 0)
            {
                perror("fork");
                exit(1);
            }
            else if (cpid == 0) // child
            {
                executeCommand(argv, argc);
            }
            if (amper == 0)
                waitpid(cpid, NULL, 0);
        }
    }
}

