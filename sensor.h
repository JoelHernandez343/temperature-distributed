#include "defs.h"

void copy_clock(Clock *origin, Clock *dest);
void read_clock(int iic_fd, Clock *clock);
void print_clock(Clock *clock);
