#include "kernel/types.h"
#include "user/user.h"

int
main()
{
    int pid = getpid();
    setreg(pid, S2, 2);
    setreg(pid, S3, 3);
    setreg(pid, S4, 4);
    setreg(pid, S5, 5);
    setreg(pid, S6, 6);
    setreg(pid, S7, 7);
    setreg(pid, S8, 8);
    setreg(pid, S9, 9);
    setreg(pid, S10, 10);
    setreg(pid, S11, 11);
    dump();
    exit(0);
}