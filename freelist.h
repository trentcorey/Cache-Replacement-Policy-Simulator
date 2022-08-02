#pragma once
#include <stdint.h>
#include <cstddef>

/* A FreeList is a linked list data structure that helps allocate dynamic memory. 
In this case, the FreeList will allocate events and packets.*/

/* Define the nodes within FreeList */
struct node
{
    node *next;
    node *prev;
};

/* Define the FreeList itself which will act as a buffer for deallocating nodes */
struct FreeList
{
    node *head;
    uint32_t num_alloc; /* Current # of nodes allocated */
    uint32_t curr_num; /* Current # of nodes in list */
    uint32_t list_limit; /* If a node is trying to be freed and the limit isn't reached, put it at the back of the queue and make it the new head. If the limit is released, then just free the node (deallocate it). */
};

void InitFreeList(FreeList *list, int limit=4);

node *GetNode(FreeList *list, size_t nodesize);

void FreeNode(FreeList *list, node *node);

int ReverseList(node **head, node **tail);

int ReverseDoubleList(node **head, node **tail);