# CS344_Assignment3
This assignment creates a custom-made shell that the user can fully use

***NOTICE***
This assignment has not been implemented completely

Here's what's been implemented:
	1. command prompt
	2. handles comments
	3. provides expansion of $$
	4. handles 'cd', 'exit', 'status'
	5. use of execvp and execlp
	6. file redirection

What is not implementd:
	1. background/foreground processes
	2. signals

TO COMPILE:

gcc -std=c99 linkedlist.c main.c -o smallsh

TO RUN:
	./p3testscript

	
***NOTICE***
Because of what's been implemented, the script is unable to run to completeion
(gets hung up on testing the foreground only mode). Since I haven't implemented
signals, I ^C to get out of this
