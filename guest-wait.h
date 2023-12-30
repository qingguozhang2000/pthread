#include "./common.h"

#ifndef GUEST_WAIT
#define GUEST_WAIT

#define WAIT_QUEUE_LIMIT 20

struct GuestQueue
{
    struct Guest *p_queue_head;
    struct Guest *p_queue_tail;
};

void on_start_guest_wait();
void on_finish_guest_wait();
void on_guest_arriving(char *part_name, int party_size);
void *guest_thread_worker();
void print_queue();

#endif
