#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "page.h"

int frame_cnt; // 프레임 수
int inputType; // 1 -> 랜덤, 2 -> 파일
int randoms[RAND_MIN];
int ps_num = 20;
int rw[RAND_MIN]; // 0 -> R, 1 -> W
char filename[BUF_SIZE];

//todo : ps_num 500, rand 30

void esc(){
    fprintf(stdout, "\n Enhanced Second Chance\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");    
    int i, j, ref;
    int pointer = 0;
    int idx = 0;
    int done;
    int faults = 0;
    int temp; // pointer keep

    // int *frames = calloc(frame_cnt, sizeof(int));
    // int *reference_bit = calloc(frame_cnt, sizeof(int));
    // int *modify_bit = calloc(frame_cnt, sizeof(int));

    int frames[frame_cnt], reference_bit[frame_cnt], modify_bit[frame_cnt];

    memset(frames, -1, sizeof(frames));
    memset(reference_bit, -1, sizeof(reference_bit));
    memset(modify_bit, -1, sizeof(modify_bit));

    // // initialize the frames to be all -1
    // for (i = 0; i < frame_cnt; i++){
    //     frames[i] = -1;
    //     modify_bit[i] = -1;
    // }

    fprintf(stdout, "Incoming  \t");
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "cnt %d \t", i+1);
    }
    fprintf(stdout, "\n");

    for(i = 0; i < ps_num; i++){
        bool isFault = false;

        printf("%d|1%d\t\t", randoms[i], rw[i]);
        ref = -1;
        // frame 탐색
        for(int j = 0; j < frame_cnt; j++){
            if( randoms[i] == frames[j] ){
                ref = j;
                break;
            }
        }

        // ref = search_frames(frames, randoms[i], frame_cnt);
        if (ref < 0){ // page fault 발생
            isFault = true;
            if (idx < frame_cnt){ // 아직 프레임 못 채웠을때
                // printf("REPLACING %d WITH %d AT: %d\n", frames[i], randoms[i], idx);
                frames[idx] = randoms[i];
                reference_bit[idx] = 1; // R bit 1로 고정
                modify_bit[idx] = rw[i];
                idx++;
            }
            else{
                done = 0;
                temp = pointer;

                // R비트 모두 1인 상태에서 다음 page fault이면 모두 0으로 변경
                bool allOne = true;
                for (j = 0; j < frame_cnt; j++){
                    if (reference_bit[j] != 1){
                        allOne = false;
                        break;
                    }
                }
                if(allOne){
                    for (j = 0; j < frame_cnt; j++){
                        reference_bit[j] = 0;
                    }
                }

                // 우선 순위 같으면, 먼저 탐색한 순서로 처리
                // 00 우선 검사
                for (j = 0; j < frame_cnt; j++){
                    // 00 1순위
                    if (reference_bit[pointer] == 0 && modify_bit[pointer] == 0){
                        done = 1;
                        break;
                    }
                    pointer = (pointer + 1) % (frame_cnt);
                }
                // 01 검사
                if (done == 0){
                    pointer = temp;
                    for (j = 0; j < frame_cnt; j++){
                        if (reference_bit[pointer] == 0 && modify_bit[pointer] == 1){
                            done = 1;
                            break;
                        }
                        pointer = (pointer + 1) % (frame_cnt);
                    }
                }
                // 10검사
                if (done == 0){
                    pointer = temp;
                    for (j = 0; j < frame_cnt; j++){
                        if (reference_bit[pointer] == 1 && modify_bit[pointer] == 0){
                            done = 1;
                            break;
                        }
                        pointer = (pointer + 1) % (frame_cnt);
                    }
                }
                // 11검사
                if (done == 0){
                    pointer = temp;
                    for (j = 0; j < frame_cnt; j++){
                        if (reference_bit[pointer] == 1 && modify_bit[pointer] == 1){
                            break;
                        }
                        pointer = (pointer + 1) % (frame_cnt);
                    }
                }

                // do this no matter what loop breaks
                frames[pointer] = randoms[i];
                reference_bit[pointer] = 1; // R bit 1로 고정
                modify_bit[pointer] = rw[i];
                // pointer = (pointer + 1) % (frame_cnt);
            }
            faults++;
        }
        else if (ref >= 0){ // page hit
            reference_bit[ref] = 1; // R bit 1로 고정
            if (rw[i] == 1)
                modify_bit[ref] = 1;
            else modify_bit[ref] = 0;
        }

        for (int k = 0; k < frame_cnt; k++){
            if(frames[k] != -1){
                printf("%d|%d%d\t", frames[k], reference_bit[k], modify_bit[k]);
            }
            else
                printf(" - \t");
        }
        if(isFault) fprintf(stdout, "page fault!");
        printf("\n");
    }
    // free(frames);
    // free(reference_bit);
    // free(modify_bit);
    printf("total page fault : %d\n", faults);
}

