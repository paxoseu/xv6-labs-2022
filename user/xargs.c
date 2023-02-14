#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char **
resize_and_cpy(char **src, int l, int r, int len)
{
    char **arg_prefix;
    arg_prefix = (char **)malloc(len * sizeof(char *));
    for (int i = l; i < r; i++)
    {
        char *buf;
        buf = (char *)malloc(strlen(src[i]) * sizeof(char));
        memcpy(buf, src[i], strlen(src[i]));
        arg_prefix[i-l] = buf;
    }
    return arg_prefix;
}

void
print_slice(char **str)
{
    char **c = str;
    while (1)
    {
        if(!strcmp(*c, '\0'))
        {
            break;
        }
        printf("%s ", *c++);
    }
    printf("\n");
}

void
append_char(char buf[], char ch)
{
    char *c = buf;
    for (;*c;c++);
    *c++ = ch;
    *c = 0;
}

int
read_row(int fd, char buf[])
{
    int split_len = 0;
    while (1)
    {
        char b[1];
        int n = read(fd, b, 1);
        if (n <= 0)
        {
            return n;
        }
        if (b[0] == '\n')
        {
            return ++split_len;
        }
        if (b[0] == ' ')
        {
            split_len ++;
        }
        append_char(buf, b[0]);
    }
}

void 
split_and_append(char **dst, char *str)
{
    char *c = str;

    char *buf = (char *)malloc(512 * sizeof(char));
    int idx = 0;

    char **dc = dst;
    
    for (;strcmp(*dc, '\0'); dc++);

    for (;*c;c++)
    {
        if (*c == ' ')
        {
            *dc++ = buf;
            buf = (char *)malloc(512 * sizeof(char));
            idx =  0;
            continue;
        }
        buf[idx++] = *c;
    }
    if (strlen(buf) > 0)
    {
        *dc++ = buf;
    }
}

int
main(int argc, char *argv[])
{   
    // read row from 0(stdout)
    while (1) 
    {
        char *buf;
        buf = (char*)malloc(sizeof(char) * 512);
        int n = read_row(0, buf);
        if (n <= 0)
        {
            break;
        }
        if (strlen(buf) ==0)
        {
            break;
        }
        char ** args = resize_and_cpy(argv, 1, argc, argc - 1 + n);

        split_and_append(args, buf);
        if (fork() > 0)
        {
            exec(argv[1], args);
            exit(1);
        } else{
            wait((int *)0);
        }
    }

    exit(0);
}