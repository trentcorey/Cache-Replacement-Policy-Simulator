#include <stdio.h>
#include <stdlib.h>

#include "freelist.h"

/* Init the free list, default limit size is 4 */
void InitFreeList(FreeList *list, int limit)
{
    list->head = NULL;
    list->num_alloc = 0;
    list->curr_num = 0;
    list->list_limit=limit;
    return;
}

/* Get a free node. Try to first grab one from the list, otherwise just allocate one */
node *GetNode(FreeList *list, size_t itemsize)
{
    node *node = NULL;

    if(list->head != NULL)
    {
        /* If the list isn't empty, take the head of the queue */
        node = list->head;
        list->head = node->next;
        list->curr_num--;

        return node;
    } 
    else {
        /* If the list is empty, try to allocate another node. If successful increment num allocated */
        //node = (node *)malloc(itemsize);
        if(node != NULL){
           list->num_alloc++;
        }
        return node;
    }
}

/* Free a node (meaning put a node into our free list as a buffer). If the list is full, just deallocate instead */
void FreeNode(FreeList *list, node *node)
{
    if(list->curr_num >= list->list_limit)
    {
        /* If our list is alread at max capacity, just deallocate the node */
        free(node);
        list->num_alloc--;
    }
    else {
        /* If our list has space, add the node to the front of the queue */
        node->next = list->head;
        list->head = node;
        list->curr_num++;
    }
}

int ReverseList(node **head, node **tail)
{
    // Is it okay to just copy his algorithm?
}

int ReverseDoubleList(node **head, node **tail)
{
    // Is it okay to just copy his algorithm?
}
