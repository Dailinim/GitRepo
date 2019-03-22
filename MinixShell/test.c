#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char const *argv[])
{
    int fds[3][2];
    pid_t pid;


    char** pcmd;
    char* cmd[10];
    cmd[0] = "ls";
    cmd[1] = "-a";
    cmd[2] = "-l";
    cmd[3] = NULL;
    cmd[4] = "grep";
    cmd[5] = "x";
    cmd[6] =NULL;
    cmd[7] = "grep";
    cmd[8] = "dr";
    cmd[9] = NULL;

    int p_num[3] = {0, 4, 7};

    for (int i=1; i<=2; i++)
    {
        pipe(fds[i]);
    }

    int cmd_count=0;
    int cmd_num=3;
    while(cmd_count<=cmd_num)
    {
        if((pid = fork())<0)
            perror("fork error!");
        if(pid != 0)
        {
            int tmpfds[3][2];
            for(int i=1; i<=cmd_num-1; i++)
            {
                dup2(fds[i][0], tmpfds[i][0]);
                dup2(fds[i][1], tmpfds[i][1]);
                close(fds[i][0]);
            }
            printf("parents start to wait!\n");
            waitpid(pid, NULL, 0);
            for(int i=1; i<=cmd_num-1; i++)
            {
                dup2(tmpfds[i][0], fds[i][0]);
                dup2(tmpfds[i][1], fds[i][1]);
            }
            
            printf("parents stop to wait!\n");
            printf("\ninto Parents!\n");
            cmd_count++;
        }
        else
        {
            printf("\ninto Children!\n");
            pcmd = cmd + p_num[cmd_count];
            for(int i=0; pcmd[i]!=NULL ;i++)
                printf("\npcmd = %s\n", pcmd[i]);
            if(cmd_count > 0)
            {
                dup2(fds[cmd_count][0], 0);
                close(fds[cmd_count][1]);
            }
            if(cmd_count < cmd_num-1)
            {
                dup2(fds[cmd_count+1][1], 1);
                close(fds[cmd_count+1][0]);
            }
            if(execvp(pcmd[0], pcmd)<0)
                perror("execvp error in pipe!");
            exit(0);
        }
        
    }

    return 0;
}





    //建立管道,从1开始
    for(i=1; i<=pipe_num; i++)
        if(pipe(fds[i])<0)
            perror("pipe error!");
    
    int cmd_count=0;//子进程从0开始计数， 共n个
    int pipe_count=1;//管道从1开始计数，共n-1个
    while(cmd_count < cmd_num)
    {
        printf("Loop: cmd_count = %d\n", cmd_count);
        pid_t pid;
        if((pid = fork())<0)
            perror("Fork error in pipe!");
        if(pid != 0)//父进程
        {
            int tmpfds[pipe_num+1][2];
            for(int i=1; i<=pipe_num; i++)
            {
                if(i!=cmd_count && i!=cmd_count+1)
                {
                    //printf("fds[%d][%d] = %d   ", i, 0, fds[i][0]);
                    dup2(fds[i][0], tmpfds[i][0]);
                    //printf("tmpfds[%d][%d] = %d   ", i, 0, tmpfds[i][0]);
                    //printf("fds[%d][%d] = %d   ", i, 1, fds[i][1]);
                    dup2(fds[i][1], tmpfds[i][1]);
                    //printf("tmpfds[%d][%d] = %d   ", i, 1, tmpfds[i][1]);
                    //printf("\n");
                    close(fds[i][0]);
                    close(fds[i][1]);
                }
            }
            printf("\n [%d]: parents started waiting!\n", cmd_count);

            waitpid(pid, NULL, 0);

            for(int i=1; i<=pipe_num; i++)
            {
                if(i!=cmd_count && i!=cmd_count+1)
                {
                    dup2(tmpfds[i][0], fds[i][0]);
                    dup2(tmpfds[i][1], fds[i][1]);
                    close(tmpfds[i][0]);
                    close(tmpfds[i][1]);
                }

            }
            printf("\n [%d]: parents stopped waiting!\n", cmd_count);
            cmd_count++;
            pipe_count++;
            
        }
        else
        {
            printf("\nHas come in:   ");
            printf("cmd_count = %d\n", cmd_count);

            //找到该执行的命令，保存在p_cmd中
            p_cmd = cmdline->argv + cmd_pointer[cmd_count];
            char* cmd_tmp[CMDSIZE];
            int t=0;
            if(cmd_count < cmd_num-1)
            {
                while(strcmp(p_cmd[t], "|")!=0)
                {
                    cmd_tmp[t] = p_cmd[t];
                    printf("\ncmd_tmp[%d]: %s\n", t, cmd_tmp[t]);
                    t++;
                }
                cmd_tmp[t] = NULL;
            }
            if(cmd_count == cmd_num-1)
            {
                while(p_cmd[t]!=NULL)
                {
                    cmd_tmp[t] = p_cmd[t];
                    printf("\ncmd_tmp[%d]: %s\n", t, cmd_tmp[t]);
                    t++;
                }
                cmd_tmp[t] = NULL;
            }
            

            //调整输入输出
            //执行第一条指令的进程只重定向标准输出，执行最后一条的指令只重定向标准输入
            //其余指令重定向标准输入和标准输出
            if(cmd_count < cmd_num-1)
            {
                //close(STD_OUTPUT);
                dup2(fds[cmd_count+1][1], STD_OUTPUT);
                close(fds[cmd_count+1][0]);
                //close(fds[cmd_count+1][1]);
                //fprintf(stdout, "has closed std_output\n");
            }
            if(cmd_count > 0)
            {
                //close(STD_INPUT);
                close(fds[cmd_count][1]);
                dup2(fds[cmd_count][0], STD_INPUT);
                //close(fds[cmd_count][0]);
                //fprintf(stdout, "has closes std_input\n");
            }

            fprintf(stdout, "begin to execvp!");
            if(execvp(cmd_tmp[0], cmd_tmp)<0)
                perror("Execvp error in pipe!");

            exit(0);

        }
        
    }
    return;

}


