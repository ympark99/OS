#include "types.h"
#include "user.h"

#define T_TRACE 1
#define T_FORK 2
#define T_UNTRACE 0

int main(int argc, char *argv[])
{
    int masks = atoi(argv[1]);

    char* cmd_argv[argc-2];
    for(int i = 0; i < argc - 2; i++){
        cmd_argv[i] = argv[i+2];
    }

    int pid = fork();
    if(pid < 0){
        printf(1, "fork error\n");
        exit();
    }
    else if(pid == 0){
        trace(masks);
        exec(cmd_argv[0], cmd_argv);
        exit();
    }
    else
        wait();

    exit();
}
