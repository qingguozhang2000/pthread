#include "./guest-wait.h"
#include "./guest-dine.h"

int main(void)
{
    load_config();

    on_start_guest_wait();
    on_finish_guest_wait();

    free_config();
    return 0;
}
