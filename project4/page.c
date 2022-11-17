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
    FILE *fp = fopen("esc_output.txt", "w");
    fprintf(stdout, "\n Enhanced Second Chance\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");    
    int i, j, ref;
    int pointer = 0;
    int idx = 0;
    int done;
    int faults = 0;
    int temp; // pointer keep

    int frames[frame_cnt], reference_bit[frame_cnt], modify_bit[frame_cnt];

    memset(frames, -1, sizeof(frames));
    memset(reference_bit, -1, sizeof(reference_bit));
    memset(modify_bit, -1, sizeof(modify_bit));

    fprintf(stdout, "Incoming  \t");
    fprintf(fp, "Incoming  \t");
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "frame%d \t", i+1);
        fprintf(fp, "frame%d \t", i+1);
    }
    printf("\n");
    fprintf(fp, "\n");

    for(i = 0; i < ps_num; i++){
        bool isFault = false;

        printf("%d|1%d\t\t", randoms[i], rw[i]);
        fprintf(fp, "%d|1%d\t\t", randoms[i], rw[i]);
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
                fprintf(fp ,"%d|%d%d\t", frames[k], reference_bit[k], modify_bit[k]);
            }
            else{
                printf(" - \t");
                fprintf(fp, " - \t\t");
            }
        }
        if(isFault){
            fprintf(stdout, "page fault!");
            fprintf(fp, "page fault!");
        }
        printf("\n");
        fprintf(fp, "\n");
    }
    printf("Total Page Faults : %d\n", faults);
    fprintf(fp, "Total Page Faults : %d\n", faults);
    fprintf(stdout, "esc_output.txt에 시뮬레이션이 저장되었습니다.\n");
    fclose(fp);
}

// second chance : 참조비트 1로 만들고 그 비트부터 검사
void sc(){
    FILE *fp = fopen("sc_output.txt", "w");
    fprintf(stdout, "\n Second Chance\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    // counter
    int i, j, ref;
    int pointer = 0;
    int idx = 0;
    int faults = 0;
    // the "frames" to keep track of memory
    int frames[frame_cnt], reference_bit[frame_cnt];

    memset(frames, -1, sizeof(frames));
    memset(reference_bit, -1, sizeof(reference_bit));

    fprintf(stdout, "Incoming  \t");
    fprintf(fp, "Incoming  \t");
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "frame%d \t", i+1);
        fprintf(fp, "frame%d \t", i+1);
    }
    printf("\n");
    fprintf(fp, "\n");

    for(i = 0; i < ps_num; i++){
        bool isFault = false;
        printf("%d\t\t", randoms[i] );
        fprintf(fp, "%d\t\t\t", randoms[i]);
        ref = -1;
        // frame 탐색
        for(int j = 0; j < frame_cnt; j++){
            if( randoms[i] == frames[j] ){
                ref = j;
                break;
            }
        }

        if (ref < 0){ // 같은 frame 없을때
            if (idx < frame_cnt){ // 아직 프레임 못 채웠을때
                // printf("REPLACING %d WITH %d AT: %d\n", frames[i], randoms[i], idx);
                frames[idx] = randoms[i];
                reference_bit[idx] = 0; // 참조비트 0
                idx++;
                isFault = true;
                faults++;
            }
            else{
                for (j = 0; j < frame_cnt; j++){
                    // 참조비트 0인경우
                    if (reference_bit[pointer] == 0){
                        isFault = true;
                        faults++;
                        break;
                    }
                    else reference_bit[pointer] = 0; // 참조비트 1이면 0으로 변경
                    pointer = (pointer + 1) % (frame_cnt); // 다음 검사
                }

                frames[pointer] = randoms[i];
                reference_bit[pointer] = 0;
                pointer = (pointer + 1) % (frame_cnt); // 다음으로 이동
            }
        }
        else if (ref >= 0){ // page hit
            reference_bit[ref] = 1; // 참조비트 1로 설정
        }

        for (int k = 0; k < frame_cnt; k++){
            if(frames[k] != -1){
                printf("%d|%d\t", frames[k], reference_bit[k]);
                fprintf(fp, "%d|%d\t\t", frames[k], reference_bit[k]);
            }
            else{
                printf(" - \t");
                fprintf(fp, " - \t\t");
            }
        }
        if(isFault){
            fprintf(stdout, "page fault!");
            fprintf(fp, "page fault!");
        }
        printf("\n");
        fprintf(fp, "\n");
    }
    printf("Total Page Faults : %d\n", faults);
    fprintf(fp, "Total Page Faults : %d\n", faults);
    fprintf(stdout, "sc_output.txt에 시뮬레이션이 저장되었습니다.\n");
    fclose(fp);
}

