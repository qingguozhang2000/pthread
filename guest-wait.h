#include "./common.h"

#ifndef GUEST_WAIT
#define GUEST_WAIT

#define WAIT_QUEUE_LIMIT 5

struct GuestQueue
{
    struct Guest *p_queue_head;
    struct Guest *p_queue_tail;
};

void on_start_guest_wait();
void on_finish_guest_wait();

void *guest_arrival_worker();
void *guest_seating_worker();

void print_queue();

#endif
