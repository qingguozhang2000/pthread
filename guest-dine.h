#include "./common.h"

#ifndef GUEST_DINE
#define GUEST_DINE

void on_guest_leaving(struct Guest *p_guest);
void on_table_available(struct Table *p_table);

#endif
