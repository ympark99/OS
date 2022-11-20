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

//todo : ps_num 500, rand 30, RAND_MIN 500

void esc(){
    FILE *fp = fopen("esc_output.txt", "w");
    fprintf(stdout, "\n Enhanced Second Chance\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");    
    int i, j, ref;
    int pointer = 0;
    int idx = 0;
    int done;
    int faults = 0;
    int temp; // 포인터 저장

    int frames[frame_cnt], reference_bit[frame_cnt], modify_bit[frame_cnt];

    memset(frames, -1, sizeof(frames));
    memset(reference_bit, -1, sizeof(reference_bit));
    memset(modify_bit, -1, sizeof(modify_bit));

    fprintf(stdout, "Incoming  \t");
    fprintf(fp, "Incoming  \t");
    for(i = 0; i < frame_cnt; i++){
        fprintf(stdout, "frame%d \t", i+1);
        fprintf(fp, "frame%d \t", i+1);
    }
    fprintf(stdout, "\n");
    fprintf(fp, "\n");

    for(i = 0; i < ps_num; i++){
        bool isFault = false;

        fprintf(stdout, "%d|1%d\t\t", randoms[i], rw[i]);
        fprintf(fp, "%d|1%d\t\t", randoms[i], rw[i]);
        ref = -1;
        // frame 탐색
        for(int j = 0; j < frame_cnt; j++){
            if( randoms[i] == frames[j] ){
                ref = j;
                break;
            }
        }

        if(ref < 0){ // page fault 발생
            isFault = true;
            if (idx < frame_cnt){ // 아직 프레임 못 채웠을때
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
                for(j = 0; j < frame_cnt; j++){
                    // 00 1순위
                    if (reference_bit[pointer] == 0 && modify_bit[pointer] == 0){
                        done = 1;
                        break;
                    }
                    pointer = (pointer + 1) % (frame_cnt);
                }
                // 01 검사
                if(done == 0){
                    pointer = temp;
                    for (j = 0; j < frame_cnt; j++){
                        if (reference_bit[pointer] == 0 && modify_bit[pointer] == 1){
                            done = 1;
                            break;
                        }
                        pointer = (pointer + 1) % (frame_cnt);
                    }
                }
                // 00 다시 검사
                if(done == 0){
                    pointer = temp;
                    for (j = 0; j < frame_cnt; j++){
                        if (reference_bit[pointer] == 0 && modify_bit[pointer] == 0){
                            done = 1;
                            break;
                        }
                        pointer = (pointer + 1) % (frame_cnt);
                    }
                }
                // 01 다시 검사
                if(done == 0){
                    pointer = temp;
                    for (j = 0; j < frame_cnt; j++){
                        if (reference_bit[pointer] == 0 && modify_bit[pointer] == 1){
                            break;
                        }
                        pointer = (pointer + 1) % (frame_cnt);
                    }
                }
                frames[pointer] = randoms[i];
                reference_bit[pointer] = 1; // R bit 1로 고정
                modify_bit[pointer] = rw[i];
            }
            faults++;
        }
        else if(ref >= 0){ // page hit
            reference_bit[ref] = 1; // R bit 1로 고정
            if (rw[i] == 1)
                modify_bit[ref] = 1;
            else modify_bit[ref] = 0;
        }
        // 시뮬레이션 결과 출력
        for(int k = 0; k < frame_cnt; k++){
            if(frames[k] != -1){
                fprintf(stdout, "%d|%d%d\t", frames[k], reference_bit[k], modify_bit[k]);
                fprintf(fp ,"%d|%d%d\t", frames[k], reference_bit[k], modify_bit[k]);
            }
            else{
                fprintf(stdout, " - \t");
                fprintf(fp, " - \t\t");
            }
        }
        if(isFault){
            fprintf(stdout, "page fault!");
            fprintf(fp, "page fault!");
        }
        fprintf(stdout, "\n");
        fprintf(fp, "\n");
    }
    fprintf(stdout, "Total Page Faults : %d\n", faults);
    fprintf(fp, "Total Page Faults : %d\n", faults);
    fprintf(stdout, "esc_output.txt에 시뮬레이션이 저장되었습니다.\n");
    fclose(fp);
}

