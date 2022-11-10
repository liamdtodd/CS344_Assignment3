/********************
 * Liam Todd
 * This is the main code file that runs the shell
 * *****************/
#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

void checkDollar(char*);
int checkComment(struct Linked_List*);
int locateInIndex(char**, int);
int locateOutIndex(char**, int);
void file_redirect(char**, int);
int execute(char**);
int callFork(char*, struct Linked_List*);
void changeDIR(char*);
int parseInput(struct Linked_List*, char*, int);

int main() {
	char* readstr = malloc(2048 * sizeof(char));
	memset(readstr, '\0', 2048);		//initialize values for readstr

	int status = -5;
	size_t buffer_size = 2048;
	size_t bytes_read = -5;			//variables needed for getline command

	while (1) {
		memset(readstr, '\0', 2048);	//reset the readstr for each pass through loop
		printf(": ");
		fflush(stdout);
		bytes_read = getline(&readstr, &buffer_size, stdin);

		if (strncmp(readstr, "exit", 4) == 0)
			exit(0);

		struct Linked_List* list = malloc(sizeof(struct Linked_List));
		list->head = NULL;
		list->tail = NULL;
		list->length = 0;		//initialize linked list

		status = parseInput(list, readstr, status);

		free_listelements(list);
		free(list);
		list = NULL;			//free dynamic mem for next line of input
	}


	free(readstr);
	readstr = NULL;				//free dynamic mem
	return 0;
}

//this function will change to the desired directory
void changeDIR(char* desiredDIR) {
	DIR* directory = opendir(".");

	chdir(desiredDIR);

	closedir(directory);
}

//this function will check to see if the command entered by the user is a comment if yes, ret 0, else ret 2
int checkComment(struct Linked_List* list) {
	char* substr = NULL;
	substr = strstr(list->head->readstr, "#");
	if (substr != NULL)
		return 0;			//if substr is NULL, there is no '#' in the command, so it's not a comment

	return 2;
}

//this function will check to see if there is '$$', if there is, it will replace the index's value with pidstr
void checkDollar(char* arg) {
	int thePid = getpid();
	char noDollar[50];
	char pidstr[50];
	memset(noDollar, '\0', 50);
	memset(pidstr, '\0', 50);

	if (strstr(arg, "$") != NULL) {
		if (strlen(arg) == 2) {
			sprintf(pidstr, "%d", thePid);		//for string = "$$", write pid in place of dollar
			strcpy(arg, pidstr);
		}
		else {
			strncpy(noDollar, arg, strlen(arg) - 2);	//copy string minus "$$"
			sprintf(pidstr, "%s%d", noDollar, thePid);	//append pid to string
			strcpy(arg, pidstr);				//copy to array's indexed value
		}
	}
}

//this function will parse through the user's input and store each string of chars b/w spaces into a node of a linkedlist
//then the function will call the corresponding execution (cd, status, or excelvp)
int parseInput(struct Linked_List* list, char* dataline, int status) {
	char* token = NULL;
	char* tknptr = NULL;

	token = strtok_r(dataline, "\n", &tknptr);	//remove 'enter' so strtok_r can read last string entered by user

	char* line = malloc((strlen(dataline) + 1) * sizeof(char));
	strcpy(line, dataline);				//save contents of dataline (w/out "\n") for later use
	
	token = strtok_r(dataline, " ", &tknptr);	//now tokenize string by spaces
	while (token != NULL) {
		add_back(list, token);			//make token a node in a linked list
		token = strtok_r(NULL, " ", &tknptr);
	}						//will loop through entire string until there are no more string vals

	if (strcmp(list->head->readstr, "cd") == 0) {
		if (list->length == 1)
			changeDIR("/nfs/stak/users/toddl/");	//"HOME" env var value
		else
			changeDIR(list->head->next->readstr);	//reading first value of linkedlist (has first command from user)
	}

	else if (strcmp(list->head->readstr, "status") == 0) {
		printf("exit status: %d\n", status);
		fflush(stdout);
	}
	else {
		status = checkComment(list);
		if (status == 0)
			return 0;			//command is a comment, do not fork or exec

		status = callFork(line, list);
	}

	return status;		
}