// lfu print
void lfu_print(int frame_cnt, int frames[], int flag, FILE *fp){
    int j;
    for (j = 0; j < frame_cnt; j++){
            if(frames[j] != -1){
                printf("%d\t", frames[j]);
                fprintf(fp, " %d\t\t", frames[j]);
            }
            else{
                printf(" - \t");
                fprintf(fp, " - \t\t");
            }
    }
    if(flag == 0) fprintf(stdout, "page fault!");
    printf("\n");
    fprintf(fp, "\n");
}

// lfu : 카운터 같으면 가장 앞에거
void lfu(){
    FILE *fp = fopen("lfu_output.txt", "w");
    fprintf(stdout, "\n LFU\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    int i, j, k, frames[frame_cnt], move = 0, flag, faults = 0, count1[frame_cnt], repindex, leastcount;

    memset(count1, 0, sizeof(count1));

    for (i = 0; i < frame_cnt; i++){
        frames[i] = -1;
    }
    
    fprintf(stdout, "Incoming  \t");
    fprintf(fp, "Incoming  \t");
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "frame%d \t", i+1);
        fprintf(fp, "frame%d \t", i+1);
    }
    printf("\n");
    fprintf(fp, "\n");

    for (i = 0; i < ps_num; i++){
        printf("%d\t\t", randoms[i]);
        fprintf(fp, "%d\t\t\t", randoms[i]);
        flag = 0;
        for (j = 0; j < frame_cnt; j++){
            // page hit
            if (randoms[i] == frames[j]){
                flag = 1;
                count1[j]++;
                lfu_print(frame_cnt, frames, flag, fp);
                break;
            }
        }
        // 다 못채운경우
        if (flag == 0 && faults < frame_cnt){
            frames[move] = randoms[i];
            count1[move] = 1;
            move = (move + 1) % frame_cnt;
            faults++;
            lfu_print(frame_cnt, frames, flag, fp);
        }
        else if (flag == 0){
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
            lfu_print(frame_cnt, frames, flag, fp);
        }
    }
    printf("\nTotal Page Faults : %d\n", faults);
    fprintf(fp, "\nTotal Page Faults : %d\n", faults);
    fprintf(stdout, "lfu_output.txt에 시뮬레이션이 저장되었습니다.\n");
    fclose(fp);
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
    FILE *fp = fopen("lru_output.txt", "w");
    fprintf(stdout, "\n LRU\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    int frames[frame_cnt], counter = 0, time[10], flag1, flag2, i, j, pos, faults = 0;

    for(i = 0; i < frame_cnt; i++){
        frames[i] = -1;
    }

    fprintf(stdout, "Incoming  \t");
    fprintf(fp, "Incoming  \t");
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "frame%d \t", i+1);
        fprintf(fp, "frame%d \t", i+1);
    }

    for(i = 0; i < ps_num; i++){
        flag1 = flag2 = 0;
        // page hit
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
                // 아직 다 못채운 경우
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
        fprintf(fp, "\n");
        printf("%d\t\t", randoms[i]);
        fprintf(fp, "%d\t\t\t", randoms[i]);
        for(j = 0; j < frame_cnt; j++){
            if(frames[j] != -1){
                printf("%d\t", frames[j]);
                fprintf(fp, " %d\t\t", frames[j]);
            }
            else{
                printf(" - \t");
                fprintf(fp, " - \t\t");
            }
        }
        if(flag1 == 0 || flag2 == 0){
            fprintf(stdout, "page fault!");
            fprintf(fp, "page fault!");
        }
    }
    printf("\nTotal Page Faults : %d\n", faults);
    fprintf(fp, "\nTotal Page Faults : %d\n", faults);
    fprintf(stdout, "lru_output.txt에 시뮬레이션이 저장되었습니다.\n");
    fclose(fp);
}