// second chance : 참조비트 1로 만들고 그 비트부터 검사
void sc(){
    FILE *fp = fopen("sc_output.txt", "w");
    fprintf(stdout, "\n Second Chance\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    int i, j, ref;
    int pointer = 0;
    int idx = 0;
    int faults = 0;
    int frames[frame_cnt], reference_bit[frame_cnt];

    memset(frames, -1, sizeof(frames));
    memset(reference_bit, -1, sizeof(reference_bit));

    fprintf(stdout, "Incoming  \t");
    fprintf(fp, "Incoming  \t");
    for(i = 0; i < frame_cnt; i++){
        fprintf(stdout, "frame%d \t", i+1);
        fprintf(fp, "frame%d \t", i+1);
    }
    fprintf(stdout, "\n");
    fprintf(fp, "\n");

    for(i = 0; i < ps_num; i++){
        bool isFault = false;
        fprintf(stdout, "%d\t\t", randoms[i] );
        fprintf(fp, "%d\t\t\t", randoms[i]);
        ref = -1;
        // frame 탐색
        for(int j = 0; j < frame_cnt; j++){
            if( randoms[i] == frames[j] ){
                ref = j;
                break;
            }
        }

        if(ref < 0){ // 같은 frame 없을때
            if (idx < frame_cnt){ // 아직 프레임 못 채웠을때
                frames[idx] = randoms[i];
                reference_bit[idx] = 0; // 참조비트 0
                idx++;
                isFault = true;
                faults++;
                pointer = (pointer + 1) % (frame_cnt); // 다음으로 이동
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
        else if(ref >= 0){ // page hit
            reference_bit[ref] = 1; // 참조비트 1로 설정
        }

        for(int k = 0; k < frame_cnt; k++){
            if(frames[k] != -1){
                fprintf(stdout, "%d|%d\t", frames[k], reference_bit[k]);
                fprintf(fp, "%d|%d\t\t", frames[k], reference_bit[k]);
            }
            else{
                fprintf(stdout, " - \t");
                fprintf(fp, " - \t\t");
            }
        }
        if(isFault){
            fprintf(stdout, "page fault!");
            fprintf(fp, "page fault!");
        }
        fprintf(stdout, "\n");
        fprintf(fp, "\n");
    }
    fprintf(stdout, "Total Page Faults : %d\n", faults);
    fprintf(fp, "Total Page Faults : %d\n", faults);
    fprintf(stdout, "sc_output.txt에 시뮬레이션이 저장되었습니다.\n");
    fclose(fp);
}

// lfu print
void lfu_print(int frame_cnt, int frames[], int flag, FILE *fp){
    int j;
    for (j = 0; j < frame_cnt; j++){
            if(frames[j] != -1){
                fprintf(stdout, "%d\t", frames[j]);
                fprintf(fp, " %d\t\t", frames[j]);
            }
            else{
                fprintf(stdout, " - \t");
                fprintf(fp, " - \t\t");
            }
    }
    if(flag == 0) fprintf(stdout, "page fault!");
    fprintf(stdout, "\n");
    fprintf(fp, "\n");
}

// lfu : 카운터 같으면 가장 앞에거
void lfu(){
    FILE *fp = fopen("lfu_output.txt", "w");
    fprintf(stdout, "\n LFU\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    // freq : 참조 횟수 저장 배열
    int i, j, k, frames[frame_cnt], pointer = 0, flag, faults = 0, freq[frame_cnt], idx, min_cnt;

    memset(freq, 0, sizeof(freq));
    memset(frames, -1, sizeof(frames));
    
    fprintf(stdout, "Incoming  \t");
    fprintf(fp, "Incoming  \t");
    for(i = 0; i < frame_cnt; i++){
        fprintf(stdout, "frame%d \t", i+1);
        fprintf(fp, "frame%d \t", i+1);
    }
    fprintf(stdout, "\n");
    fprintf(fp, "\n");

    for (i = 0; i < ps_num; i++){
        fprintf(stdout, "%d\t\t", randoms[i]);
        fprintf(fp, "%d\t\t\t", randoms[i]);
        flag = 0;
        // 프레임을 순환하며 현재 들어온 페이지 스트링과 같은 페이지 스트링이 있는지 탐색(page hit)
        for (j = 0; j < frame_cnt; j++){
            if (randoms[i] == frames[j]){
                flag = 1;
                freq[j]++;
                lfu_print(frame_cnt, frames, flag, fp); // 시뮬레이션 정보 출력 & 파일 정보 저장
                break;
            }
        }
        // 아직 한 번도 페이지 스트링이 들어오지 않은 프레임이 있는지 탐색
        if (flag == 0 && faults < frame_cnt){
            frames[pointer] = randoms[i]; // 페이지 교체
            freq[pointer] = 1; // 처음 참조하므로 빈도 1
            pointer = (pointer + 1) % frame_cnt; // 포인터 이동
            faults++;
            lfu_print(frame_cnt, frames, flag, fp);
        }
        // page fault
        else if (flag == 0){
            idx = 0;
            min_cnt = freq[0];
            for (j = 1; j < frame_cnt; j++){
                // 가장 적게 사용한 것 탐색
                if(freq[j] < min_cnt){
                    idx = j; // 인덱스 갱신
                    min_cnt = freq[j]; // 최소 참조 횟수 갱신
                }
            }
            frames[idx] = randoms[i];
            freq[idx] = 1; // 참조횟수 1로 리셋
            faults++;
            lfu_print(frame_cnt, frames, flag, fp);
        }
    }
    fprintf(stdout, "\nTotal Page Faults : %d\n", faults);
    fprintf(fp, "\nTotal Page Faults : %d\n", faults);
    fprintf(stdout, "lfu_output.txt에 시뮬레이션이 저장되었습니다.\n");
    fclose(fp);
}

// lru 찾아줌
int findLRU(int recent[], int n){
    int i, min_num = recent[0], pos = 0;
    // 프레임 개수만큼 반복
    for(i = 1; i < n; i++){
        if(recent[i] < min_num){ // 더 오래된 시간이면
            min_num = recent[i]; // 가장 작은 값(오래된 시간) 갱신
            pos = i; // 해당 인덱스 갱신
        }
    }
    return pos;
}

// lru
void lru(){
    FILE *fp = fopen("lru_output.txt", "w");
    fprintf(stdout, "\n LRU\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    // recent : 가장 최근에 사용했던 시간을 저장할 배열
    // time_cnt : 시간 카운터
    int frames[frame_cnt], time_cnt = 0, recent[frame_cnt], flag1, flag2, i, j, pos, faults = 0;

    memset(frames, -1, sizeof(frames));
    memset(frames, -1, sizeof(recent));

    fprintf(stdout, "Incoming  \t");
    fprintf(fp, "Incoming  \t");
    for(int i = 0; i < frame_cnt; i++){
        fprintf(stdout, "frame%d \t", i+1);
        fprintf(fp, "frame%d \t", i+1);
    }

    for(i = 0; i < ps_num; i++){
        flag1 = flag2 = 0;
        // 프레임을 순환하며 현재 들어온 페이지 스트링과 같은 페이지 스트링이 있는지 탐색
        for(j = 0; j < frame_cnt; j++){
            if(frames[j] == randoms[i]){
                time_cnt++; // 시간 카운터 증가
                recent[j] = time_cnt; // 가장 최근 시간 갱신
                flag1 = flag2 = 1;
                break;
            }
        }
            
        if(flag1 == 0){
            // 아직 한 번도 페이지 스트링이 들어오지 않은 프레임이 있는지 탐색
            for(j = 0; j < frame_cnt; j++){
                // 아직 다 못채운 경우 && page fault
                if(frames[j] == -1){
                    time_cnt++; // 시간 카운터 증가
                    faults++;
                    frames[j] = randoms[i];
                    recent[j] = time_cnt; // 시간 갱신
                    flag2 = 1;
                    break;
                }
            }
        }
        // page fault
        if(flag2 == 0){
            pos = findLRU(recent, frame_cnt); // 가장 오랫동안 사용하지 않았던 페이지 찾기
            time_cnt++;
            faults++;
            frames[pos] = randoms[i]; // 페이지 교체
            recent[pos] = time_cnt; // 시간 갱신
        }

        fprintf(stdout, "\n");
        fprintf(fp, "\n");
        fprintf(stdout, "%d\t\t", randoms[i]);
        fprintf(fp, "%d\t\t\t", randoms[i]);
        for(j = 0; j < frame_cnt; j++){
            if(frames[j] != -1){
                fprintf(stdout, "%d\t", frames[j]);
                fprintf(fp, " %d\t\t", frames[j]);
            }
            else{
                fprintf(stdout, " - \t");
                fprintf(fp, " - \t\t");
            }
        }
        if(flag1 == 0 || flag2 == 0){
            fprintf(stdout, "page fault!");
            fprintf(fp, "page fault!");
        }
    }
    fprintf(stdout, "\nTotal Page Faults : %d\n", faults);
    fprintf(fp, "\nTotal Page Faults : %d\n", faults);
    fprintf(stdout, "lru_output.txt에 시뮬레이션이 저장되었습니다.\n");
    fclose(fp);
}

// lifo
void lifo(){
    FILE *fp = fopen("lifo_output.txt", "w");
    fprintf(stdout, "\n LIFO\n");
    fprintf(stdout, "-----------------------------------------------------------------------------\n");

    int faults = 0;
    int i, j, flag_hit;

    fprintf(stdout, "Incoming  \t");
    fprintf(fp, "Incoming  \t");
    for(i = 0; i < frame_cnt; i++){
        fprintf(stdout, "frame%d \t", i+1);
        fprintf(fp, "frame%d \t", i+1);
    }
    int frames[frame_cnt];
    memset(frames, -1, sizeof(frames));

    for(i = 0; i < ps_num; i++){
        flag_hit = 0;

        for(j = 0; j < frame_cnt; j++){
            // page hit시 faults 그대로
            if(randoms[i] == frames[j]){
                flag_hit++;
                faults--;
            }
        }
        faults++;
        
        // 아직 다 못채운경우
        if((faults < frame_cnt) && (flag_hit == 0)){
            // (페이지 카운터 -1 % 프레임 개수)를 하면 0부터 프레임 개수-1까지 
            // page fault 카운터가 증가할 때마다 순차적으로 다음 인덱스를 가리키게 된다. (즉, 메모리에 올라온 순서대로 교체)
            frames[(faults - 1) % frame_cnt] = randoms[i];
        }
        // page fault
        else if(flag_hit == 0){
            frames[frame_cnt - 1] = randoms[i]; // 맨 마지막 교체
        }

        fprintf(stdout, "\n");
        fprintf(fp, "\n");
        fprintf(stdout, "%d\t\t", randoms[i]);
        fprintf(fp, "%d\t\t\t", randoms[i]);
        for(j = 0; j < frame_cnt; j++){
            if(frames[j] != -1){
                fprintf(stdout, " %d\t", frames[j]);
                fprintf(fp, " %d\t\t", frames[j]);
            }
            else{
                fprintf(stdout, " - \t");
                fprintf(fp, " - \t\t");
            }
        }
        if(flag_hit == 0){
            fprintf(stdout, "page fault!");
            fprintf(fp, "page fault!");
        }
    }

    fprintf(stdout, "\nTotal Page Faults : %d\n", faults);
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

    int frames[frame_cnt], temp[frame_cnt], flag1, flag2, flag3, i, j, k, pos, max;
    int faults = 0;
    memset(frames, -1, sizeof(frames));

    fprintf(stdout, "Incoming  \t");
    fprintf(fp, "Incoming  \t");
    for(i = 0; i < frame_cnt; i++){
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
                // 아직 한 번도 페이지 스트링이 들어오지 않은 프레임이 있는지 탐색
                if(frames[j] == -1){
                    // page fault 카운터를 1증가시키고 채워넣음, 다음 단계 실행 x
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
                temp[j] = -1; // 해당 프레임 내 페이지 스트링 다음에 언제 사용하는지 인덱스 저장
            
                for(k = i + 1; k < ps_num; k++){
                    // 앞으로 올 페이지 스트링에서 다음에 언제 사용하는지 인덱스 저장
                    if(frames[j] == randoms[k]){
                        temp[j] = k;
                        break;
                    }
                }
            }            
            for(j = 0; j < frame_cnt; j++){
                // 앞으로 사용할 페이지가 없는 경우 교체할 페이지 확정
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
                        max = temp[j]; // 가장 큰 값(인덱스)이 오랫동안 사용하지 않을 페이지
                        pos = j;
                    }
                }            
            }
            frames[pos] = randoms[i]; // 교체
            faults++;
        }
        
        fprintf(stdout, "\n");
        fprintf(fp, "\n");
        fprintf(stdout, "%d\t\t", randoms[i]);
        fprintf(fp, "%d\t\t\t", randoms[i]);
        for(j = 0; j < frame_cnt; j++){
            if(frames[j] != -1){
                fprintf(stdout, "%d\t", frames[j]);
                fprintf(fp, " %d\t\t", frames[j]);
            }
            else{
                fprintf(stdout, " - \t");
                fprintf(fp, " - \t\t");
            }
        }
        if(flag1 == 0 || flag2 == 0){
            fprintf(stdout, "page fault!");
            fprintf(fp, "page fault!");
        }
    }
    
    fprintf(stdout, "\nTotal Page Faults : %d\n", faults);
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
    int i, j, flag_hit;

    fprintf(stdout, "Incoming  \t");
    fprintf(fp, "Incoming  \t");
    for(i = 0; i < frame_cnt; i++){
        fprintf(stdout, "frame%d \t", i+1);
        fprintf(fp, "frame%d \t", i+1);
    }
    int frames[frame_cnt];
    memset(frames, -1, sizeof(frames));

    for(i = 0; i < ps_num; i++){
        flag_hit = 0;

        for(j = 0; j < frame_cnt; j++){
            // 프레임을 순환하며 현재 들어온 페이지 스트링과 같은 페이지 스트링이 있는지 탐색(page hit)
            if(randoms[i] == frames[j]){
                flag_hit++;
                faults--;
            }
        }
        faults++;
        // page fault
        if(flag_hit == 0){
            // (페이지 폴트 카운터 -1 % 프레임 개수)를 하면 0부터 프레임 개수-1까지 page fault 카운터가 증가할 때마다 
            // 순차적으로 다음 인덱스를 가리키게 된다. (즉, 메모리에 올라온 순서대로 교체)
            frames[(faults - 1) % frame_cnt] = randoms[i];
        }
        // 파일 및 표준 출력
        fprintf(stdout, "\n");
        fprintf(fp, "\n");
        fprintf(stdout, "%d\t\t", randoms[i]);
        fprintf(fp, "%d\t\t\t", randoms[i]);
        for(j = 0; j < frame_cnt; j++){
            if(frames[j] != -1){
                fprintf(stdout, " %d\t", frames[j]);
                fprintf(fp, " %d\t\t", frames[j]);
            }
            else{
                fprintf(stdout, " - \t");
                fprintf(fp, " - \t\t");
            }
        }
        if(flag_hit == 0){
            fprintf(stdout, "page fault!");
            fprintf(fp, "page fault!");
        }
    }

    fprintf(stdout, "\nTotal Page Faults : %d\n", faults);
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
