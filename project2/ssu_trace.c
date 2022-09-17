#include "types.h"
#include "user.h"

#define T_TRACE 1
#define T_FORK 2
#define T_UNTRACE 0

int main(int argc, char *argv[])
{
    int masks = atoi(argv[1]); // 입력한 mask 값 저장

    char* cmd_argv[argc-2]; // exec 실행 시 인자 넘기기 위해 선언
    for(int i = 0; i < argc - 2; i++){
        cmd_argv[i] = argv[i+2]; // 입력한 인자 중 command 부분만 넣어줌
    }

    int pid = fork(); // fork() 실행
    if(pid < 0){
        printf(1, "fork error\n");
        exit();
    }
    else if(pid == 0){ // 자식 코드
        trace(masks); // 인자로 받은 mask 값 넣어 trace() 실행
        exec(cmd_argv[0], cmd_argv); // exec 실행, command에 입력한 인자 넘겨줌
        exit();
    }
    else
        wait(); // 대기

    exit();
}
