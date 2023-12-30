#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define NUM_THREADS 5

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

int max_party_size;
int table_count;
struct Table *tables;

int name_count;
char **names;

struct GuestQueue guest_queue;
int last_guest_id = 0;

pthread_mutex_t mutext;

void on_start();
void on_finish();
void on_guest_arriving(char *part_name, int party_size);
void on_guest_leaving(struct Guest *p_guest);
void on_table_available(struct Table *p_table);

void load_tables();
void load_names();
int line_count(FILE *fd);

void print_queue();

void *guest_thread_worker();

int main(void)
{
    on_start();

    pthread_t guest_th;

    assert(!pthread_create(&guest_th, NULL, guest_thread_worker, NULL));
    assert(!pthread_mutex_init(&mutext, NULL));
    assert(!pthread_mutex_destroy(&mutext));
    assert(!pthread_join(guest_th, NULL));

    print_queue();

    on_finish();
    printf("Main program has ended.\n");

    return 0;
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

void on_start()
{
    srand(time(NULL));

    guest_queue.p_queue_head = NULL;
    guest_queue.p_queue_tail = NULL;

    load_tables();
    load_names();
}

int line_count(FILE *fd)
{
    char ch;
    int lines = 0;

    while (!feof(fd))
    {
        ch = fgetc(fd);
        if (ch == '\n')
        {
            lines++;
        }
    }

    rewind(fd);
    return lines;
}

void load_tables()
{
    FILE *fd = fopen("table.txt", "r");
    if (fd == NULL)
    {
        perror("Unable to open table.txt");
    }

    max_party_size = 0;
    table_count = line_count(fd);
    tables = malloc(table_count * sizeof(struct Table));

    char line[10];
    for (int i = 0; i < table_count; i++)
    {
        char *s = fgets(line, 10, fd);
        if (!s)
        {
            break;
        }

        int capacity = atoi(s);

        tables[i].id = i;
        tables[i].capacity = capacity;
        tables[i].p_guest = NULL;

        if (max_party_size < capacity)
        {
            max_party_size = capacity;
        }
    }

    fclose(fd);
}

void load_names()
{
    FILE *fd = fopen("name.txt", "r");
    if (fd == NULL)
    {
        perror("Unable to open name.txt");
    }

    name_count = line_count(fd);
    names = malloc(name_count * sizeof(char *));

    char line[15];
    for (int i = 0; i < name_count; i++)
    {
        char *name = fgets(line, 15, fd);
        int n = strlen(name);
        names[i] = malloc(n);
        for (int j = 0; j < n; j++)
        {
            if (name[j] != '\n')
            {
                names[i][j] = name[j];
            }
        }
    }

    fclose(fd);
}

void on_finish()
{
    // free dynamically allocated memory
    free(tables);
    free(names);
}

void on_guest_arriving(char *party_name, int party_size)
{
    printf("Guest arrived: %s - %d people\n", party_name, party_size);
    pthread_mutex_lock(&mutext);

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

    pthread_mutex_unlock(&mutext);
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

void print_queue()
{
    printf("\n\nCurrent waiting queue:\n\n");

    struct Guest *p = guest_queue.p_queue_head;
    while (p != NULL) {
        printf("Guest party %d: %s - %d people\n", p->id, p->name, p->count);
        p = p->p_next;
    }
}
