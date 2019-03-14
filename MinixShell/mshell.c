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
    //按照空格对命令进行分割，保存在argv数组中
    //默认字符之间单词之间一定有空格
    parse_cmd(cmd, cmdline);

    //判断是否为内置命令，如是，直接执行, 返回1；否则返回0
    if(builtin_cmd(cmdline))
        return;

    //先判断是否含有管道，所有含管道的命令都由函数pipe()执行
    if(check('|'))
    {
        pipe_cmd(cmd, cmdline);//????是否有必要标记每一种cmd的info
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
    cmdline->argv[i]=0;
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
                    printf("\nstdout has changed\n");
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