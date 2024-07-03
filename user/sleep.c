#include "kernel/types.h"
#include "user/user.h"

int main(int argc,char* argv[]){
    if(argc !=2){
        printf("Error:sleep 2");
        exit(-1);
    }

    int num=atoi(argv[1]);
    if(sleep(num)<0){
        printf("can not sleep");
        exit(-1);
    }
    exit(0);
}

