# pythonpower

This project uses the python interpreter within a C program to compute arbitrarily large exponentiation.

The program uses fork and exec to generate a child process then upgrades the child via exec to execute an interactive python interpreter which performs the actual computation. 

Projects requires two pipes to communicate with the child process: one for sending expressions to the interpreter and one for reading back the results. 
Stdin and stdout of the child process are redirected via dup calls to the proper pipe ends to receive and send data. Python process is started with the "-i" option to run the interpreter in interactive mode. 

Communication occurs via the exchange of text lines.

Program enters are loop repeating a series of steps:
  - Read in two integers of the type int from stdin. If this fails break out of the loop.
  - Generate an exponentiation expression and send it to the python process (accomplished via sprintf).
  - Read the result from the python process.
  - Print out the result to stdout.

Program cleans up any allocated resources (closes file descriptors, wait for child processes, frees dynamically allocated memory). 
