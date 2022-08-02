#include <stdio.h>
#include <stdlib.h>
#include "splay.h"

/*
 *      Event structure definition
 */
typedef struct Event_Struct {
  struct Event_Struct *next;          /* link events into list */
  struct Event_Struct *left;          /* Splay tree internals */
  struct Event_Struct *right;         /* Splay tree internals */
  struct Event_Struct *parent;        /* Splay tree internals */
  double              key;            /* timer value          */
  int    id;
} Event;

struct Event_Struct *EventTree;

int main(int argc, char *argv[])
{
  Event *ev_array;
  Event *e;
  int i, num;
  double r;

  if( argc != 2 ) {
    printf( "Usage : %s num\n", argv[0] );
    exit(0);
  }     

  sscanf( argv[1], "%d", &num); 

  if ((ev_array = (Event *) malloc (sizeof(Event) * num)) == NULL) {
    printf("main(): out of memory\n");
    exit(-1);
  }

  SInit( (SplayTree *) &EventTree);

  for (i = 0; i < num; i++) {
    r = (double) random();
    //printf("random is: %g\n", r);
    ev_array[i].key = r;
    SInsert( (SplayNode *) (& ev_array[i]), (SplayTree *) &EventTree);
  }
  
  /* process all events scheduled for current time */
  do {
    SDelete((SplayNode **) &e, (SplayTree *) &EventTree);
    if (e == NULL) {
      break;
    }
    //printf("Event Key: %g\n", e->key);
  } while (1);
  
  return (0);
}