void pipe_cmd(struct command_type *cmdline)
{
    char ** p_cmd = cmdline->argv;
    int cmd_pointer[CMDNUM];//记录每一条指令开始的位置
    int fds[4][2];
    pid_t pid;

    //计算需要多少管道
    int i=0;
    cmd_pointer[i++] = 0;
    for(int j=0; j<cmdline->argc; j++)
    {
        if(strcmp(cmdline->argv[j],"|")==0)
        {
            //pipe_num++;
            cmd_pointer[i++] = j+1;
        }
    }
    //cmd_num = pipe_num+1; 
    //printf("\n pipe_num = %d\n", pipe_num);

    //建立管道,从1开始
    for(i=1; i<=3; i++)
        if(pipe(fds[i])<0)
            perror("pipe error!");

    //first direction
    char* cmd_tmp1[CMDSIZE];
    int t=0;
     while(strcmp(p_cmd[t], "|")!=0)
        {
            cmd_tmp1[t] = p_cmd[t];
            printf("\ncmd_tmp[%d]: %s\n", t, cmd_tmp1[t]);
            t++;
        }
        cmd_tmp1[t] = NULL;
    
    if((pid = fork())<0)
        perror("fork error!");
    if(pid!=0)
    {
            int tmpfds[4][2];
            for(int i=1; i<=3; i++)
            {
                    dup2(fds[i][0], tmpfds[i][0]);
                    dup2(fds[i][1], tmpfds[i][1]);
                    close(fds[i][0]);
                    close(fds[i][1]);
            }

            printf("\n [0]: parents started waiting!\n");

            waitpid(pid, NULL, 0);

            for(int i=1; i<=3; i++)
            {
                    dup2(tmpfds[i][0], fds[i][0]);
                    dup2(tmpfds[i][1], fds[i][1]);
                    close(tmpfds[i][0]);
                    close(tmpfds[i][1]);
            }
        
    }
    else
    {
        dup2(fds[1][1], STD_OUTPUT);
        close(fds[1][0]);
        if(execvp(cmd_tmp1[0], cmd_tmp1)<0)
            perror("Execvp error in pipe!");
    }

    //second direction
    p_cmd = cmdline->argv + cmd_pointer[1];
    char* cmd_tmp2[CMDSIZE];
    t=0;
   
        while(strcmp(p_cmd[t], "|")!=0)
        {
            cmd_tmp2[t] = p_cmd[t];
            printf("\ncmd_tmp[%d]: %s\n", t, cmd_tmp2[t]);
            t++;
        }
        cmd_tmp2[t] = NULL;
   
    if((pid = fork())<0)
        perror("fork error!");
    if(pid!=0)
    {
            /*int tmpfds[4][2];
            for(int i=1; i<=3; i++)
            {
                if(i!=1)
                {
                    dup2(fds[i][1], tmpfds[i][1]);
                    close(fds[i][1]);
                }
                if(i!=2)
                {
                    dup2(fds[i][0], tmpfds[i][0]);
                    close(fds[i][0]);
                }
            }*/
            printf("\n [1]: parents started waiting!\n");

            waitpid(pid, NULL, 0);

            /*for(int i=1; i<=3; i++)
            {
                if(i!=2)
                {
                    dup2(tmpfds[i][0], fds[i][0]);
                    close(tmpfds[i][0]);
                }
                if(i!=1)
                {
                    close(tmpfds[i][1]);
                    dup2(tmpfds[i][1], fds[i][1]);
                }
            }*/
        
    }
    else
    {
        dup2(fds[2][1], STD_OUTPUT);
        close(fds[2][0]);
        dup2(fds[1][0], STD_INPUT);
        close(fds[1][1]);
        if(execvp(cmd_tmp2[0], cmd_tmp2)<0)
            perror("Execvp error in pipe!");
    }

    //third direction
    p_cmd = cmdline->argv + cmd_pointer[2];
    char* cmd_tmp3[CMDSIZE];
    t=0;
   
        while(p_cmd[t]!=NULL)
        {
            cmd_tmp3[t] = p_cmd[t];
            printf("\ncmd_tmp[%d]: %s\n", t, cmd_tmp3[t]);
            t++;
        }
        cmd_tmp3[t] = NULL;
    
    if((pid = fork())<0)
        perror("fork error!");
    if(pid!=0)
    {
            int tmpfds[4][2];
            for(int i=1; i<=3; i++)
            {
                    dup2(fds[i][0], tmpfds[i][0]);
                    dup2(fds[i][1], tmpfds[i][1]);
                    close(fds[i][0]);
                    close(fds[i][1]);
            }
            printf("\n [2]: parents started waiting!\n");

            waitpid(pid, NULL, 0);
            exit(0);
        
    }
    else
    {
        dup2(fds[2][0], STD_INPUT);
        close(fds[2][1]);
        if(execvp(cmd_tmp3[0], cmd_tmp3)<0)
            perror("Execvp error in pipe!");
    }

}


