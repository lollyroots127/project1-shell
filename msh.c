// The MIT License (MIT)
// 
// Copyright (c) 2016 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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

#define MAX_NUM_ARGUMENTS 11     // Mav shell only supports ten arguments

#define MAX_HISTORY_SIZE 15     // The maximum commands stored in history

#define MAX_PID_HISTORY_SIZE 15 // The maximum pids stored in history

int main()
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
  char * history[MAX_HISTORY_SIZE] = {};
  char * pid_history[MAX_PID_HISTORY_SIZE] = {};
  int history_count = 0, pid_count = 0;

  while ( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while ( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    // Store command in history
    history[history_count] = strdup(command_string);
    history_count++;

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if ( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Print the tokenized input as a debug check
    int token_index  = 0;
    for ( token_index = 0; token_index < token_count; token_index ++ ) 
    {
      printf( "token[%d] = %s\n", token_index, token[token_index] );
    }
    
    // If no command entered, repeat loop until proper input is detected
    if ( token[0] == NULL) continue;

    // Check if command is to exit or quit, then free memory and exit
    if ( strcmp(token[0], "exit" ) == 0 || strcmp( token[0], "quit" ) == 0 ) 
    {
      printf( "Exiting msh...\n" );
      free( command_string );
      exit( 0 );
    }

    // TODO: Implement "cd" command (must use chdir in the parent process)
        
    // TODO: Implement "mkdir" command using mkdir() system call
        
    // TODO: Implement "rm" command using unlink() system call
        
    // TODO: Implement "ls" command using fork() and execvp()
        
    // TODO: Implement "pidhistory" command to display last 15 process IDs
        
    // TODO: Implement "history" command to display last 15 user commands
        
    // TODO: Implement "!n" to execute nth command from history


    free( head_ptr );

  }
  return 0;
  // e2520ca2-76f3-90d6-0242ac120003
}
