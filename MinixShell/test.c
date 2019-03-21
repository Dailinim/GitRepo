#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char const *argv[])
{
    int fd[2];
    pid_t pid;
    pipe(fd);

    if((pid = fork())<0)
        perror("fork error!");
    if(pid == 0)
    {
        char* a[4];
        char* d="ls", *e="-a", *f="-l";
        a[0] = d;
        a[1] = e;
        a[2] = f;
        a[3] = NULL;
        close(1);
        dup2(fd[1], 1);
        execvp(a[0], a);
        exit(0);
    }
    else{
        char* b[3];
        b[0] = "grep";
        b[1] = "x";
        b[2] = NULL;
        waitpid(pid, NULL, 0);
        close(0);
        dup2(fd[0], 0);
        execvp(b[0], b);
        exit(0);
    }
    return 0;
}


