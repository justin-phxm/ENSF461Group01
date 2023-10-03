#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "parser.h"

#define BUFLEN 1024

// To Do: This base file has been provided to help you start the lab, you'll need to heavily modify it to implement all of the features

int main()
{
    char buffer[1024];
    char *parsedinput;
    char *args[3];
    char newline;

    printf("Welcome to the Group01 shell! Enter commands, enter 'quit' to exit\n");
    do
    {
        // Print the terminal prompt and get input
        printf("$ ");
        char *input = fgets(buffer, sizeof(buffer), stdin);
        if (!input)
        {
            fprintf(stderr, "Error reading input\n");
            return -1;
        }

        // Clean and parse the input string
        parsedinput = (char *)malloc(BUFLEN * sizeof(char));
        size_t parselength = trimstring(parsedinput, input, BUFLEN);

        // Sample shell logic implementation
        if (strcmp(parsedinput, "quit") == 0)
        {
            printf("Bye!!\n");
            return 0;
        }
        else
        {
            pid_t forkV = fork();
            if (forkV == 0)
            {
                char *command = firstwordpointer(parsedinput);
                char *absolutePathCommand = getcommand(command);
                int argCount = 0;
                char **args_p = parseInput(parsedinput, &argCount);
                args[0] = command;
                // args[0] = "echo";
                // args[0] = "/usr/bin/echo";
                // args[1] = parsedinput;

                args[1] = args_p[1];
                // args[2] = NULL;
                if (execve(absolutePathCommand, args_p, NULL) == -1)
                {
                    fprintf(stderr, "Error running command in execve\n");
                    return -100;
                }
            }
            else
                wait(NULL);
        }

        // Remember to free any memory you allocate!
        free(parsedinput);
    } while (1);

    return 0;
}
