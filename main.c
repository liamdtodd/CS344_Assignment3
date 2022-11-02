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

void changeDIR(char*);
void parseInput(struct Linked_List*, char*);

int main() {
	char* readstr = malloc(2048 * sizeof(char));
	memset(readstr, '\0', 2048);		//initialize values for readstr

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

		parseInput(list, readstr);

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
void parseInput(struct Linked_List* list, char* dataline) {
	char* token = NULL;
	char* tknptr = NULL;

	token = strtok_r(dataline, "\n", &tknptr);	//remove 'enter' so strtok_r can read last string entered by user
	token = strtok_r(dataline, " ", &tknptr);	//now tokenize string by spaces
	while (token != NULL) {
		add_back(list, token);
		token = strtok_r(NULL, " ", &tknptr);
	}						//will loop through entire string until there are no more string vals

	if (strcmp(list->head->readstr, "cd") == 0)
		changeDIR(list->head->next->readstr);	//reading first value of linkedlist (has first command from user)
}	
