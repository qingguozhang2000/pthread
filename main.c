#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define NUM_THREADS 5
#define MAX_TABLE 100

struct Table
{
    int id;                // id of a dining table in the resturant
    int capacity;          // maximum guest the table can acommodate
    struct Guest *p_guest; // the guest currently assigned to the table
};

struct Guest
{
    int id;     // id of guest party
    char *name; // name of the guest reserving a dining table
    int count;  // number of guest in the dining party
    struct Table *p_table;
    struct Guest *p_prev;
    struct Guest *p_next;
};

struct GuestQueue
{
    struct Guest *p_queue_head;
    struct Guest *p_queue_tail;
};

int table_count;
struct Table tables[MAX_TABLE];
struct GuestQueue guest_queue;
int last_guest_id = 0;

void on_start();
void on_finish();
void on_guest_arriving(const char *part_name, int party_count);
void on_guest_leaving(struct Guest *p_guest);
void on_table_available(struct Table *p_table);
void *worker(void *arguments);

int main(void)
{
    on_start();

    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int i, rc;

    // create all threads one by one
    for (i = 0; i < NUM_THREADS; i++)
    {
        printf("In main: Creating thread %d.\n", i);
        thread_args[i] = i;
        rc = pthread_create(&threads[i], NULL, worker, &thread_args[i]);
        assert(!rc);
    }

    printf("In main: All threads are created.\n");

    // wait for each thread to complete
    for (i = 0; i < NUM_THREADS; i++)
    {
        rc = pthread_join(threads[i], NULL);
        assert(!rc);
        printf("In main: Thread %d has ended.\n", i);
    }

    on_finish();
    printf("Main program has ended.\n");

    return 0;
}

void *worker(void *arguments)
{
    int index = *((int *)arguments);
    int sleep_time = 1 + rand() % NUM_THREADS;

    printf("Thread %d: Started.\n", index);
    printf("Thread %d: Will be sleeping for %d seconds.\n", index, sleep_time);

    sleep(sleep_time);

    printf("Thread %d: Ended.\n", index);

    return NULL;
}

void on_start()
{
    guest_queue.p_queue_head = NULL;
    guest_queue.p_queue_tail = NULL;

    int i = 0;
    char line[10];
    char *s;
    FILE *fd = fopen("table.txt", "r");
    while (s = fgets(line, 10, fd)) {
        tables[i].id = i;
        tables[i].capacity = atoi(s);
        tables[i].p_guest = NULL;
        i++;
    }

    table_count = i;
}

void on_finish()
{
    // TODO: free dynamically allocated memory
}

void on_guest_arriving(const char *party_name, int party_count)
{
    last_guest_id++;

    struct Guest *p_guest = malloc(sizeof(struct Guest));
    p_guest->id = last_guest_id;
    p_guest->name = party_name;
    p_guest->count = party_count;
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

void on_table_available(struct Table *p_table)
{
    // look through waiting queue and find first guest party that can fit the table
    struct Guest *p_guest = guest_queue.p_queue_head;
    while (p_guest != NULL)
    {
        if (p_guest->count <= p_table->capacity)
        {
            break;
        }

        p_guest = p_guest->p_next;
    }

    // if we find a matching guest party for the table,
    // remove the guest from the waiting queue
    // and assign the guest to the table 
    if (p_guest != NULL)
    {
        // remove guest from waiting queue
        if (p_guest->p_prev != NULL)
        {
            p_guest->p_prev->p_next = p_guest->p_next;
        }

        if (p_guest->p_next != NULL)
        {
            p_guest->p_next->p_prev = p_guest->p_prev;
        }

        // assign guest to the table
        p_guest->p_table = p_table;
        p_table->p_guest = p_guest;
    }
}

void on_guest_leaving(struct Guest *p_guest)
{
    // the guest's table is now available
    on_table_available(p_guest->p_table);

    // free dynamic allocated memory for the guest
    free(p_guest->name);
    free(p_guest);
}