// lifo
// todo : lifo 프레임 비워있을때 그대로?
void lifo(){
    FILE *fp = fopen("lifo_output.txt", "w");
    fprintf(stdout, "\n LIFO\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    int faults = 0;
    int m, n, flag_hit;

    fprintf(stdout, "Incoming  \t");
    fprintf(fp, "Incoming  \t");
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "frame%d \t", i+1);
        fprintf(fp, "frame%d \t", i+1);
    }
    int temp[frame_cnt];
    for(m = 0; m < frame_cnt; m++){
        temp[m] = -1;
    }

    for(m = 0; m < ps_num; m++){
        flag_hit = 0;

        for(n = 0; n < frame_cnt; n++){
            if(randoms[m] == temp[n]){
                flag_hit++;
                faults--;
            }
        }
        faults++;
        
        if((faults < frame_cnt) && (flag_hit == 0)){
            temp[m] = randoms[m];
        }
        else if(flag_hit == 0){
            temp[frame_cnt - 1] = randoms[m];
        }

        printf("\n");
        fprintf(fp, "\n");
        printf("%d\t\t", randoms[m]);
        fprintf(fp, "%d\t\t\t", randoms[m]);
        for(n = 0; n < frame_cnt; n++){
            if(temp[n] != -1){
                printf(" %d\t", temp[n]);
                fprintf(fp, " %d\t\t", temp[n]);
            }
            else{
                printf(" - \t");
                fprintf(fp, " - \t\t");
            }
        }
        if(flag_hit == 0){
            fprintf(stdout, "page fault!");
            fprintf(fp, "page fault!");
        }
    }

    printf("\nTotal Page Faults : %d\n", faults);
    fprintf(fp, "\nTotal Page Faults : %d\n", faults);
    fprintf(stdout, "lifo_output.txt에 시뮬레이션이 저장되었습니다.\n");
    fclose(fp);
    return;
}

// optimal
void optimal(){
    FILE *fp = fopen("optimal_output.txt", "w");
    fprintf(stdout, "\n Optimal\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    int frames[frame_cnt], temp[frame_cnt], flag1, flag2, flag3, i, j, k, pos, max, faults = 0;
    memset(frames, -1, sizeof(frames));

    fprintf(stdout, "Incoming  \t");
    fprintf(fp, "Incoming  \t");
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "frame%d \t", i+1);
        fprintf(fp, "frame%d \t", i+1);
    }
    for(i = 0; i < ps_num; i++){
        flag1 = flag2 = 0;
        // page hit
        for(j = 0; j < frame_cnt; j++){
            if(frames[j] == randoms[i]){
                flag1 = flag2 = 1;
                break;
            }
        }
        if(flag1 == 0){
            // 프레임 못채운 경우
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
            flag3 = 0;        
            for(j = 0; j < frame_cnt; j++){
                temp[j] = -1;
            
                for(k = i + 1; k < ps_num; k++){
                    if(frames[j] == randoms[k]){
                        temp[j] = k;
                        break;
                    }
                }
            }            
            for(j = 0; j < frame_cnt; j++){
                // 프레임 못채운 경우
                if(temp[j] == -1){
                    pos = j;
                    flag3 = 1;
                    break;
                }
            }
            // 가장 오랫동안 사용되지 않을 페이지 탐색
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
        fprintf(fp, "\n");
        printf("%d\t\t", randoms[i]);
        fprintf(fp, "%d\t\t\t", randoms[i]);
        for(j = 0; j < frame_cnt; j++){
            if(frames[j] != -1){
                printf("%d\t", frames[j]);
                fprintf(fp, " %d\t\t", frames[j]);
            }
            else{
                printf(" - \t");
                fprintf(fp, " - \t\t");
            }
        }
        if(flag1 == 0 || flag2 == 0){
            fprintf(stdout, "page fault!");
            fprintf(fp, "page fault!");
        }
    }
    
    printf("\nTotal Page Faults : %d\n", faults);
    fprintf(fp, "\nTotal Page Faults : %d\n", faults);
    fprintf(stdout, "optimal_output.txt에 시뮬레이션이 저장되었습니다.\n");
    fclose(fp);
}

