#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
fp(int i)
{
    printf("prime %d\n", i);
}

__attribute__((noreturn))
void
guard(int g, int p[2])
{   
    close(p[1]); // do not need write;
    int next_p[2];
    int has_next = 0;
    while (1)
    {
        char rec_buf[1];
        if(read(p[0], &rec_buf, 1) <= 0)
        {
            close(p[0]);
            if (has_next)
            {
                close(next_p[1]);
            }
            wait((int *)0);
            exit(0);
        }

        int i = rec_buf[0];
        if (i == g)
        {
            fp(i);
            continue;
        }

        if (i % g == 0) 
        {
            continue;
        }

        if (has_next)
        {
            char buf[1];
            buf[0] = i;
            write(next_p[1], buf, 1);
        } else {
            fp(i);
            has_next = 1;
            pipe(next_p);
            if (fork() > 0) {
                guard(i, next_p);
            }
            close(next_p[0]);
        }
    }
}

int
main(int argc, char *argv[])
{
    int next_p[2];
    int has_next = 0;

    for (int i = 2; i < 36; i++)
    {   
        if (i == 2)
        {
            fp(i);
            continue;
        }
        if (has_next)
        {
            char buf[1];
            buf[0] = i;
            write(next_p[1], buf, 1);
        } else {
            fp(i);
            has_next = 1;
            pipe(next_p);
            if (fork() > 0) {
                guard(i, next_p);
            }
            close(next_p[0]);
        }
    }
    close(next_p[1]);
    wait((int *)0);
    exit(0);
}