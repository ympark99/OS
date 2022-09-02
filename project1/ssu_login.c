#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char userID[16][32];
char pwdID[16][32];

char buf;
int cnt;

void get_user_list(){
    int fd;
    int i, n;

    fd = open("list.txt", O_RDONLY);

    for(i = 0; i < 16; i++){
        int flag = 0; // 0 : username, 1 : password
        for(int j = 0; i < 32; j++){
            if((n = read(fd, &buf, 1)) > 0){
                if(!strcmp(&buf, " ")){
                    flag = 1;
                    continue;;
                }
                else if(!strcmp(&buf, "\n")){
                    break;
                }
                
                if(!flag)
                    userID[i][j] = buf;
                    // strcpy(userID[i][j], buf);
                else if(flag == 1)
                    userID[i][j] = buf;
                    // strcpy(pwdID[i][j], buf);
            }
            else break;
        }
    }
}

int check_idpw(char user[32], char pw[32]){
    int i;
    for(i = 0; i < 16; i++){
        if((!strcmp(userID[i], user)) && (!strcmp(pwdID[i], pw))){
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    char user[32];
    char pw[32];

    printf(1, "Username: ");
    gets(user, 32);
    printf(1, "Password: ");
    gets(pw, 32);

    get_user_list();

    if(check_idpw(user, pw) == 1)
        exec("sh", argv);

    return 0;
}
