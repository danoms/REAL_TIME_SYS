#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"

list_t list_create()
{
	list_t list = malloc(sizeof(*list));
	if (list == NULL)
	{
		perror("Error: ");
		return NULL;
	}
	
	list->head = malloc(sizeof( *(list->head) ));
	if (list->head == NULL)
	{
		free(list);
		perror("Error: ");
		return NULL;
	}
	
	list->tail = malloc(sizeof( *(list->tail) ));
	if (list->tail == NULL)
	{
		free(list->head);
		free(list);
		perror("Error: ");
		return NULL;
	}
	
	// list is empty, taking care of pointers
	list->head->next = NULL;
	list->head->prev = NULL;
	list->head->data = 0;
	
	list->tail->next = NULL;
	list->tail->prev = NULL;
	list->tail->data = 0;
	
	list->length = 0;
	
	return list;
}

void list_delete(list_t list)
{	
	free(list->head);
	free(list->tail);
	free(list);
}

void list_insert(list_t list, int index, int data)
{
	int index_now;
	
	struct node *node = malloc(sizeof(*node));
	if (node == NULL)
		perror("Error: ");
		
	struct node *tmpNode = list->head->next;
	
	// if index is out of bonds
	if (index > list->length)
	{
		printf("index out of bonds, reduce index to insert element!!\n");
	}
	// index points to last element
	else if (index == list->length-1)
	{
		list_append(list, data);
	}
	else
	{
		for (index_now=1; index_now<index; index_now++)
		{
			tmpNode = tmpNode->next;
		}
		//increase lists length
		list->length++;
	
		// insert new element
		node->prev = tmpNode;
		node->next = tmpNode->next;
		node->data = data;
		
		// update element before new element
		tmpNode->next = node;
		
		// update element after new element
		node->next->prev = node;
	}
}

void list_append(list_t list, int data)
{
	struct node *node = malloc(sizeof(*node));
	if (node == NULL)
		perror("Error: ");
		
	// update new element
	node->next = NULL;
	node->prev = list->tail->next;
	node->data = data;
	
	//increase lists length
	list->length++;
		
	// if adding first element to list
	if (list->length == 1)
	{
		node->prev = list->head;
		
		list->head->next = node;
		list->tail->next = node;
	}
	else 
	{		
		// point last element to new one
		list->tail->next->next = node;
		
		// move tail up in elements
		list->tail->prev = list->tail->next;
		list->tail->next = node;
	}
}

void list_print(list_t list)
{
	// point to first element
	struct node *node = list->head->next;
	
	// while it points to some element
	while(node != NULL)
	{
		printf("%d ", node->data);
		node = node->next;
	}
	printf("\n");
}

long list_sum(list_t list)
{
	long sum = 0;
	
	// point to first element
	struct node *node = list->head->next;
	
	// while it points to some element
	while(node != NULL)
	{
		sum += node->data;
		node = node->next;
	}
	return sum;
}

int list_get(list_t list, int index)
{
	int index_now;
	struct node *tmpNode = list->head->next;
	
	// count up to index
	for (index_now=0; index_now<index; index_now++)
	{
		tmpNode = tmpNode->next;
	}
	return tmpNode->data;
}

int list_extract(list_t list, int index)
{
	int index_now, tmpData;
	struct node *tmpNode = list->head->next;
	
	// count up to index
	for (index_now=0; index_now<index; index_now++)
	{
		tmpNode = tmpNode->next;
	}
	tmpData = tmpNode->data;
	
	// if removes last element
	if (list->length == 1)
	{
		list->head->next = NULL;
		
		list->tail->prev = NULL;
		list->tail->next = NULL;
	}
	else
	{
		// before index element points to after index element and vica
		tmpNode->prev->next = tmpNode->next;
		tmpNode->next->prev = tmpNode->prev;
	}
	
	free(tmpNode);
	
	//decrease lists length
	list->length--;
	
	return tmpData;
}
