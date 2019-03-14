#define CMDSIZE 100
#define HISNUM 50
#define CMDNUM 10
#define FNAMESIZE 20
#define PIPENUM 20
#define STD_INPUT 0
#define STD_OUTPUT 1
#include <unistd.h> //dup()
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>//open函数及参数

enum info_type {REGULAR, REDIRECT_IN, REDIRECT_OUT, APPEND_OUT};
struct command_type
{
    enum info_type cmd_info;//保存该命令种类：INTERAL内置, PROGRAM普通外部命令, REDIRECTION含重定向, PIPE管道, BACK后台
    int background;
    char* argv[CMDNUM];//argv[0]保存命令，[1]及之后保存参数 
    int argc;//argv数组中有多少项
    //输入与输出
    char infile[FNAMESIZE];
    char outfile[FNAMESIZE];
};

struct historys_type
{
    int his_num;
    char* history_cmd[HISNUM];
};

char cmd[CMDSIZE+1];
struct command_type* cmdline;
struct historys_type* historys;

void init_shell();

void init_loop();

void read_command(char* cmd);

void command(char* cmd, struct command_type cmdline[]);

int check(char x);

void parse_cmd(char* cmd, struct command_type* cmdline);

int builtin_cmd(struct command_type cmdline[]);

void pipe_cmd(char* cmd);

void execute_command();

void update_history(char *cmd);

void print_history(int n);
