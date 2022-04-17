/*
We want to read in pairs of integers n and e from stdin and compute n^e.
We use fork and exec to create a child process then upgrade the child process to execute
an interactive python interpreter that does the actual computation.

Power-in-python must create two pipes to communicate with the child process, one for sending expressions to python and one for reading 
back the results. 

The stdin and stdout of the child process should be redirected to the proper pipe ends.
The stderr of the child python process should be redirected to the special file /dev/null.

The communications between the power-in-python process and the python process are done by text lines,
each ending with a newline. 
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
   char *cmd = "python3";
   int parenttochild[2], childtoparent[2]; // two element arrays for pipes
   pid_t pid;

   // two pipes, one for sending from parent to child and one for sending from child to parent
   pipe(parenttochild);
   pipe(childtoparent);
   
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
      
      // close the other pipe ends
      close(parenttochild[0]);
      close(childtoparent[1]);
      close(devnull);

      // call the python3 command
      execlp(cmd, cmd, "-i", NULL);

   } else {
      int number1, number2;

      close(parenttochild[0]); // closing the parent to child read end
      close(childtoparent[1]); // closing the child to parent write end

      while (1) {

         // get input so long as the user is entering it 
         if (scanf("%d %d", &number1, &number2) != 2)
            break; // break out

         char bufferout[MAXLINE]; // declare a buffer for reading out
         char bufferin[MAXLINE] = {0};

         sprintf(bufferout, "(%d) ** (%d)\n", number1, number2);

         write(parenttochild[1], bufferout, strlen(bufferout));

         read(childtoparent[0], bufferin, 1024);

         printf("(%d) ** (%d) = %s", number1, number2, bufferin);
      }

      close(parenttochild[1]);
      close(childtoparent[0]);

      // wait for child processes
      int status;
      waitpid(pid, &status, 0);
      printf("Child exit status: %d\n", status);

   }
   return 0;
}
