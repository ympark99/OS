#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "page.h"

void start(){

    int num = 0;
    int frame = 0;
    int inputType = 0;
    int page_num[3] = {-1, -1, -1};
    while(1){
        fprintf(stdout, "\n-----------------------------------------------------------------------------\n");
        fprintf(stdout, "사용할 시뮬레이터 개수를 입력하세요 (최대 3개)\n");
        fprintf(stdout, ">> ");
        scanf("%d", &num);
        if(num > 3 || num < 1){
            fprintf(stderr, "1~3 수 입력\n");
        }
        else break;
    }
    getchar(); // 입력 버퍼 비우기(\n)

    while(1){
        fprintf(stdout, "\n-----------------------------------------------------------------------------\n");
        fprintf(stdout, "A. Page Replacement 알고리즘 시뮬레이터를 선택하시오\n");
        fprintf(stdout, "%d개 공백으로 구분해서 입력\n", num);
        fprintf(stdout, "(1) Optimal (2) FIFO (3) LIFO (4) LRU (5) LFU (6) SC (7) ESC (8) ALL\n");
        fprintf(stdout, ">> ");

        char *oper = malloc(sizeof(char) * BUF_SIZE);
        fgets(oper, BUF_SIZE, stdin); // 명령어 입력

        char *findOper[3] = {NULL, }; // 명령어 split        
        char *ptr = strtok(oper, " "); // 공백 기준으로 문자열 자르기
        int idx = 0;
        bool goNext = true;
        while (ptr != NULL){
            if(idx < 3)	page_num[idx] = atoi(ptr);
            idx++;
            ptr = strtok(NULL, " ");
        }
        free(oper);
        if(idx != num){
            fprintf(stderr, "%d개 공백으로 입력\n", num);
            goNext = false;
        }
        else{
            for(int i = 0; i < num; i++){
                if(page_num[i] < 1 || page_num[i] > 8){
                    fprintf(stderr, "1~8중 입력\n");
                    goNext = false;
                    break;
                }
            }
        }
        if(goNext) break;
        memset(page_num, -1, sizeof(page_num));
    }
    fprintf(stdout, "page_num : %d %d %d\n", page_num[0], page_num[1], page_num[2]);

    while (1){
        fprintf(stdout, "-----------------------------------------------------------------------------\n");
        fprintf(stdout, "B. 페이지 프레임의 개수를 입력하시오. (3~10)\n");
        fprintf(stdout, "-----------------------------------------------------------------------------\n");
        fprintf(stdout, ">> ");         
        scanf("%d", &frame);   
        if(frame < 3 || frame > 10){
            fprintf(stderr, "3~10 사이의 수 입력\n");
        }
        else break;
    }

    while (1){
        fprintf(stdout, "-----------------------------------------------------------------------------\n");
        fprintf(stdout, "C. 데이터를 입력할 방식을 선택하시오. (1,2)\n");
        fprintf(stdout, "(1) 랜덤하게 생성 (2) 사용자 생성 파일 오픈 \n");        
        fprintf(stdout, "-----------------------------------------------------------------------------\n");
        fprintf(stdout, ">> ");
        scanf("%d", &inputType);
        if(inputType != 1 && inputType != 2){
            fprintf(stderr, "1또는 2 입력\n");
        }
        else break;
    }


    // switch (page_num){
    //     case 1: // 관리자 모드
    //         master_mode();
    //         break;
    //     case 2: // 사용자 모드
    //         user_mode();
    //         break;
    //     case 3: // 종료
    //         exit(0);
    //     default:
    //         fprintf(stdout, "잘못된 입력\n");
    //         break;
    // }
}

int main(void){
    start();
    exit(0);
}
