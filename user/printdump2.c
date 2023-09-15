#include "kernel/types.h"
#include "user/user.h"

int
main()
{
    uint64 register_value;
    int pid = getpid();

    for (int i = 2; i < 12; i++) {
        dump2(pid, i, &register_value);
        fprintf(1, "s%d: %d\n", i, register_value);
    }

    exit(0);
}