void pipe_cmd(struct command_type *cmdline)
{
    char ** p_cmd = cmdline->argv;
    int cmd_pointer[CMDNUM+1];//记录每一条指令开始的位置
    int fds[PIPENUM+1][2];
    int pipe_num=0;
    int cmd_num=0;
    pid_t pid;
    pid_t pid0;

    //计算需要多少管道
    //cmd从1开始计数
    int i=0;
    cmd_pointer[i++] = 0;
    for(int j=0; j<cmdline->argc; j++)
    {
        if(strcmp(cmdline->argv[j],"|")==0)
        {
            pipe_num++;
            cmd_pointer[i++] = j+1;
        }
    }
    cmd_num = pipe_num+1; 
    printf("\n pipe_num = %d\n", pipe_num);

    //建立管道,从1开始
    for(i=1; i<=pipe_num; i++)
        if(pipe(fds[i])<0)
            perror("pipe error!");
    
    for(int pro_count=cmd_num; pro_count>0; pro_count--)
    {
        if((pid=fork())<0)
            perror("fork error!");
        if(pid!=0)
        {
            if(pro_count==1)
                pid0 = pid;
        }
        else
        {
                //找到命令
                //printf("Into Child[%d]\n", cmd_num - pro_count + 1);
                p_cmd = cmdline->argv + cmd_pointer[pro_count-1];
                char* cmd_tmp[CMDSIZE];
                int t=0;
                if(pro_count < cmd_num)
                {
                    while(strcmp(p_cmd[t], "|")!=0)
                    {
                        cmd_tmp[t] = p_cmd[t];
                        printf("\nIn Child[%d]: cmd_tmp[%d]: %s\n", pro_count, t, cmd_tmp[t]);
                        t++;
                    }
                    cmd_tmp[t] = NULL;
                }
                if(pro_count == cmd_num)
                {
                    while(p_cmd[t]!=NULL)
                    {
                        cmd_tmp[t] = p_cmd[t];
                        printf("\nIn Child[%d]: cmd_tmp[%d]: %s\n", pro_count, t, cmd_tmp[t]);
                        t++;
                    }
                    cmd_tmp[t] = NULL;
                }

            //重定向文件读写并关闭其他管道两端读写
            if(pro_count==1)
            {
                dup2(fds[1][1], STD_OUTPUT);
                close(fds[1][0]);
                for(int m=2; m<=pipe_num; m++)
                {
                    close(fds[m][1]);
                    close(fds[m][0]);
                }
            }
            else
                if(pro_count==cmd_num)
                {
                    dup2(fds[pipe_num][0], STD_INPUT);
                    close(fds[pipe_num][1]);
                    for(int m=1; m<pipe_num; m++)
                    {
                        close(fds[m][1]);
                        close(fds[m][0]);
                    }
                }
                else
                {
                    for(int m=1; m<=pipe_num; m++)
                    {
                        if(m==pro_count-1)
                        {
                            close(fds[m][1]);
                            dup2(fds[m][0], STD_INPUT);
                        }else
                            if(m==pro_count)
                            {
                                close(fds[m][0]);
                                dup2(fds[m][1], STD_OUTPUT);
                            }
                            else
                            {
                                close(fds[m][0]);
                                close(fds[m][1]);
                            }

                    }
                }
                fprintf(stdout, "Begin to execvp in child[%d]\n", pro_count);
                if(execvp(p_cmd[0], p_cmd)<0)
                    perror("execvp error!");
        }
    }
    waitpid(pid0, NULL, 0);
    exit(0);
    
}




