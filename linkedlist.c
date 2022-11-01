/*******************
 * Liam Todd
 * the .c implementation of linkedlist and it's associated functions
 * ****************/
#include "linkedlist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//this function dynamically creates the linked list
struct Linked_List* create_linkedlist() {
	struct Linked_List* list = malloc(sizeof(struct Linked_List));
	list->length = 0;
	list->head = NULL;
	list->tail = NULL;
	
	return list;
}

//this function checks to see if the list is empty
int check_empty(struct node* head_temp) {
	if (head_temp == NULL)
		return 1;	//1 is true
	return 0;		//0 is false
}
	
//this function is specifically for inserting the first element to the linked list
void insert_firstelement(struct Linked_List* list, char* strval) {
	struct node* n1 = malloc(sizeof(struct node));
	n1->readstr = malloc((strlen(strval) + 1) * sizeof(char));
	strcpy(n1->readstr, strval);
//	n1->readstr = strval;					//creating node, storing movie's data to it
	n1->next = NULL;					//special case, 1st node needs own function to declare it's next as NULL
	
	list->head = n1;
	list->tail = n1;
}

//this function adds to the back of the linked list, at the tail
void add_back(struct Linked_List* list, char* strval) {
	if (check_empty(list->head) == 1)
		insert_firstelement(list, strval);		//checking to see if this will be the first element inserted
	else {
		struct node* n1 = malloc(sizeof(struct node));	
		n1->readstr = malloc((strlen(strval) + 1) * sizeof(char));
		strcpy(n1->readstr, strval);
//		n1->readstr = strval;				//creating node, storing movie's data to it
		n1->next = NULL;
		list->tail->next = n1;				//organizing linkedlist
		list->tail = n1;
	}
	
	list->length++;
}

//this function frees all the dynamically allocated data associated with the linked list
void free_listelements(struct Linked_List* list) {
	int x = 0;
	struct node* temp = NULL;
	for (x; x < list->length; x++) {
		temp = list->head;
		list->head = list->head->next;
		free(temp->readstr);
		free(temp);
		temp = NULL;
	}				//loop traverses through list, frees each dynamic allocated node
	list->length = 0;		//reset length, if list was to be used again
}