// FIFO
void fifo(){
    FILE *fp = fopen("fifo_output.txt", "w");
    fprintf(stdout, "\n FIFO\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    int faults = 0;
    int m, n, flag_hit;

    fprintf(stdout, "Incoming  \t");
    fprintf(fp, "Incoming  \t");
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "frame%d \t", i+1);
        fprintf(fp, "frame%d \t", i+1);
    }
    int frames[frame_cnt];
    memset(frames, -1, sizeof(frames));

    for(m = 0; m < ps_num; m++){
        flag_hit = 0;

        for(n = 0; n < frame_cnt; n++){
            if(randoms[m] == frames[n]){
                flag_hit++;
                faults--;
            }
        }
        faults++;

        if((faults < frame_cnt) && (flag_hit == 0)){
            frames[m] = randoms[m];
        }
        else if(flag_hit == 0){
            frames[(faults - 1) % frame_cnt] = randoms[m];
        }

        printf("\n");
        fprintf(fp, "\n");
        printf("%d\t\t", randoms[m]);
        fprintf(fp, "%d\t\t\t", randoms[m]);
        for(n = 0; n < frame_cnt; n++){
            if(frames[n] != -1){
                printf(" %d\t", frames[n]);
                fprintf(fp, " %d\t\t", frames[n]);
            }
            else{
                printf(" - \t");
                fprintf(fp, " - \t\t");
            }
        }
        if(flag_hit == 0){
            fprintf(stdout, "page fault!");
            fprintf(fp, "page fault!");
        }
    }

    printf("\nTotal Page Faults : %d\n", faults);
    fprintf(fp, "\nTotal Page Faults : %d\n", faults);
    fprintf(stdout, "fifo_output.txt에 시뮬레이션이 저장되었습니다.\n");
    fclose(fp);
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
        // 랜덤으로 스트림 생성
        for(int i = 0; i < RAND_MIN; i++){
            randoms[i] = rand() % 6 + 1;
            rw[i] = rand() % 2;

            if(rw[i] == 0)
                fprintf(stdout, "%d: %d R\n", i+1, randoms[i]);
            else fprintf(stdout, "%d: %d W\n", i+1, randoms[i]);
        }
        fprintf(stdout, "--------------\n");
    }
    // 2 선택시 파일 입력 받기
    else{
        fprintf(stdout, "현재 위치 내 생성할 파일 이름 입력 - 형식(.txt) 포함, 스트림은 랜덤으로 생성됩니다.\n");
        fprintf(stdout, ">> ");
        getchar(); // 입력 버퍼 비우기(\n)
        fgets(filename, BUF_SIZE, stdin); // 명령어 입력

        FILE *fp = fopen(filename, "w");
        // 파일에 랜덤으로 생성
        for (int i = 0; i < ps_num; i++){
            fprintf(fp, "%d ", rand() % 6 + 1);
            fprintf(fp, "%d", rand() % 2);
            fprintf(fp, "\n");
        }
        fclose(fp);
        fp = fopen(filename, "rt"); // 읽기 모드로 open
        for(int i = 0; i < ps_num; i++){
            fscanf(fp, "%d %d", &randoms[i], &rw[i]);
        }

        fclose(fp);
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
                optimal();
                break;
            case 8: // ALL
                fifo();
                lifo();
                lru();
                lfu();
                sc();
                esc();
                optimal();
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
