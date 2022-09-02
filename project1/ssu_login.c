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
        for(int j = 0; i < 32; j++){
            if((n = read(fd, &buf, 1)) > 0){
                if(!strcmp(&buf, " ")){
                    break;
                }
                userID[i][j] = buf;
            }
            else break;
        }

        for(int j = 0; i < 32; j++){
            if((n = read(fd, &buf, 1)) > 0){
                if(!strcmp(&buf, "\n")){
                    break;
                }                
                pwdID[i][j] = buf;
            }
            else break;
        }
    }
}

int check_idpw(char* user, char* pw){
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
    user[strlen(user) - 1] = '\0';
    printf(1, "Password: ");
    gets(pw, 32);
    pw[strlen(pw) - 1] = '\0';

    get_user_list();

    if(check_idpw(user, pw) == 1)
        exec("sh", argv);

    return 0;
}
