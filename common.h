#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#ifndef COMMON
#define COMMON

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

void load_config();
void free_config();

int max_party_size;
int table_count;
struct Table *tables;

int name_count;
char **names;

#endif
