#include "kernel/types.h"
#include "user/user.h"


int
main(int argc, char *argv[])
{
    int t = atoi(argv[0]);
    if (sleep(t) < 0) exit(-1);
    exit(0);
}