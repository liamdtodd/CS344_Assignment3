/********************
 * Liam Todd
 * the struct and its associated functions for 'linkedlist'
 * *****************/
#ifndef LINKEDLIST_H
#define LINKEDlIST_H
#include "node.h"
#include <stdlib.h>

struct Linked_List {
	int length;
	struct node* head;
	struct node* tail;
};

struct Linked_List* create_linkedlist();

int check_empty(struct node*);

void insert_firstelement(struct Linked_List*, char*);
void add_back(struct Linked_List*, char*);

void free_listelements(struct Linked_List*);
#endif
