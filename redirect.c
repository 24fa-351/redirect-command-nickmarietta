#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "helperfunctions.c"

// usage for base program: redir <inp> <cmd> <out>
// assume values for input and output are filenames! read & write to them

int main(int argc, char *argv[]) {

  // argument error handling
  if (argc < 4) {
    fprintf(stderr, "Usage: %s redir <inp> <cmd> <out>...\n", argv[0]);
    return 1;
  }

  int input_fd;
  if (strcmp(argv[1], "-") == 0) {
    // if "-", leave as stdin
    input_fd = STDIN_FILENO;
  } else {
    input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
      perror("Could not open input file");
      return 1;
    }
  }

  int output_fd;
  // if it is a -, leave as stdout
  if (strcmp(argv[3], "-") == 0) {
    output_fd = STDOUT_FILENO;
  } else {
    // argv[3] since we are targeting the output file
    output_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (output_fd == -1) {
      perror("Could not open output file");
      return 1;
    }
  }

  char absolute_path[1000];
  char *cmd[1000];

  // split the command into diff. arguments (split by spaces)
  break_into_words(argv[2], cmd, ' ');

  // check if path is messed up
  if (!find_absolute_path(cmd[0], absolute_path)) {
    fprintf(stderr, "Error: command not found %s\n", cmd[0]);
    return 0;
  }

  int child_pid = fork();
  if (child_pid == -1) {
    perror("fork error");
    return 1;
  }

  if (child_pid == 0) {
    // child process
    if (input_fd != STDIN_FILENO) {
      // this is like saying stdout = input_fd
      dup2(input_fd, STDIN_FILENO);
      close(input_fd);
    }
    if (output_fd != STDOUT_FILENO) {
      dup2(output_fd, STDOUT_FILENO);
      close(output_fd);
    }

    // NOTE: execvp would find the path too but we use execve
    execve(absolute_path, cmd, NULL);

    perror("execve failed to run command");
    // exit the child process if execve fails
    exit(1);
  }

  // wait for the child to finish
  wait(NULL);

  return 0;
}