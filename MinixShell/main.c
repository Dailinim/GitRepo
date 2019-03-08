#include<stdio.h>
#include<cstring>
#define BUFSIZE 100

void read_command(char* cmd){
   cmd = fgets(cmd, BUFSIZE, stdin);
   
}
int main(int argc, char const *argv[])
{
    //1.initiation
    //2.loop of reading and executing: 
    //read command-->parse command
    //execute command-->execute internal command-->find shell internal command(if not--->find other command)
    
    char cmdline[BUFSIZE];
    int cmdlen;
    while(1){
        print_prompt_sign();
        read_command(cmdline, cmdlen);

        if(cmd!=NULL)
    }
}
