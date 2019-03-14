#include <stdio.h>
#include <stdlib.h>


void create(char* s)
{
    char* tmp=(char*)malloc(sizeof(char)*10);
    fgets(tmp, 9, stdin);
    s = tmp;
    printf("s = %s\n", s);
    
    return;
}
int main(int argc, char const *argv[])
{//pipe_cmd
    int pipe_num=0;
    char pipe_argv[CMDSIZE];

    for(int i=0; i<cmdline->argc; i++)
    {
        if(cmdline->argv[i] == '|')
        {
            pipe_num++;
            cmdline->argv[i] == 0;
        }       
    } 
    for(int cp_num=1;cp_num <= pipe_num; cp_num++)
    {
        pid_t pid;
        int fd[2];
        if(pipe(&fd[0])<0)
            perror("Build pipe error!");

        if((pid = fork())<0)
            perror("Fork Error!");

        if(pid!=0)//父进程 写
        {
            close(fd[0]);
            int tmpin = dup(STD_OUTPUT);
            dup(STD_OUTPUT);
            if(execvp())
            
        }
    }


    //只有两个进程的管道
    pipe(fd);
    
    if((pid = fork())<0)
        perror("fork error!");
    if(pid!=0)
    {
        int tmp_stdout;

        close(fd[0]);
        tmp_stdout = dup(STD_OUTPUT);
        close(STD_OUTPUT);

        for(i=0; cmdline->argv[i]!="|"; i++)
            pipe_argv[i] = cmdline->argv[i];
        pipe_argv[i]=0;
        
        if(execvp(pipe_argv[0], pipe_argv)<0)
            perror("execvp error!");

        close(fd[1]);   
        dup(tmp_stdout);
        waitpid(pid, NULL, 0);
    }
    else
    {
        int tmp_stdin;
        //char* pipe_argv[CMDSIZE];

        close(fd[1]);
        tmp_stdin = dup(STD_INPUT);
        close(STD_OUTPUT);

        int i;
        for(i=0; cmdline->argv[i]!="|"; i++);
        int j = i+1;
        for(int i=0; cmdline->argv[j]!=0; j++, i++)
            pipe_argv[i] = cmdline->argv[j];
        pipe_argv[i] = 0;

        if(execvp(pipe_argv[0], pipe_argv)<0)
            perror("execvp error!");
        exit(0);

    }
    return 0;
}
