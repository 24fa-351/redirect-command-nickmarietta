#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

// usage for base program: redirect <input> <output> <cmd>
// assume values for input and output are filenames! read & write to them

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr,
                "Usage: %s <input> <output> <cmd>...\n", argv[0]);
        return 1;
    }

    int input_fd;
    if (strcmp(argv[1], "-") == 0)
    {
        // if "-", leave them as stdin, stdout
        input_fd = STDIN_FILENO;
    }
    else
    {
        input_fd = open(argv[1], O_RDONLY);
        if (input_fd == -1)
        {
            perror("Could not open input file");
            return 1;
        }
    }

    int output_fd;
    if (strcmp(argv[2], "-") == 0)
    {
        output_fd = STDOUT_FILENO;
    }
    else
    {
        output_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (output_fd == -1)
        {
            perror("Could not open output file");
            return 1;
        }
    }

    // split the command into diff. arguments
    char **newargv = (char **)malloc(sizeof(char *) * (argc - 2));
    newargv[0] = argv[3];
    for (int ix = 3; ix < argc; ix++)
    {
        newargv[ix - 3] = argv[ix];
    }
    newargv[argc - 3] = NULL;

    printf("Debug: Executing this command: %s\n", newargv[0]);
    for (int ix = 0; newargv[ix] != NULL; ix++)
    {
        printf("Debug: newargv[%d] = %s\n", ix, newargv[ix]);
    }

    int child_pid = fork();
    if (child_pid == -1)
    {
        perror("fork error");
        return 1;
    }
    else if (child_pid == 0)
    {
        // child process
        if (input_fd != STDIN_FILENO)
        {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        if (output_fd != STDOUT_FILENO)
        {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        // need absolute path for execve
        execvp(newargv[0], newargv);
        // permission denied here
        /*for (int ix = 0; newargv[ix] != NULL; ix++)
        {
            printf("newargv[%d] = %s\n", ix, newargv[ix]);
        }
         HERE: added read and write permissions to files
        */

        perror("execvp failed to run command");
        // exit the child process if execvp fails
        exit(1);
    }
    if (input_fd != STDIN_FILENO)
    {
        close(input_fd);
    }
    if (output_fd != STDOUT_FILENO)
    {
        close(output_fd);
    }

    // wait for the child to finish
    wait(NULL);

    // free the allocated memory
    free(newargv);
    return 0;
}