#include "types.h"
#include "stat.h"
#include "user.h"

#define PNUM 5 // 프로세스 개수
#define PRINT_CYCLE 100000000 // 정보 출력 주기
#define TOTAL_COUNTER 500000000 // 종료 시 counter 값

void sdebug_func(void)
{
    int n, pid;

    printf(1, "start sdebug command\n");

    int pid = fork(); // 프로세스 생성
    if(pid < 0){
        printf(1, "fork error\n");
        exit();
    }
    else if(pid == 0){ // 자식 코드
        
        exit();
    }
    else
        wait(); // 대기    

    printf(1, "end of sdebug command\n");
    return;
}

int main(void)
{
    sdebug_func();
    exit();
}