// second chance : 참조비트 1로 만들고 그 비트부터 검사
void sc(){
    fprintf(stdout, "\n Second Chance\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    int i;
    int full = 0;
    int count = 0;
    int frames[frame_cnt];
    int chance[frame_cnt];
    int repptr = 0;
    memset(frames, -1, sizeof(frames));
    memset(chance, 0, sizeof(chance));

    fprintf(stdout, "Incoming  \t");
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "cnt %d \t", i+1);
    }
    fprintf(stdout, "\n");
    for (i = 0; i < ps_num; i++){
        printf("%d\t\t", randoms[i]);
        int flag = 0;
        if (full != 0){
            for (int j = 0; j < full; j++)
                if (randoms[i] == frames[j]){
                    flag = 1;
                    chance[j] = 1; // When ever page reference occurs, it's rference bit is set to 1
                    break;
                }
        }        
        if (flag != 1){
            // 새로 할당
            if (full != frame_cnt){
                chance[full] = 1; // All the chance bits are set to 1 as each frames is being filled firstly
                frames[full++] = randoms[i];
            }
            else{
                int temp;
                while (chance[repptr] != 0){
                    chance[repptr++] = 0;
                    if (repptr == frame_cnt)
                        repptr = 0;
                }
                temp = frames[repptr];
                frames[repptr] = randoms[i];
                chance[repptr] = 1; // The latest page reference, hence it is set to 1
                // printf("The page replaced is %d", temp);
            }
            count++;
        }
        for (int k = 0; k < frame_cnt; k++){
            if(frames[k] != -1)
                printf("%d\t", frames[k]);
            else
                printf(" - \t");
        }
        printf("\n");
    }
    printf("\nThe number of page faults are %d\n", count);
    getchar();
}

// lfu print
void lfu_print(int frame_cnt, int frames[]){
    int j;
    for (j = 0; j < frame_cnt; j++){
            if(frames[j] != -1)
                printf("%d\t", frames[j]);
            else
                printf(" - \t");
    }
    printf("\n");
}

// lfu : 카운터 같으면 가장 앞에거
void lfu(){
    fprintf(stdout, "\n LFU\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    int i, j, k, frames[frame_cnt], move = 0, flag, faults = 0, count1[frame_cnt], repindex, leastcount;

    memset(count1, 0, sizeof(count1));

    for (i = 0; i < frame_cnt; i++){
        frames[i] = -1;
    }
    
    fprintf(stdout, "Incoming  \t");
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "cnt %d \t", i+1);
    }
    printf("\n");

    for (i = 0; i < ps_num; i++){
        printf("%d\t\t", randoms[i]);
        flag = 0;
        for (j = 0; j < frame_cnt; j++)
        {
            if (randoms[i] == frames[j])
            {
                flag = 1;
                count1[j]++;
                lfu_print(frame_cnt, frames);
                break;
            }
        }
        if (flag == 0 && faults < frame_cnt)
        {
            frames[move] = randoms[i];
            count1[move] = 1;
            move = (move + 1) % frame_cnt;
            faults++;
            lfu_print(frame_cnt, frames);
        }
        else if (flag == 0)
        {
            repindex = 0;
            leastcount = count1[0];
            for (j = 1; j < frame_cnt; j++)
            {
                if (count1[j] < leastcount)
                {
                    repindex = j;
                    leastcount = count1[j];
                }
            }

            frames[repindex] = randoms[i];
            count1[repindex] = 1;
            faults++;
            lfu_print(frame_cnt, frames);
        }
    }
    printf("Total Page Faults : %d\n", faults);
}

