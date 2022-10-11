#include "types.h"
#include "stat.h"
#include "user.h"

#define PNUM 5 // 프로세스 개수
#define PRINT_CYCLE 10000000 // 정보 출력 주기
#define TOTAL_COUNTER 500000000 // 종료 시 counter 값s

void sdebug_func(void)
{
    // int n;
    int pid;
    int i;
    uint counter[PNUM];

    printf(1, "start sdebug command\n");

    // int starttime = uptime();
    for(i = 0; i < PNUM; i++){
        pid = fork(); // 프로세스 생성
        if(pid < 0){
            printf(1, "fork error\n");
            exit();
        }
        else if(pid == 0){ // 자식 코드
            int starttime = uptime();
            weightset(i+1); // weight 값 1씩 증가하면서 부여
            // TOTAL_COUNTER만큼 반복
            for(counter[i] = 0; counter[i] < TOTAL_COUNTER; counter[i]++){
                if(counter[i] == PRINT_CYCLE){                    
                    // TOTAL_COUNTER 프로세스 종료시 카운터 값
                    int endtime = uptime();
                    // todo:프로세스 정보 출력
                    printf(1, "PID: %d, WEIGHT: %d, TIMES: %d ms\n", getpid(), i+1, (endtime-starttime) * 10);
                }
            }
            if(counter[i] >= TOTAL_COUNTER){
                printf(1, "PID: %d terminated\n", getpid());
                exit();
            }
        }
    }
    for(; i > 0; i--){
        if(wait() < 0){
            printf(1, "wait stopped early\n");
            exit();
        }
    }
    printf(1, "end of sdebug command\n");
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
