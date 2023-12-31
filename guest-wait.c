#include "./guest-wait.h"

struct GuestQueue guest_queue;
int last_guest_id = 0;

pthread_t guest_arrival_thread;
pthread_t guest_seating_thread;

pthread_mutex_t guest_queue_mutex;

sem_t *q_empty_sem;
sem_t *q_full_sem;

void on_start_guest_wait()
{
    srand(time(NULL));

    guest_queue.p_queue_head = NULL;
    guest_queue.p_queue_tail = NULL;

    assert(pthread_create(&guest_arrival_thread, NULL, guest_arrival_worker, NULL) == 0);
    assert(pthread_create(&guest_seating_thread, NULL, guest_seating_worker, NULL) == 0);

    assert(pthread_mutex_init(&guest_queue_mutex, NULL) == 0);

    sem_unlink("/empty_sem");
    q_empty_sem = sem_open("/empty_sem", O_CREAT | O_EXCL, 0644, WAIT_QUEUE_LIMIT);
    assert(q_empty_sem != SEM_FAILED);


    sem_unlink("/full_sem");
    q_full_sem = sem_open("/full_sem", O_CREAT | O_EXCL, 0644, 0);
    assert(q_full_sem != SEM_FAILED);
}

void on_finish_guest_wait()
{
    print_queue();

    assert(pthread_join(guest_arrival_thread, NULL) == 0);
    assert(pthread_join(guest_seating_thread, NULL) == 0);

    assert(pthread_mutex_destroy(&guest_queue_mutex) == 0);

    assert(sem_close(q_empty_sem) == 0);
    assert(sem_close(q_full_sem) == 0);

    sem_unlink("/empty_sem");
    sem_unlink("/full_sem");
}

void add_guest_to_queue(char *party_name, int party_size)
{
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
}

void *guest_arrival_worker()
{
    while (1)
    {
        int sleep_time = rand() % 1000000;
        usleep(sleep_time);

        int j = rand() % name_count;
        char *name = names[j];
        int party_size = rand() % max_party_size + 1;

        printf("Guest arrived: %s - %d people\n", name, party_size);
        printf("Waiting for adding to queue\n");

        sem_wait(q_empty_sem);
        pthread_mutex_lock(&guest_queue_mutex);

        add_guest_to_queue(name, party_size);

        printf("Added the guest party %d (%s) to queue\n", last_guest_id, name);

        pthread_mutex_unlock(&guest_queue_mutex);
        sem_post(q_full_sem);
    }

    return NULL;
}

void *guest_seating_worker()
{
    while (1) {
        printf("Waiting for queue populated\n");

        sem_wait(q_full_sem);
        pthread_mutex_lock(&guest_queue_mutex);

        print_queue();

        struct Guest *p_head = guest_queue.p_queue_head;

        printf("Removing %d - %s from waiting queue\n", p_head->id, p_head->name);

        guest_queue.p_queue_head = p_head->p_next;
        if (p_head->p_next != NULL) {
            p_head->p_next->p_prev = NULL;
        } else {
            guest_queue.p_queue_tail = NULL;
        }
        free(p_head);

        print_queue();

        pthread_mutex_unlock(&guest_queue_mutex);
        sem_post(q_empty_sem);
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
