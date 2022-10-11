#include "types.h"
#include "stat.h"
#include "user.h"

#define PNUM 5 // 프로세스 개수
#define PRINT_CYCLE 100000000 // 정보 출력 주기
#define TOTAL_COUNTER 500000000 // 종료 시 counter 값

void sdebug_func(void)
{
    // int n;
    int pid;
    int i;
    int counter = TOTAL_COUNTER;

    int starttime, endtime;
    starttime = uptime();
    printf(1, "start sdebug command\n");

    for(i = 0; i < PNUM; i++){
        if((pid = fork()) == 0)
            break;
    }

    // for(i = 0; i < PNUM; i++){
        // pid = fork(); // 프로세스 생성
        // if(pid < 0){
        //     printf(1, "fork error\n");
        //     exit();
        // }
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
    // }
    return;
}

// void sdebug_func(void)
// {
//     // int n;
//     int pid;
//     int i;

//     printf(1, "start sdebug command\n");

//     int starttime = uptime();
//     for(i = 0; i < PNUM; i++){
//         pid = fork(); // 프로세스 생성
//         if(pid < 0){
//             printf(1, "fork error\n");
//             exit();
//         }
//         else if(pid == 0){ // 자식 코드
//             // TOTAL_COUNTER만큼 반복
//             for(uint counter = 0; counter < TOTAL_COUNTER; counter++){
//                 if(counter == PRINT_CYCLE){                    
//                     // TOTAL_COUNTER 프로세스 종료시 카운터 값
//                     int endtime = uptime();
//                     weightset(i+1); // weight 값 1씩 증가하면서 부여
//                     // todo:프로세스 정보 출력
//                     printf(1, "PID: %d, WEIGHT: %d, TIMES: %d ms\n", getpid(), i+1, (endtime-starttime) * 10);
//                 }
//             }
//             printf(1, "PID: %d terminated\n", getpid());
//             exit();
//         }
//     }
//     for(; i > 0; i--){
//         if(wait() < 0){
//             printf(1, "wait stopped early\n");
//             exit();
//         }
//     }
//     printf(1, "end of sdebug command\n");
//     return;
// }

int main(void)
{
    sdebug_func();
    exit();
}