// lru 찾아줌
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

    int frames[frame_cnt], counter = 0, time[10], flag1, flag2, i, j, pos, faults = 0;

    for(i = 0; i < frame_cnt; i++){
        frames[i] = -1;
    }

    fprintf(stdout, "Incoming  \t");
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "cnt %d \t", i+1);
    }

    for(i = 0; i < ps_num; i++){
        flag1 = flag2 = 0;
        
        for(j = 0; j < frame_cnt; j++){
            if(frames[j] == randoms[i]){
                counter++;
                time[j] = counter;
                flag1 = flag2 = 1;
                break;
            }
        }
            
        if(flag1 == 0){
            for(j = 0; j < frame_cnt; j++){
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
            pos = findLRU(time, frame_cnt);
            counter++;
            faults++;
            frames[pos] = randoms[i];
            time[pos] = counter;
        }


        printf("\n");
        printf("%d\t\t", randoms[i]);
        for(j = 0; j < frame_cnt; j++){
            if(frames[j] != -1)
                printf("%d\t", frames[j]);
            else
                printf(" - \t");
        }
    }
    printf("\nTotal Page Faults = %d\n", faults);
}

// lifo
void lifo(){
    fprintf(stdout, "\n LIFO\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    int pageFaults = 0;
    int m, n, s;

    fprintf(stdout, "Incoming  \t");
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "cnt %d \t", i+1);
    }
    int temp[frame_cnt];
    for(m = 0; m < frame_cnt; m++){
        temp[m] = -1;
    }

    for(m = 0; m < ps_num; m++){
        s = 0;

        for(n = 0; n < frame_cnt; n++){
            if(randoms[m] == temp[n]){
                s++;
                pageFaults--;
            }
        }
        pageFaults++;
        
        if((pageFaults <= frame_cnt) && (s == 0)){
            temp[m] = randoms[m];
        }
        else if(s == 0){
            temp[frame_cnt - 1] = randoms[m];
        }

        printf("\n");
        printf("%d\t\t", randoms[m]);
        for(n = 0; n < frame_cnt; n++){
            if(temp[n] != -1)
                printf(" %d\t", temp[n]);
            else
                printf(" - \t");
        }
    }

    printf("\nTotal Page Faults : %d\n", pageFaults);
    return;
}

// optimal
void optimal(){
    fprintf(stdout, "\n Optimal\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    int frames[frame_cnt], temp[frame_cnt], flag1, flag2, flag3, i, j, k, pos, max, faults = 0;
    for(i = 0; i < frame_cnt; i++){
        frames[i] = -1;
    }

    fprintf(stdout, "Incoming  \t");
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "cnt %d \t", i+1);
    }
    for(i = 0; i < ps_num; i++){
        flag1 = flag2 = 0;
        
        for(j = 0; j < frame_cnt; j++){
            if(frames[j] == randoms[i]){
                flag1 = flag2 = 1;
                break;
            }
        }
        
        if(flag1 == 0){
            for(j = 0; j < frame_cnt; j++){
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
        
            for(j = 0; j < frame_cnt; j++){
                temp[j] = -1;
            
                for(k = i + 1; k < ps_num; ++k){
                    if(frames[j] == randoms[k]){
                        temp[j] = k;
                        break;
                    }
                }
            }
            
            for(j = 0; j < frame_cnt; j++){
                if(temp[j] == -1){
                    pos = j;
                    flag3 = 1;
                    break;
                }
            }
            
            if(flag3 == 0){
                max = temp[0];
                pos = 0;
            
                for(j = 1; j < frame_cnt; j++){
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
        for(j = 0; j < frame_cnt; j++){
            if(frames[j] != -1)
                printf("%d\t", frames[j]);
            else
                printf(" - \t");
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
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "cnt %d \t", i+1);
    }
    int temp[frame_cnt];
    for(m = 0; m < frame_cnt; m++){
        temp[m] = -1;
    }

    for(m = 0; m < ps_num; m++){
        s = 0;

        for(n = 0; n < frame_cnt; n++){
            if(randoms[m] == temp[n]){
                s++;
                pageFaults--;
            }
        }
        pageFaults++;
        
        if((pageFaults <= frame_cnt) && (s == 0)){
            temp[m] = randoms[m];
        }
        else if(s == 0){
            temp[(pageFaults - 1) % frame_cnt] = randoms[m];
        }

        printf("\n");
        printf("%d\t\t", randoms[m]);
        for(n = 0; n < frame_cnt; n++){
            if(temp[n] != -1)
                printf(" %d\t", temp[n]);
            else
                printf(" - \t");
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
        scanf("%d", &frame_cnt);   
        if(frame_cnt < 3 || frame_cnt > 10){
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
            randoms[i] = rand() % 6 + 1;
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
                lfu();
                optimal();
                break;
            case 6: // SC
                sc();
                optimal();
                break;
            case 7: // ESC
                esc();
                // optimal();
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
