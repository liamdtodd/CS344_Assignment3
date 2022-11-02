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

	while (strncmp(readstr, "exit", 4) != 0) {
		memset(readstr, '\0', 2048);	//reset the readstr for each pass through loop
		printf(": ");
		fflush(stdout);
		bytes_read = getline(&readstr, &buffer_size, stdin);

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
		add_back(list, token);
		token = strtok_r(NULL, " ", &tknptr);
	}						//will loop through entire string until there are no more string vals

	if (strcmp(list->head->readstr, "cd") == 0)
		changeDIR(list->head->next->readstr);	//reading first value of linkedlist (has first command from user)
	else if (strcmp(list->head->readstr, "#") == 0) {
		//does nothing
	}
	else if (strcmp(list->head->readstr, "status") == 0) {
		printf("exit status: %d\n", status);
		fflush(stdout);
	}
	else
		status = callFork(line, list);

	return status;		
}

//this function will fork the processes and then call an exec function to run the user's command
int callFork(char* dataline, struct Linked_List* list) {
	int status = -5;

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
			return -1;
		}					//failed fork()

		case 0: {				//CHILD PROCESS
//			if (execvp(args[0], args) < 0)
			execute(args);
				return -1;		//this line will only run if execute() fails
		}

		default: {				//PARENT process
			pid_t parentPID = waitpid(PID, &status, 0);
		
			for (x = 0; x < list->length; x++)
				free(args[x]);
		
			return 0;
		}
	}

	return 0;					//fork did not fail, exec previously called
}

//this function will call execvp() and return the status of that call
int execute(char** argv) {
	if (execvp(*argv, argv) < 0)
		return -1;
	return 0;					//this line should not run
}
