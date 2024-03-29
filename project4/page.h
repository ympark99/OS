#ifndef PAGE_H_
#define PAGE_H_


#define BUF_SIZE 1024
#define RAND_MIN 500

void esc();
void sc();
void start();
void lfu_print(int frame_cnt, int frames[], int flag, FILE *fp);
void lfu();
int findLRU(int time[], int n);
void lru();
void lifo();
void optimal();
void fifo();
void start();


#endif
