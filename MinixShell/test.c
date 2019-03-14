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


//专门处理管道
void pipe_cmd(char* cmd, struct command_type* cmdline)
{
    int fd[2];
    int i=0;
    int pipe_num = 0;//所需管道数量
    int fds[PIPENUM][2];
    pid_t pid;
    int pipe_cmd_num;//命令数量
    char* pipe_cmd[CMDSIZE];//保存每一条命令
    char* p;
    
    //计算需要多少管道
    for(i=0; i<cmdline->argc; i++)
    {
        if(cmdline->argv[i] == '|')
        {
            pipe_num++;
            cmdline->argv[i] == 0;
        }       
    } 

    //创建管道
    for(i=0; i<pipe_num; i++)
        pipe(&fds[i][0]);

    //按照|将原始命令分割开， pipe_argv[]的一项为一个命令
    p= strtok(cmd, "|");
    while(p)
    {
        pipe_cmd[i++] = p;
        p = strtok(NULL, "|");
    }
    pipe_cmd[i]=0;
    pipe_cmd_num = i;

    //
    int pipe_count = 1;
    int pro_count = 1;

    if((pid = fork())<0)
        perror("fork error!");

    if(pid == 0)//子进程
    {
        close(STD_OUTPUT);
        dup2(fd[pipe_count-1][1], STD_OUTPUT);
        
        char* pipe_argv[CMDSIZE];
        int 
        p = strtok(pipe_cmd[pro_count-1], " ");
        while(p)
        {

        }
    }
    else//父进程
    {
        pipe_loop()
    }
        

void pipe_loop(int pipe_count, int pro_count, char* pipe_cmd[])
{
    while(pro_count>1)
    {
        pipe_count--;
        pro_count--;
        if((pid = fork())<0)
            perror("pipe error!");
        
        if(pid != 0)
        {   
            waitpid(pid, NULL, 0);
            close(STD_OUTPUT);
            dup2(fds[pipe_count+1][1], STD_OUTPUT);
            close(STD_INPUT);
            dup2(fds[pipe_count][0], STD_INPUT);

            //按照空格分割命令
            char pipe_argv[CMDNUM];
            int j=0;
            p = strtok(pipe_cmd[pro_count], " ");
            while(p)
            {
                pipe_argv[j++]=p;
                p = strtok(NULL, " ");
            }
            pipe_argv[j]=0;

            if(execvp(argv[0], argv)<0)
                perror("execvp error!");
        }
        else
        {
            if(pro_count==1)
                break;
             
        }
        
    }
    
}