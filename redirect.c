#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

// usage for base program: redir <inp> <cmd> <out>
// assume values for input and output are filenames! read & write to them

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr,
                "Usage: %s redir <inp> <cmd> <out>...\n", argv[0]);
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
    if (strcmp(argv[3], "-") == 0)
    {
        output_fd = STDOUT_FILENO;
    }
    else
    {
        // argv[3] since we are targeting the output file
        output_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (output_fd == -1)
        {
            perror("Could not open output file");
            return 1;
        }
    }

    // split the command into diff. arguments
    char **newargv = (char **)malloc(sizeof(char *) * (argc - 2));

    // we start at 2 since we are not using the first 2 arguments
    for (int ix = 2; ix < argc; ix++)
    {
        newargv[ix - 2] = argv[ix];
    }
    newargv[argc - 3] = NULL;

    /*
    printf("Debug: Executing this command: %s\n", newargv[0]);
    for (int ix = 0; newargv[ix] != NULL; ix++)
    {
        printf("Debug: newargv[%d] = %s\n", ix, newargv[ix]);
    }
    */

    int child_pid = fork();
    if (child_pid == -1)
    {
        perror("fork error");
        return 1;
    }
    if (child_pid == 0)
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

        // need absolute path for execve so we use execvp
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