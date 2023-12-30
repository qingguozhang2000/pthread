#include "./guest-wait.h"

struct GuestQueue guest_queue;
int last_guest_id = 0;

pthread_t guest_th;
pthread_mutex_t mutex_guest_queue;

void on_start_guest_wait()
{
    srand(time(NULL));

    guest_queue.p_queue_head = NULL;
    guest_queue.p_queue_tail = NULL;

    assert(pthread_create(&guest_th, NULL, guest_thread_worker, NULL) == 0);
    assert(pthread_mutex_init(&mutex_guest_queue, NULL) == 0);
}

void on_finish_guest_wait()
{
    assert(pthread_mutex_destroy(&mutex_guest_queue) == 0);
    assert(pthread_join(guest_th, NULL) == 0);

    print_queue();
}

void on_guest_arriving(char *party_name, int party_size)
{
    printf("Guest arrived: %s - %d people\n", party_name, party_size);
    pthread_mutex_lock(&mutex_guest_queue);

    last_guest_id++;

    struct Guest *p_guest = malloc(sizeof(struct Guest));
    p_guest->id = last_guest_id;
    p_guest->name = party_name;
    p_guest->count = party_size;
    p_guest->p_table = NULL;
    p_guest->p_prev = NULL;
    p_guest->p_next = NULL;

    if (guest_queue.p_queue_tail == NULL)
    {
        guest_queue.p_queue_head = p_guest;
        guest_queue.p_queue_tail = p_guest;
    }
    else
    {
        p_guest->p_prev = guest_queue.p_queue_tail;
        guest_queue.p_queue_tail->p_next = p_guest;
        guest_queue.p_queue_tail = p_guest;
    }

    pthread_mutex_unlock(&mutex_guest_queue);
}

void *guest_thread_worker()
{
    for (int i = 0; i < 10; i++)
    {
        int sleep_time = rand() % 1000000;
        usleep(sleep_time);

        int j = rand() % name_count;
        char *name = names[j];
        int party_size = rand() % max_party_size + 1;

        on_guest_arriving(name, party_size);
    }

    return NULL;
}

void print_queue()
{
    printf("\n\nCurrent waiting queue:\n\n");

    struct Guest *p = guest_queue.p_queue_head;
    while (p != NULL)
    {
        printf("Guest party %d: %s - %d people\n", p->id, p->name, p->count);
        p = p->p_next;
    }
}
