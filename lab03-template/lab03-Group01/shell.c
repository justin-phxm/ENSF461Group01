#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "parser.h"

#define BUFLEN 1024

int main()
{
    char buffer[BUFLEN];
    char *parsedinput;

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
            free(parsedinput);
            return 0;
        }
        
        pid_t forkV = fork();
        if (forkV == 0) {
            int argCount = 0;
            char **args_p = parseInput(parsedinput, &argCount);

            // Check for pipes
            int isPipe = 0;
            int pipePos = -1;
            for (int i = 0; i < argCount; i++) {
                if (strcmp(args_p[i], "|") == 0) {
                    isPipe = 1;
                    pipePos = i;
                    break;
                }
            }

            if (isPipe) {
                int fd[2];  // file descriptors for the pipe
                if (pipe(fd) == -1) {
                    perror("pipe");
                    exit(-1);
                }

                pid_t pid = fork();
                if (pid == 0) {  // First command in child
                    close(fd[0]); // Close read end
                    dup2(fd[1], STDOUT_FILENO);  // Redirect stdout to the pipe
                    close(fd[1]);

                    args_p[pipePos] = NULL;  // terminate the list of arguments for the first command
                    char *absolutePathCommand1 = getcommand(args_p[0]);
                    if (execve(absolutePathCommand1, args_p, NULL) == -1) {
                        fprintf(stderr, "Error running first command in execve\n");
                        exit(-100);
                    }
                    free(absolutePathCommand1);
                } else {
                    wait(NULL);  // Wait for first command to complete

                    close(fd[1]);  // Close write end
                    dup2(fd[0], STDIN_FILENO);  // Redirect stdin to get input from the pipe
                    close(fd[0]);

                    char *absolutePathCommand2 = getcommand(args_p[pipePos + 1]);
                    if (execve(absolutePathCommand2, &args_p[pipePos + 1], NULL) == -1) {
                        fprintf(stderr, "Error running second command in execve\n");
                        exit(-100);
                    }
                    free(absolutePathCommand2);
                }
            } else {
                char *command = firstwordpointer(parsedinput);
                char *absolutePathCommand = getcommand(command);

                if (execve(absolutePathCommand, args_p, NULL) == -1) {
                    fprintf(stderr, "Error running command in execve\n");
                    exit(-100);
                }
                free(command);
                free(absolutePathCommand);
            }
            for (int i = 0; i < argCount; i++) {
                free(args_p[i]);
            }
            free(args_p);

        } else {
            wait(NULL);
        }

        // Remember to free any memory you allocate!
        
        free(parsedinput);
    } while (1);

    return 0;
}
