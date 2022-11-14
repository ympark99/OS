#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "page.h"

int frame; // 프레임 수
int inputType; // 1 -> 랜덤, 2 -> 파일
int randoms[RAND_MIN];
int pages = 30;
int rw[RAND_MIN]; // 0 -> R, 1 -> W
char filename[BUF_SIZE];

int findLRU(int time[], int n){
    int i, minimum = time[0], pos = 0;
    
    for(i = 1; i < n; i++){
        if(time[i] < minimum){
            minimum = time[i];
            pos = i;
        }
    }
    return pos;
}

// lru
void lru(){
    fprintf(stdout, "\n LRU\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    int frames[frame], counter = 0, time[10], flag1, flag2, i, j, pos, faults = 0;


    for(i = 0; i < frame; i++){
        frames[i] = -1;
    }

    fprintf(stdout, "Incoming  \t");
    for(int i = 0; i < frame; i++){
        fprintf(stdout, "Frame %d \t", i+1);
    }

    for(i = 0; i < pages; i++){
        flag1 = flag2 = 0;
        
        for(j = 0; j < frame; j++){
            if(frames[j] == randoms[i]){
                counter++;
                time[j] = counter;
                flag1 = flag2 = 1;
                break;
            }
        }
            
        if(flag1 == 0){
            for(j = 0; j < frame; j++){
                if(frames[j] == -1){
                    counter++;
                    faults++;
                    frames[j] = randoms[i];
                    time[j] = counter;
                    flag2 = 1;
                    break;
                }
            }
        }
            
        if(flag2 == 0){
            pos = findLRU(time, frame);
            counter++;
            faults++;
            frames[pos] = randoms[i];
            time[pos] = counter;
        }


        printf("\n");
        printf("%d\t\t", randoms[i]);
        for(j = 0; j < frame; j++){
            if(frames[j] != -1)
                printf("%d\t\t", frames[j]);
            else
                printf(" - \t\t");
        }
    }
    printf("\nTotal Page Faults = %d\n", faults);
}

// lifo
void lifo(){
    fprintf(stdout, "\n LIFO\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

}

// optimal
void optimal(){
    fprintf(stdout, "\n Optimal\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    int frames[frame], temp[frame], flag1, flag2, flag3, i, j, k, pos, max, faults = 0;
    for(i = 0; i < frame; i++){
        frames[i] = -1;
    }

    fprintf(stdout, "Incoming  \t");
    for(int i = 0; i < frame; i++){
        fprintf(stdout, "Frame %d \t", i+1);
    }
    for(i = 0; i < pages; i++){
        flag1 = flag2 = 0;
        
        for(j = 0; j < frame; j++){
            if(frames[j] == randoms[i]){
                flag1 = flag2 = 1;
                break;
            }
        }
        
        if(flag1 == 0){
            for(j = 0; j < frame; j++){
                if(frames[j] == -1){
                    faults++;
                    frames[j] = randoms[i];
                    flag2 = 1;
                    break;
                }
            }    
        }        
        if(flag2 == 0){
            flag3 =0;
        
            for(j = 0; j < frame; j++){
                temp[j] = -1;
            
                for(k = i + 1; k < pages; ++k){
                    if(frames[j] == randoms[k]){
                        temp[j] = k;
                        break;
                    }
                }
            }
            
            for(j = 0; j < frame; j++){
                if(temp[j] == -1){
                    pos = j;
                    flag3 = 1;
                    break;
                }
            }
            
            if(flag3 == 0){
                max = temp[0];
                pos = 0;
            
                for(j = 1; j < frame; j++){
                    if(temp[j] > max){
                        max = temp[j];
                        pos = j;
                    }
                }            
            }
            frames[pos] = randoms[i];
            faults++;
        }
        
        printf("\n");
        printf("%d\t\t", randoms[i]);
        for(j = 0; j < frame; j++){
            if(frames[j] != -1)
                printf("%d\t\t", frames[j]);
            else
                printf(" - \t\t");
        }
    }
    
    printf("\nTotal Page Faults : %d\n", faults);
}

// FIFO
void fifo(){
    fprintf(stdout, "\n FIFO\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    int pageFaults = 0;
    int m, n, s;

    fprintf(stdout, "Incoming  \t");
    for(int i = 0; i < frame; i++){
        fprintf(stdout, "Frame %d \t", i+1);
    }
    int temp[frame];
    for(m = 0; m < frame; m++){
        temp[m] = -1;
    }

    for(m = 0; m < pages; m++){
        s = 0;

        for(n = 0; n < frame; n++){
            if(randoms[m] == temp[n]){
                s++;
                pageFaults--;
            }
        }
        pageFaults++;
        
        if((pageFaults <= frame) && (s == 0)){
            temp[m] = randoms[m];
        }
        else if(s == 0){
            temp[(pageFaults - 1) % frame] = randoms[m];
        }

        printf("\n");
        printf("%d\t\t", randoms[m]);
        for(n = 0; n < frame; n++){
            if(temp[n] != -1)
                printf(" %d\t\t", temp[n]);
            else
                printf(" - \t\t");
        }
    }

    printf("\nTotal Page Faults : %d\n", pageFaults);
    return;
}

void start(){
    int num = 0;
    int algo_run[3] = {-1, -1, -1};
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
            if(idx < 3)	algo_run[idx] = atoi(ptr);
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
                if(algo_run[i] < 1 || algo_run[i] > 8){
                    fprintf(stderr, "1~8중 입력\n");
                    goNext = false;
                    break;
                }
            }
        }
        if(goNext) break;
        memset(algo_run, -1, sizeof(algo_run));
    }

    while (1){
        fprintf(stdout, "-----------------------------------------------------------------------------\n");
        fprintf(stdout, "B. 페이지 프레임의 개수를 입력하시오. (3~10)\n");
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
        fprintf(stdout, ">> ");
        scanf("%d", &inputType);
        if(inputType != 1 && inputType != 2){
            fprintf(stderr, "1또는 2 입력\n");
        }
        else break;
    }
    // 1 선택시 랜덤생성 후 출력
    if(inputType == 1){
        fprintf(stdout, "\n--------------\n");
        fprintf(stdout, "페이지 스트림\n");
        fprintf(stdout, "--------------\n");
        for(int i = 0; i < RAND_MIN; i++){
            randoms[i] = rand() % 9 + 1;
            rw[i] = rand() % 2;

            if(rw[i] == 0)
                fprintf(stdout, "%d R : %d\n", i+1, randoms[i]);
            else fprintf(stdout, "%d W : %d\n", i+1, randoms[i]);
        }
        fprintf(stdout, "--------------\n");
    }
    // 2 선택시 파일 입력 받기
    else{
        fprintf(stdout, "파일경로 입력\n");
        fprintf(stdout, ">> ");
        getchar(); // 입력 버퍼 비우기(\n)
        fgets(filename, BUF_SIZE, stdin); // 명령어 입력
    }


    for(int i = 0; i < 3; i++){
        if(algo_run[i] == -1) break;

        switch (algo_run[i]){
            case 1: // Optimal
                optimal();
                break;
            case 2: // FIFO
                fifo();
                optimal();
                break;
            case 3: // LIFO
                lifo();
                optimal();
                break;
            case 4: // LRU
                lru();
                optimal();
                break;
            case 5: // LFU
                break;
            case 6: // SC
                break;
            case 7: // ESC
                break;
            case 8: // ALL
                break;
            default:
                break;
        }
    }
}

int main(void){
    start();
    exit(0);
}