void pipe_cmd(struct command_type *cmdline)
{
    char ** p_cmd = cmdline->argv;
    int cmd_pointer[CMDNUM+1];//记录每一条指令开始的位置
    int fds[PIPENUM+1][2];
    int pipe_num=0;
    int cmd_num=0;
    //pid_t pid;
    //pid_t pid0;

    //计算需要多少管道
    //cmd从1开始计数
    int i=0;
    cmd_pointer[i++] = 0;
    for(int j=0; j<cmdline->argc; j++)
    {
        if(strcmp(cmdline->argv[j],"|")==0)
        {
            pipe_num++;
            cmd_pointer[i++] = j+1;
        }
    }
    cmd_num = pipe_num+1; 
    printf("\n pipe_num = %d\n", pipe_num);

    //建立管道,从1开始
    for(i=1; i<=pipe_num; i++)
        if(pipe(fds[i])<0)
            perror("pipe error!");
    
    for(int pro_count=cmd_num; pro_count>0; pro_count--)
    {
        if((pid=fork())<0)
            perror("fork error!");
        if(pid!=0)
        {
            if(pro_count==1)
                pid0 = pid;
        }
        else
        {
                //找到命令
                printf("Into Child[%d]\n", pro_count);
                p_cmd = cmdline->argv + cmd_pointer[pro_count-1];
                char* cmd_tmp[CMDSIZE];
                int t=0;
                if(pro_count < cmd_num)
                {
                    while(strcmp(p_cmd[t], "|")!=0)
                    {
                        cmd_tmp[t] = p_cmd[t];
                        printf("\nIn Child[%d]: cmd_tmp[%d]: %s\n", pro_count, t, cmd_tmp[t]);
                        t++;
                    }
                    cmd_tmp[t] = NULL;
                }
                if(pro_count == cmd_num)
                {
                    while(p_cmd[t]!=NULL)
                    {
                        cmd_tmp[t] = p_cmd[t];
                        printf("\nIn Child[%d]: cmd_tmp[%d]: %s\n", pro_count, t, cmd_tmp[t]);
                        t++;
                    }
                    cmd_tmp[t] = NULL;
                }

            //重定向文件读写并关闭其他管道两端读写
            if(pro_count==1)
            {
                printf("In Chile[%d]: redirection\n", pro_count);
                dup2(fds[1][1], STD_OUTPUT);
                close(fds[1][0]);
                for(int m=2; m<=pipe_num; m++)
                {
                    close(fds[m][1]);
                    close(fds[m][0]);
                }
            }
            else
                if(pro_count==cmd_num)
                {
                    printf("In Chile[%d]: redirection\n", pro_count);
                    dup2(fds[pipe_num][0], STD_INPUT);
                    close(fds[pipe_num][1]);
                    for(int m=1; m<pipe_num; m++)
                    {
                        close(fds[m][1]);
                        close(fds[m][0]);
                    }
                }
                else
                {
                    printf("In Chile[%d]: redirection\n", pro_count);
                    for(int m=1; m<=pipe_num; m++)
                    {
                        if(m==pro_count-1)
                        {
                            close(fds[m][1]);
                            dup2(fds[m][0], STD_INPUT);
                        }else
                            if(m==pro_count)
                            {
                                close(fds[m][0]);
                                dup2(fds[m][1], STD_OUTPUT);
                            }
                            else
                            {
                                close(fds[m][0]);
                                close(fds[m][1]);
                            }

                    }
                }
                //fprintf(stdout, "Begin to execvp in child[%d]\n", pro_count);
                if(execvp(p_cmd[0], p_cmd)<0)
                    perror("execvp error!");
        }
    }
    //waitpid(pid0, NULL, 0);
    //exit(0);
    
}