#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "kernel/fs.h"

// //echo hello too | xargs echo bye
// echo "1\n2" | xargs echo line
int
main(int argc, char *argv[])
{

//先获取到自己的命令行参数
char *xargv[MAXARG];
for (int i = 1; i < argc; i++)
{
  xargv[i-1]=argv[i];
}
//将标准输入拼接到自己的命令行参数中
char buf[512];
int i;
int len;
while(1)
{
    i=0;
    //读一行
    while(1){
        len=read(0,&buf[i],1);
        if(len==0 || buf[i]=='\n') break;
        i++;
    }
    if(i==0) break;
    //C是一个非常底层(接近机器语言)的编程语言，并没有一个方法来确定一个数组究竟有多长。所以为了告诉内核数组的结尾在哪，我们将0作为最后一个指针
    buf[i]=0;//终止字符串
    xargv[argc-1]=buf;//拼接
    xargv[argc] = 0;  // 确保参数数组以 NULL 结尾
    if(fork()==0){
        exec(xargv[0],xargv);//xargv[0]类似于echo,cat这种,操作系统从名为echo的文件中加载指令到当前的进程中，并替换了当前进程的内存，之后开始执行这些新加载的指令
        exit(0);
    }
    else{
        wait(0);//等待子进程结束
    }
}
  
    exit(0);//0表示程序正常退出。
}

// int main(int argc, char *argv[])
// {
//   char buf[512];
//   char* full_argv[MAXARG];
//   int i;
//   int len;
//   if(argc < 2){
//     fprintf(2, "usage: xargs your_command\n");
//     exit(1);
//   }
//   // we need an extra arg and a terminating zero
//   // and we don't need the first argument xargs
//   // so in total, we need one extra space than the origin argc
//   if (argc + 1 > MAXARG) {
//       fprintf(2, "too many args\n");
//       exit(1);
//   }
//   // copy the original args
//   // skip the first argument xargs
//   for (i = 1; i < argc; i++) {
//       full_argv[i-1] = argv[i];
//   }
//   // full_argv[argc-1] is the extra arg to be filled
//   // full_argv[argc] is the terminating zero
//   full_argv[argc] = 0;
//   while (1) {
//       i = 0;
//       // read a line
//       while (1) {
//         len = read(0,&buf[i],1);
//         if (len == 0 || buf[i] == '\n') break;
//         i++;
//       }
//       if (i == 0) break;
//       // terminating 0
//       buf[i] = 0;
//       full_argv[argc-1] = buf;
//       if (fork() == 0) {
//         // fork a child process to do the job
//         exec(full_argv[0],full_argv);
//         exit(0);
//       } else {
//         // wait for the child process to complete
//         wait(0);
//       }
//   }
//   exit(0);
// }

// //op就是传进来的指令，args是参数
// void run(char *op, char **args){
// 	if(fork() == 0){
// 		exec(op,args);
// 		exit(0);
// 	}
// 	return;
// }

// int main(int argc, char* argv[])
// {
// 	char buf[2048]; // 读入时使用的内存池
// 	char *p = buf, *last_p = buf;  //当前参数的结束、开始指针
// 	 // 全部参数列表，字符串指针数组，包含 argv 传进来的参数和 stdin 读入的参数
// 	char *argsbuf[128];
// 	char **args = argsbuf; 
	
// 	//将 argv 提供的参数加入到最终的参数列表中
// 	//可以用数组的角度去看这是个指针，vector<string> *args = argsbuf[0],*args +1 ...
// 	for(int i = 1; i < argc; i++)
// 	{
// 		*args = argv[i];
// 		args++;
// 	}
	
// 	// 开始读入从管道传过来的参数，然后根据换行符把相应的参数拼接上去
// 	char **pa = args; 

// 	while(read(0, p, 1) != 0) {
// 		if(*p == ' ' || *p == '\n') {
// 	// 读入一个参数完成（以空格分隔，如 `echo hello world`，则 hello 和 world 各为一个参数）
// 			// 读入一行完成,执行指令
// 			if(*p == '\n') {
// 				*pa = 0; // 参数列表末尾用 null 标识列表结束
// 				run(argv[1], argsbuf);
// 				pa = args; // 重置读入参数指针，准备读入下一行
// 			}
// 			*p = '\0';	
// 			// 将空格替换为 \0 分割开各个参数，这样可以直接使用内存池中的字符串作为参数字符串
// 			// 而不用额外开辟空间
// 			*(pa++) = last_p;
// 			last_p = p+1;
// 		}
// 		p++;
// 	}
	
// 	 //如果最后一行不是空行,还有未处理的参数
// 	if(pa != args) {
// 		// 收尾最后一个参数
// 		*p = '\0';
// 		*(pa++) = last_p;
// 		// 收尾最后一行
// 		*pa = 0; // 参数列表末尾用 null 标识列表结束
// 		// 执行最后一行指令
// 		run(argv[1], argsbuf);
// 	}
// 	while(wait(0) != -1) {}; // 循环等待所有子进程完成，每一次 wait(0) 等待一个
// 	exit(0);
// }

