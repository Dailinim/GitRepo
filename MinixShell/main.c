#include "mshell.c"

int main(int argc, char const *argv[])
{
    //1.initiation
    //2.loop of reading and executing: 
    //read command-->parse command
    //execute command-->execute internal command-->find shell internal command(if not--->find other command)

    cmdline=(struct command_type*)malloc(sizeof(struct command_type));
    historys=(struct historys_type*)malloc(sizeof(struct historys_type));
    init_shell();//每次启动shell时调用

    while(1){
        
        //输出提示符
        init_loop();

        //读取命令到cmd字符数组中
        read_command(cmd);
        
        //分析并执行命令
        command(cmd, cmdline);

        
    }
}
