#include"mshell.h"

void init_shell()//！！！！输出初始化信息
{
    historys->his_num=0;
}

void init_loop()
{
    cmdline->cmd_info = REGULAR;
    cmdline->background = 0;
    memset(cmd, '\0', CMDSIZE);
    printf("$$: ");
}

void read_command(char cmd[])
{
    fgets(cmd, CMDSIZE, stdin);
    //最后一句,便于之后执行命令
    cmd[strlen(cmd)-1] = '\0';
    update_history(cmd);
}

void command(char* cmd, struct command_type cmdline[])
{
    //按照空格 和 管道符号对命令进行分割，保存在argv数组中
    //默认字符之间单词之间一定有空格
    parse_cmd(cmd, cmdline);

    //判断是否为内置命令，如是，直接执行, 返回1；否则返回0
    if(builtin_cmd(cmdline))
        return;

    //先判断是否含有管道，所有含管道的命令都由函数pipe()执行
    if(check('|'))
    {
        pipe_cmd(cmdline);//????是否有必要标记每一种cmd的info
        return;
    }

    if(check('>') || check('<'))
    {
        for(int i=0; i<cmdline->argc; i++)
        {
            if(strcmp(cmdline->argv[i], ">") == 0)
            {
                strcpy(cmdline->outfile, cmdline->argv[i+1]);
                cmdline->cmd_info = REDIRECT_OUT;
                cmdline->argv[i] = 0;
                break;
            }
            if(strcmp(cmdline->argv[i], ">>") == 0)
            {
                strcpy(cmdline->outfile, cmdline->argv[i+1]);
                cmdline->cmd_info = APPEND_OUT;
                cmdline->argv[i] = 0;
                break;
            }
            if(strcmp(cmdline->argv[i], "<") == 0)
            {
                strcpy(cmdline->infile, cmdline->argv[i+1]);
                cmdline->cmd_info = REDIRECT_IN;
                cmdline->argv[i] = 0;
                break;
            }
        }
    }

    execute_command(cmdline);
}

//检查字符x是否存在于命令中，是则返回1，否则返回0
int check(char x)
{
    for(int i=0; i<cmdline->argc; i++)
    {
        char* tmp = cmdline->argv[i];
        for(int j=0; j<strlen(cmdline->argv[i]); j++)
        {
            if(*(tmp+j) == x)
                return 1;
        }
    }
    return 0;
}

//处理没有管道的情况，保存空格分割的命令和字符串
void parse_cmd(char* cmd, struct command_type* cmdline)
{
    //char *cmd1 = malloc(sizeof(char)*(strlen(cmd)+1));//避免strtok函数破坏愿字符
    const char *d = " ";
    char *p = strtok(cmd,d);
    int i=0;
    while(p)
    {
        cmdline->argv[i++] = p;
        p=strtok(NULL,d);
    }
    cmdline->argv[i]=NULL;
    cmdline->argc = i;

    //判断是否为后台命令
    if(strcmp(cmdline->argv[(cmdline->argc)-1], "&") == 0)
    {
        cmdline->argv[(cmdline->argc)-1]=0;
        cmdline->background = 1;
    }
    return;
}

//判断是否为内置命令，如是，直接执行, 返回1；否则返回0
int builtin_cmd(struct command_type cmdline[])
{
    if(strcmp(cmdline[0].argv[0],"cd")==0)
    {
        if(chdir(cmdline[0].argv[1])<0)
            perror("change directory error!");
        return 1;
    }

    if(strcmp(cmdline[0].argv[0], "exit")==0)
        exit(0);

    if(strcmp(cmdline[0].argv[0], "history")==0)
    {
        if(cmdline[0].argc ==  1)
            print_history(-1);
        else
            if(cmdline[0].argc == 2)
                print_history(*cmdline[0].argv[1]-'0');
            else
                perror("History command invald!");
        return 1;
    }

    return 0;
}

