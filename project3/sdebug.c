#include "types.h"
#include "stat.h"
#include "user.h"

#define PNUM 5 // 프로세스 개수
#define PRINT_CYCLE 100000000 // 정보 출력 주기
#define TOTAL_COUNTER 500000000 // 종료 시 counter 값

void sdebug_func(void)
{
    int pid, i, starttime, endtime, counter;
    
    counter = TOTAL_COUNTER;
    starttime = uptime();
    printf(1, "start sdebug command\n");

    for(i = 0; i < PNUM; i++){
        if((pid = fork()) == 0) // 프로세스 생성
            break;
        else if(pid < 0){
            printf(1, "fork error\n");
        }
    }

    if(pid == 0){ // 자식 코드
        weightset(i+1); // weight 값 1씩 증가하면서 부여
        pid = getpid();
        // TOTAL_COUNTER만큼 반복
        for(counter = 0; counter < TOTAL_COUNTER; counter++){
            if(counter == PRINT_CYCLE){                    
                // TOTAL_COUNTER 프로세스 종료시 카운터 값
                endtime = uptime();
                printf(1, "PID: %d, WEIGHT: %d, TIMES: %d ms\n", pid, i+1, (endtime-starttime) * 10);
            }
        }            
        printf(1, "PID: %d terminated\n", pid);
    }
    else{
        for(i = 0; i < PNUM; i++){
            wait();
        }
        printf(1, "end of sdebug command\n");
    }
    return;
}

int main(void)
{
    sdebug_func();
    exit();
}
