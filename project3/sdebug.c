#include "types.h"
#include "stat.h"
#include "user.h"

#define PNUM 5 // 프로세스 개수
#define PRINT_CYCLE 10000000 // 정보 출력 주기
#define TOTAL_COUNTER 500000000 // 종료 시 counter 값

void sdebug_func(void)
{
    // int n;
    int pid;

    printf(1, "start sdebug command\n");

    int starttime = uptime();
    for(int i = 0; i < PNUM; i++){
        pid = fork(); // 프로세스 생성
        if(pid < 0){
            printf(1, "fork error\n");
            exit();
        }
        else if(pid == 0){ // 자식 코드
            // TOTAL_COUNTER만큼 반복
            for(uint counter = 0; counter < TOTAL_COUNTER; counter++){
                if(counter == PRINT_CYCLE){                    
                    // TOTAL_COUNTER 프로세스 종료시 카운터 값
                    int endtime = uptime();
                    weightset(i+1); // weight 값 1씩 증가하면서 부여
                    // todo:프로세스 정보 출력
                    printf(1, "PID: %d, WEIGHT: %d, TIMES: %d ms\n", getpid(), i+1, (endtime-starttime) * 10);
                }
            }
            exit();
        }
        else
            wait(); // 대기    
    }
    printf(1, "end of sdebug command\n");
    return;
}

int main(void)
{
    sdebug_func();
    exit();
}
