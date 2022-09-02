#include "types.h"
#include "stat.h"
#include "user.h"

int lines; // 출력할 행 수
char buf[512];

void 
cat(int fd)
{
    int n;
    int cnt = 0;
    while((n = read(fd, buf, 1)) > 0){
        if(!strcmp(buf, "\n")){
            cnt++;
        }

        if(write(1, buf, n) != n){
            printf(1, "cat: write error\n");
            exit();
        }

        if(cnt == lines) break;
    }
    if(n < 0){
        printf(1, "cat: read error\n");
        exit();
    }
}

int 
main(int argc, char *argv[])
{
    int fd, i;

    if(argc <= 1){
        // cat(0);
        exit();
    }

    lines = atoi(argv[1]);

    for(i = 2; i < argc; i++){
        if((fd = open(argv[i], 0)) < 0){
            printf(1, "cat : cannot open %s\n", argv[i]);
            exit();
        }
        cat(fd);
        close(fd);
    }
    exit();
}