//专门处理管道
void pipe_cmd(struct command_type *cmdline)
{
    char ** p_cmd = cmdline->argv;
    int cmd_pointer[CMDNUM];//记录每一条指令开始的位置
    int pipe_num=0;
    int cmd_num=0;
    int fds[PIPENUM+1][2];

    //计算需要多少管道
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
        pipe(&fds[i][0]);
    
    int cmd_count=0;//子进程从0开始计数， 共n个
    int pipe_count=1;//管道从1开始计数，共n-1个
    while(cmd_count < cmd_num)
    {
        pid_t pid;
        if((pid = fork())<0)
            perror("Fork error in pipe!");
        if(pid != 0)//父进程
        {
            
            
            int tmpfds[pipe_num+1][2];
            for(int i=1; i<=pipe_num; i++)
            {
                dup2(fds[i][0], tmpfds[i][0]);
                dup2(fds[i][1], tmpfds[i][1]);
                close(fds[i][0]);
                close(fds[i][1]);
            }
            printf("\n [%d]: parents started waiting!\n", cmd_count);
            waitpid(pid, NULL, 0);

            for(int i=1; i<=pipe_num; i++)
            {
                dup2(tmpfds[i][0], fds[i][0]);
                dup2(tmpfds[i][1], fds[i][1]);
            }

            cmd_count++;
            pipe_count++;
            printf("\n [%d]: parents stopped waiting!\n", cmd_count);
        }
        else
        {
            printf("\nHas come in\n");
            printf("\ncmd_count = %d\n", cmd_count);

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
            
            if(cmd_count<cmd_num-1)
                printf("begin to close std_output\n");
            if(cmd_count>0)
                printf("begin to close std_input\n");

            if(cmd_count < cmd_num-1)
            {
                close(STD_OUTPUT);
                dup2(fds[pipe_count][1], STD_OUTPUT);
                close(fds[pipe_count][0]);
            }
            if(cmd_count > 0)
            {
                close(STD_INPUT);
                dup2(fds[pipe_count-1][0], STD_INPUT);
                close(fds[pipe_count-1][1]);
            }

            execvp(cmd_tmp[0], cmd_tmp);

        }
        //pipe_count++;
        
    }
    return;

}

void execute_command(struct command_type* cmdline)
{
    pid_t pid;

    if((pid = fork())<0)
    {
        perror("Fork error!");
    }
    else
        if(pid!=0)//父进程
        {
            if(cmdline->background==1)//若为后台进程
                signal(SIGCHLD, SIG_IGN);
            else
                signal(SIGCHLD, SIG_DFL);

            waitpid(pid, NULL, 0);
        }
        else//子进程
        {
            int tmp_std;
            switch(cmdline->cmd_info)
            {
                case REGULAR:
                {
                    if(execvp(cmdline->argv[0], cmdline->argv)==-1)
                        perror("Execvp error!");
                };break;
                case REDIRECT_IN: 
                {
                    tmp_std = dup(STD_INPUT);
                    close(STD_INPUT);
                    if(open(cmdline->infile, O_RDONLY)==-1)
                        perror("Can't open new input file!");
                    if(execvp(cmdline->argv[0], cmdline->argv)==-1)
                        perror("Execvp error!");
                        //!!!!还没有恢复标准输入
                };break;
                case REDIRECT_OUT:
                {
                    //printf("\nstdout has changed\n");
                    tmp_std = dup(STD_OUTPUT);
                    close(STD_OUTPUT);
                    if(open(cmdline->outfile, O_WRONLY| O_CREAT, 0666)==-1)
                        perror("Can't open new output file!");
                    if(execvp(cmdline->argv[0], cmdline->argv)==-1)
                        perror("Execvp error!");
                };break;
                case APPEND_OUT:
                {
                    tmp_std = dup(STD_OUTPUT);
                    close(STD_OUTPUT);
                    if(open(cmdline->outfile, O_WRONLY| O_APPEND | O_CREAT, 0666)==-1)
                        perror("Can't open new output file!");
                    if(execvp(cmdline->argv[0], cmdline->argv)==-1)
                        perror("Execvp error!");
                };
                
                    
            }
                
            switch(cmdline->cmd_info)
            {
                case REDIRECT_IN: 
                {
                    close(STD_INPUT);
                    dup(tmp_std);
                };break;
                case REDIRECT_OUT:
                {
                    close(STD_OUTPUT);
                    dup(tmp_std);
                };break;
                case APPEND_OUT:
                {
                    close(STD_OUTPUT);
                    dup(tmp_std);
                };
            }
            exit(0);
        }
    
}



void update_history(char cmd[])
{
    if(cmd==NULL)
        return;
    char* tmp=(char*)malloc(sizeof(char)* strlen(cmd));
    strcpy(tmp, cmd);
    historys->history_cmd[historys->his_num] = tmp;
    historys->his_num++;
}

void print_history(int n)
{
    if(n == -1)//print all the historys
    {
        for(int i=0; i<historys->his_num; i++)
            printf("Command [%d]: %s\n", i+1, historys->history_cmd[i]);
        return;
    }
        
    if(n > HISNUM)
        perror("History number error!");

    for(int i=0; i<n; i++)
        printf("Command [%d]: %s\n", i+1, historys->history_cmd[i]);
    return;
}