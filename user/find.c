#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  return p;
}

void find(char *path, char *dst)
{
    int fd;
    struct dirent de;
    struct stat st;
    
    if((fd = open(path, 0)) < 0){
      fprintf(2, "find: cannot open %s\n", path);
      return;
    }
    
    if(fstat(fd, &st) < 0){
      fprintf(2, "find: cannot stat %s\n", path);
      close(fd);
      return;
    }

    switch(st.type){
    case T_DEVICE:
    case T_FILE:
      if (!strcmp(fmtname(path), dst))
      {
        printf("%s\n", path);
      }
      break;
    case T_DIR:
      if(strlen(path) + 1 + DIRSIZ + 1 > 512){
        printf("ls: path too long\n");
        break;
      }
      while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0)
          continue;

        char *p;
        char buf[512];
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';

        if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
        {
            continue;
        }

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        find(buf, dst);
      }
      break;
    }
    close(fd);
}

int
main(int argc, char *argv[])
{
    char *path = argv[1];
    char *dst = argv[2];
    find(path, dst);
    exit(1);
}