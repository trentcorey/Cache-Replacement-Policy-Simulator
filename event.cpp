#include "event.h"
//#include "sim.h"
#include "provided_code/splay.h"
#include "freelist.h"
//#include <cstddef>

/* Define the FreeList for events globally */
FreeList EventList;

/* Initiate the event free list with chunk limit */
void ev_freelist_init(int chunk)
{
    InitFreeList(&EventList, chunk);
    return;
}

/* Init the priority queue by resetting it */
// Why does he separate the functions? Why not just name the reset function init?
// Is it called elsewhere?
void ev_pq_init(Event *ev_pq)
{
    ev_queue_reset(&ev_pq);
    return;
}

/* The priority queue of events is our splay tree which will just set our tree to NULL */
void ev_queue_reset(Event **tree_ptr){
    SInit((SplayTree *) tree_ptr);
    return;
}

/* Enqueue and existing event into the splay tree */
void ev_enqueue(Event *event, Event **tree_ptr)
{
    SInsert((SplayNode *) event, (SplayTree *) tree_ptr);
    return;
}

/* Dequeue the root of the splay tree */
Event *ev_dequeue(Event **tree_ptr)
{
    Event *event;
    SDelete((SplayNode **) &event, (SplayTree *) tree_ptr);
    return event;
}

/* Get an event currently in the free list */
Event *get_ev()
{
    Event *event;
    event = (Event *) GetNode(&EventList, sizeof(Event));
    return event;
}

/* Return an event to the free list */
void free_ev(Event *event)
{
    event->packet = NULL;
    event->next = event->left = event->right = event->parent = NULL;
    FreeNode(&EventList, (node *) event);
    return;
}