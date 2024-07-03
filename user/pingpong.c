#include "kernel/types.h"
#include "user/user.h"
//题目要求使用两个管道
int main(int argc,char* argv[]){
    if(argc !=1){
        printf("Error!");
        exit(-1);
    }

    int fd_c2p[2];//子写父读
    int fd_p2c[2];//父写子读
    pipe(fd_c2p);
    pipe(fd_p2c);
    int pid=fork();
    //使用 fork() 创建一个新进程时，这个调用会返回两次：一次在父进程中，一次在子进程中
    if (pid<0)
    {
        printf("fork error!");
        exit(-1);
    }
    //表明处在子进程中
    else if(pid==0){
        close(fd_c2p[0]);//关闭读端
        close(fd_p2c[1]);//关闭写端
        //read()系统调用，读取管道，第一个参数为管道读端标识符
        read(fd_p2c[0],"p",1);
        printf("%d: received ping\n",getpid());
        //write()系统调用，写入管道，第一个参数为管道写端标识符
        write(fd_c2p[1],"p",1);
        close(fd_c2p[1]);//关闭写端
        close(fd_p2c[0]);//关闭读端
        exit(0);
    }
    //表明处在父进程中
    else{
        close(fd_c2p[1]);//关闭写端
        close(fd_p2c[0]);//关闭读端
        read(fd_c2p[0],"p",1);
        printf("%d: received pong\n",getpid());
        write(fd_p2c[1],"p",1);
        close(fd_c2p[0]);//关闭读端
        close(fd_p2c[1]);//关闭写端
        exit(0);
    }
}
//注意：对于一个管道来说，父子进程各自拥有管道的读写端文件描述符，并且它们的文件描述符是独立的。因此，关闭或打开一个进程自己的管道端口不会直接影响到另一个进程对同一管道的访问。
// #include "kernel/types.h"
// #include "user/user.h"

// int main(int argc, char* argv[]) {
//     // 检查参数
//     if (argc != 1) {
//         printf("Error: Incorrect number of arguments\n");
//         exit(-1);
//     }

//     int fd_c2p[2];  // 子进程写，父进程读
//     int fd_p2c[2];  // 父进程写，子进程读

//     //创建两个管道
//     if (pipe(fd_c2p) < 0 || pipe(fd_p2c) < 0) {
//         printf("Error: Unable to create pipes\n");
//         exit(-1);
//     }

//     int pid = fork();
//     if (pid < 0) {
//         printf("Error: Fork failed\n");
//         exit(-1);
//     }
//     else if (pid == 0) {  // 子进程
//         close(fd_c2p[0]);  // 关闭子进程读取父进程的管道
//         close(fd_p2c[1]);  // 关闭子进程写入父进程的管道

        
//         read(fd_p2c[0], "p", 1);
//         printf("%d: received ping\n", getpid());

//         write(fd_c2p[1], "p", 1);

//         close(fd_c2p[1]);
//         close(fd_p2c[0]);
//         exit(0);
//     }
//     else {  // 父进程
//         close(fd_c2p[1]);  // 关闭父进程写入子进程的管道
//         close(fd_p2c[0]);  // 关闭父进程读取子进程的管道

      

      
//         read(fd_c2p[0], "p", 1);
//         printf("%d: received pong\n", getpid());
//         write(fd_p2c[1], "p", 1);
//         close(fd_c2p[0]);
//         close(fd_p2c[1]);
//         exit(0);
//     }

   
// }
