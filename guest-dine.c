#include "./guest-dine.h"
#include "./guest-wait.h"

extern struct GuestQueue guest_queue;

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
    free(p_guest);
}
