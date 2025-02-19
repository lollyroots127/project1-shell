/*
  Name: Laura Roots
  ID:   1001637158
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11    // Mav shell only supports ten arguments

#define MAX_HISTORY_SIZE 15     // The maximum commands stored in history

#define MAX_PID_HISTORY_SIZE 15 // The maximum pids stored in history

int main()
{

  char *command_string = (char*)malloc(MAX_COMMAND_SIZE); // Dynamically allocate memory 
                                                          // to store user commands
  char *history[MAX_HISTORY_SIZE] = {NULL};               // Store the max number of commands
  pid_t pid_history[MAX_PID_HISTORY_SIZE] = {};           // Store max number of spawned child 
                                                          // processes
  int history_count = 0, pid_count = 0;

  while (1) {
    // Print out the msh prompt
    printf("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(command_string, MAX_COMMAND_SIZE, stdin));

    // If no command entered, repeat loop until proper input is detected
    if (command_string[0] == '\n') continue;

    // If the user enters the "!n" command, verify that it is an available command and then
    // Overwrite the command string and continue with execution.
    // Note: since the "!n" command deals with history, it is implemented before history is saved.
    // History will now contain the copied and executed command rather than !n.
    if (command_string[0] == '!') {
      int n = atoi(&command_string[1]);
      if (n < 0 || n >= history_count) {
        printf("Command not in history.\n");
        continue;
      } else {
        strcpy(command_string, history[n]);
      }
    }

    // Store the current command in history
    // Do this before evaluating commands to ensure they are stored, including invalid commands
    // Empty commands are not stored
    if (history_count < 15) {
      history[history_count] = strdup(command_string);
      history_count++;
    } else { 
      // If maximum history is already reached, free oldest command and shift array to only
      // show the most recent commands
      free(history[0]); 
      for (int i = 1; i < MAX_HISTORY_SIZE; i++) {
        history[i - 1] = history[i];
      }
      history[MAX_HISTORY_SIZE - 1] = strdup(command_string);
    }
    
    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_string  = strdup(command_string);                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while (((argument_ptr = strsep(&working_string, WHITESPACE)) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS) ) {
      token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0) {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Check if command is to exit or quit, then free memory and exit
    if (strcmp(token[0], "exit") == 0 || strcmp(token[0], "quit") == 0) {
      printf("Exiting msh...\n");
      free(command_string);
      for (int i = 0; i < history_count; i++) {
        free(history[i]);
      }
      exit(0);
    }

    // Implement the "cd" command in the parent process using chdir because it affects the 
    // current working directory and would be lost in a child process once it is terminated.
    if (strcmp(token[0], "cd") == 0) {
      if (token[1] == NULL) { // Verify that a location was provided
          printf("cd: no directory provided\n");
      } 
      else {
        // token[1] contains the location and if ".." was entered, chdir is able to change directory
        // to the parent directory of our current process
        if (chdir(token[1]) != 0) {
            perror("cd failed");
        }
      }
      continue;
    }
        
    // Implement "history" command to display the last commands entered, up to history_count, which
    // will not surpass 15.
    if (strcmp(token[0], "history") == 0) {
      for (int i = 0; i < history_count; i++) {
        printf("%d: %s", i, history[i]);
      }
      continue;
    }

    // Implement "showpids" command to display last 15 process IDs. These are created when a child
    // process is created and does not include the current (parent) process.
    if (strcmp(token[0], "showpids") == 0) {
      for (int i = 0; i < pid_count; i++) {
        printf("%d: %d\n", i, pid_history[i]);
      }
      continue;
    }

    // Execute external commands through the child process.
    pid_t pid = fork();
    if (pid == 0) {
      // If pid is 0, then we are in the child process and can execute the command
      int ret = execvp(token[0], &token[0]);  
      if (ret == -1) {
        printf("%s: Command not found.\n", token[0]);
        exit(1);
      }
    } else if (pid == -1) {
      perror("fork failed");
    } else {
      // In the parent process, store the child pid and wait until the child process finishes
      // execution of the command.
      // If max pid history is reached, then shift pid history over to include most recent command.
      if (pid_count < 15) {
        pid_history[pid_count] = pid;
        pid_count++;
      } else {
        for (int i = 1; i < MAX_PID_HISTORY_SIZE; i++) {
          pid_history[i-1] = pid_history[i];
        }
        pid_history[MAX_HISTORY_SIZE - 1] = pid;
      }
      int status;
      wait(&status);
    }

    free(head_ptr); // Free allocated memory.

  }
  return 0;
}
