#pragma once
#include "sim.h"

void ev_freelist_init(int chunk);

void ev_pq_init(Event *ev_pq);

void ev_queue_reset(Event **tree_ptr);

void ev_enqueue(Event *event, Event **tree_ptr);

Event *ev_dequeue(Event **tree_ptr);

Event *get_ev();

void free_ev(Event *event);
