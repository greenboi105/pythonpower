/*
We want to read in pairs of integers n and e from stdin and compute n^e.
We use fork and exec to create a child process then upgrade the child process to execute
an interactive python interpreter that does the actual computation.

Power-in-python must create two pipes to communicate with the child process, one for sending expressions to python and one for reading back the results. 

The stdin and stdout of the child process should be redirected to the proper pipe ends.
The stderr of the child python process should be redirected to the special file /dev/null.

The communications between the power-in-python process and the python process are done by text lines, each ending with a newline. 

Header files used include files for I/O, UNIX system calls, error checking, and string functions.
*/

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 1024

int main(int argc, char ** argv)
{
   // the command we want to use with exec to run the python interpreter in the child process 
   char *cmd = "python3";
   
   // two element arrays for pipes
   int parenttochild[2], childtoparent[2]; 
   
   // the process ID to determine if we are in the child or parent 
   pid_t pid;

   // two pipes, one for sending from parent to child and one for sending from child to parent
   pipe(parenttochild);
   pipe(childtoparent);
   
   // fork a child process 
   pid = fork();

   // entering the child process and preparing it for the exec call
   if (pid == 0) {

      // open the devnull file
      int devnull = open("/dev/null", O_WRONLY);

      // close unneeded file descriptors
      close(parenttochild[1]); 
      close(childtoparent[0]); 

      /// redirect the stdin, stdout and stderr
      dup2(parenttochild[0], 0);
      dup2(childtoparent[1], 1);
      dup2(devnull, 2);
      
      // close the other pipe ends once the redirection is complete 
      close(parenttochild[0]);
      close(childtoparent[1]);
      close(devnull);

      // call the python3 command with execlp 
      execlp(cmd, cmd, "-i", NULL);

   } else {
      int number1, number2;

      close(parenttochild[0]); // closing the parent to child read end
      close(childtoparent[1]); // closing the child to parent write end

      while (1) {

         // If the user does not enter input in the form of two numbers, break out of the loop due to invalid input formatting 
         if (scanf("%d %d", &number1, &number2) != 2)
            break; // break out
         
         // declare a buffer for sending information out 
         char bufferout[MAXLINE]; 
         
         // declare a buffer for reading in information; buffer is initialized to all zeroes 
         char bufferin[MAXLINE] = {0};
         
         // deposit formatted string into the buffer for writing information to the child 
         sprintf(bufferout, "(%d) ** (%d)\n", number1, number2);
      
         // send the formatted string through the pipe to the child 
         // parameters of the write system call are the file descriptor (write end of the pipe), the buffer whose contents are being sent and the size of the contents
         write(parenttochild[1], bufferout, strlen(bufferout));
         
         // read what was sent back from the child using the read end of the pipe, the buffer for storing what was read and the max potential size 
         read(childtoparent[0], bufferin, 1024);
         
         // print what was received back 
         printf("(%d) ** (%d) = %s", number1, number2, bufferin);
      }
      
      // close unneeded file descriptors 
      close(parenttochild[1]);
      close(childtoparent[0]);

      // wait for child processes
      int status;
      waitpid(pid, &status, 0);
      printf("Child exit status: %d\n", status);

   }
   return 0;
}