//this function will fork the processes and then call an exec function to run the user's command
int callFork(char* dataline, struct Linked_List* list) {
	int status = -5;
	char* pidstr = NULL;
	char* args[list->length];
	struct node* temp = list->head;
	int x = 0;
	while (temp != NULL) {
		args[x] = malloc((strlen(temp->readstr) + 1) * sizeof(char));
		strcpy(args[x], temp->readstr);
		temp = temp->next;
		x++;
	}						//storing contents of linkedlist into args[] for execvp() call
	temp = list->head;
	x = 0;

	int PID = fork();
	switch (PID) {				

		case -1: {
			return 1;
		}					//failed fork()

		case 0: {				//CHILD PROCESS
			if (list->length > 1) 
				file_redirect(args, list->length);
			
			for (int x = 0; x < list->length; x++)
				checkDollar(args[x]);

			execute(args);

			printf("%s: not a file or directory\n", list->head->readstr); fflush(stdout);
			exit(1);		//these lines will only run if execute() fails, terminates child
		}

		default: {				//PARENT process
			pid_t parentPID = waitpid(PID, &status, 0);
		
			for (x = 0; x < list->length; x++)
				free(args[x]);
		
			if (status != 0)
				return 1;
			return 0;
		}
	}

	return 1;					//fork did not fail, exec previously called
}

//this function will call execvp() and return the status of that call
int execute(char** argv) {
	if (execvp(*argv, argv) < 0)
		return -1;
	return 0;					//this line should not run
}

//this function will check to see if file redirection is being requested and then redirect 
//file i/o if it was requested
void file_redirect(char** args, int length) {
	int input = -5;					//the file descriptor for the input/read file
	int output = -5;				//the file descriptor for the output/write file
	int result = -5;				//used for result of dup2() call

	int outIndex = locateOutIndex(args, length);
	int inIndex = locateInIndex(args, length);
	
	if ((outIndex > 0) && (inIndex < 0)) {
	
		output = open(args[outIndex+1], O_WRONLY | O_CREAT | O_EXCL, 0644);	//create file if not already existing
		if (output < 0) 
			output = open(args[outIndex+1], O_WRONLY | O_TRUNC, 0644);	//assumes file previously exists, open file	

		result = dup2(output, 1);			//change stdout to point to file

		fcntl(output, F_SETFD, FD_CLOEXEC);

		if (execlp(args[outIndex-1], args[outIndex-1], NULL) < 0)
			exit(1);				//this line won't run if execlp is successful

	} else if ((outIndex < 0) && (inIndex > 0)) {

		input = open(args[inIndex+1], O_RDONLY, 0644);	//file should already exist		
		if (input < 0)
			printf("%s: does not exist\n", args[inIndex+1]); fflush(stdout);

		fcntl(input, F_SETFD, FD_CLOEXEC);

		if (execlp(args[inIndex-1], args[inIndex-1], args[inIndex+1], NULL) < 0)
			exit(1);				//if execlp fails, return with exit status 1
				
	} else if ((outIndex > 0) && (inIndex > 0)) {

		if (outIndex > inIndex) {
			input = open(args[inIndex+1], O_RDONLY, 0644);	//file should already exist
			if (input < 0)
				printf("%s: does not exist\n", args[inIndex+1]); fflush(stdout);

			output = open(args[outIndex+1], O_WRONLY | O_CREAT | O_EXCL, 0644);	//file may not exist, create if not existing
			if (output < 0)
				output = open(args[outIndex+1], O_WRONLY | O_TRUNC, 0644);

			result = dup2(output, 1);			//set stdout to the 'output' file

			fcntl(input, F_SETFD, FD_CLOEXEC);
			fcntl(output, F_SETFD, FD_CLOEXEC);
			
			if (execlp(args[inIndex-1], args[inIndex-1], args[inIndex+1], args[outIndex+1], NULL) < 0)
				exit(1);			//if execlp fails, return with exit status 1
		}

	} else
		return;						//the case where there is no ">" or "<" in the command

}

//this function will return the index of ">"
int locateOutIndex(char** args, int length) {
	for (int x = 1; x < length; x++) {	//x starts at index 1 b/c first arg won't be ">"
		if (strcmp(args[x], ">") == 0)
			return x;
	}

	return -1;					//">" was never found
}

//this function will return the index of "<"
int locateInIndex(char** args, int length) {
	for (int x = 1; x < length; x++) {	//x starts at index 1 b/c first arg won't be "<"
		if (strcmp(args[x], "<") == 0)
			return x;
	}
	
	return -1;					//"<" was never found
}
