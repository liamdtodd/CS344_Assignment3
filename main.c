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

void changeDIR(struct Linked_List*);
void parseInput(struct Linked_List*, char*);

int main() {
	char* readstr = malloc(2048 * sizeof(char));
	memset(readstr, '\0', 2048);
//	struct Linked_List* list = malloc(sizeof(struct Linked_List));

	size_t buffer_size = 2048;
	size_t bytes_read = -5;			//variables needed for getline command

	while (strncmp(readstr, "exit", 4) != 0) {
		memset(readstr, '\0', 2048);	//reset the readstr for each pass through loop
		printf(": ");
		fflush(stdout);
		bytes_read = getline(&readstr, &buffer_size, stdin);

		struct Linked_List* list = malloc(sizeof(struct Linked_List));
		parseInput(list, readstr);
		free_listelements(list);
		free(list);
		list = NULL;
	}

//	free_listelements(list);
//	free(list);
//	list = NULL;

	free(readstr);
	readstr = NULL;				//free dynamic mem
	return 0;
}

//this function will change to the desired directory
void changeDIR(struct Linked_List* list) {
	struct node* temp = list->head->next;

	DIR* directory = opendir(".");
	chdir(temp->readstr);
	
	closedir(directory);
}

//this function will parse through the user's input and store each string of chars b/w spaces into a node of a linkedlist
//then the function will call the corresponding execution (cd, status, or excelvp)
void parseInput(struct Linked_List* list, char* readstr) {
	char* token = NULL;
	char* tknptr = NULL;

//	token = strtok_r(readstr, " ", &tknptr);
	while ((token = strtok_r(readstr, " ", &tknptr)) != NULL) {
		add_back(list, token);
		token = strtok_r(readstr, " ", &tknptr);
	}

	if (strcmp(list->head->readstr, "cd") == 0)
		changeDIR(list);
}	
