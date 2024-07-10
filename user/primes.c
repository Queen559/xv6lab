// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"

// //主函数前需要一个递归函数
// //问题1递归函数的传入的参数是什么?
// //递归逻辑应该是打印读取的上一个管道的第一个数，然后把其余不是第一个数的倍数写入下一个管道，
// void backtrack(int *p){
// //关闭上一个进程的写入端
//  close(p[1]);
//  int num;
//  //这里的read函数是一个一个字节的读，所以读到第一个字节能打印
//  //递归终止条件，即读不到东西,
//  if(read(p[0],&num,sizeof(num))==0){
//     close(p[0]);
//     exit(0);
//  }
// //肯定是打印上一个管道的第一个数
// printf("primes %d\n",num);
// //再需要创建一个管道
// int fp[2];
// pipe(fp);
// //close(fp[0]);//关闭读端
// int nnum;
// while(read(p[0],&nnum,sizeof(nnum))){
//     if(nnum % num !=0){
//         write(fp[1],&nnum,sizeof(nnum));
//     }
    

// }
// close(fp[1]); // 关闭新管道的写入端
//     close(p[0]);  // 关闭当前管道的读取端

//     // 递归调用自己
//     backtrack(fp);

//     close(fp[0]); // 关闭新管道的读取端
//     exit(0);

// }


// //筛选素数


// int main(int argc, char* argv[]) {
//     // 检查参数
//     if (argc != 1) {
//         printf("Error: Incorrect number of arguments\n");
//         exit(-1);
//     }

//     int fd[2];  
    
//     //创建一个管道
//     if (pipe(fd) < 0 ) {
//         printf("Error: Unable to create pipes\n");
//         exit(-1);
//     }

//     int pid = fork();
//     if (pid < 0) {
//         printf("Error: Fork failed\n");
//         exit(-1);
//     }
//     else if (pid == 0) {  // 子进程
//     //关闭写入端别忘了
//        close(fd[1]);
//        backtrack(fd);
//     }
//     else {  // 父进程，将2-n这几个数写进去
//         close(fd[0]);  // 关闭父进程读取子进程的管道
//         //这个n改怎么写?,直接到35
//         for(int i=2;i<=35;i++)
//         {
//             write(fd[1], "&i", sizeof(i));
//         }
//         close(fd[1]);
//         wait(0);  // 等待子进程结束
       
//     }
//  exit(0);
   
// }
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void backtrack(int *p){
    //关闭上一个进程的写入端
    close(p[1]);
    int num;
    //递归终止条件，即读不到东西
    if(read(p[0], &num, sizeof(num)) == 0){
        close(p[0]);
        exit(0);
    }
    //打印上一个管道的第一个数
    printf("prime %d\n", num);
    //再需要创建一个管道
    int fp[2];
    pipe(fp);
    //重点：递归每次要创造一个子进程，在子进程里面递归
    if(fork() == 0) {
        backtrack(fp);
    } else {
        close(fp[0]);  // 关闭写端
        int nnum;
        while(read(p[0], &nnum, sizeof(nnum))){
            if(nnum % num != 0){
                write(fp[1], &nnum, sizeof(nnum));
            }
        }
        close(fp[1]);
        close(p[0]);
        wait(0);  // 等待子进程结束
        exit(0);
    }
}

//筛选素数
int main(int argc, char* argv[]) {
    // 检查参数
    if (argc != 1) {
        printf("Error: Incorrect number of arguments\n");
        exit(-1);
    }

    int fd[2];

    //创建一个管道
    if (pipe(fd) < 0) {
        printf("Error: Unable to create pipes\n");
        exit(-1);
    }

    int pid = fork();
    if (pid < 0) {
        printf("Error: Fork failed\n");
        exit(-1);
    } else if (pid == 0) {  // 子进程
        close(fd[1]);
        backtrack(fd);
    } else {  // 父进程，将2到35的数写入管道
        close(fd[0]);  // 关闭父进程读取子进程的管道
        for(int i = 2; i <= 35; i++) {
            write(fd[1], &i, sizeof(i));
        }
        close(fd[1]);
        wait(0);  // 等待子进程结束
    }

    exit(0);
}
