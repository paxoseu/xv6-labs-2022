#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int
main(int argc, char *argv[])
{
    int p[2];
    pipe(p);
    if (fork() == 0) {
        // child
        char buf[4];
        read(p[0], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
        write(p[1], "pong", 4);
        exit(0);
    }
    write(p[1], "ping", 4);
    wait((int *)0);
    printf("%d: received pong\n", getpid());
    exit(0);
}