// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"
// #include "kernel/fs.h"

// char*
// fmtname(char *path)
// {
//   static char buf[DIRSIZ+1];
//   char *p;

//   // Find first character after last slash.
//   for(p=path+strlen(path); p >= path && *p != '/'; p--)
//     ;
//   p++;

//   // Return blank-padded name.
//   if(strlen(p) >= DIRSIZ)
//     return p;
//   memmove(buf, p, strlen(p));
//   memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
//   return buf;
// }

// //递归函数
// //1、递归函数的参数传入逻、返回值,因为要比较是否名字相等，所以要把名字也传进去
// void backtrack(char* path,char *name){
// char buf[512], *p; //保存文件名的缓冲数组
// int fd; //文件描述符
// struct dirent de;  //目录相关的结构体
// struct stat st;  //文件相关的结构体，包含文件的状态信息
// //2、递归函数的终止条件：读到的文件是空
// if((fd = open(path, 0)) < 0){
//     fprintf(2, "ls: cannot open %s\n", path);
//     exit(0);
//   }
// //3、递归的逻辑应该写在哪里
// //fstat 函数用于获取与文件描述符 fd 关联的文件状态信息
// if(fstat(fd, &st) < 0){
//     fprintf(2, "ls: cannot stat %s\n", path);
//     close(fd);
//     exit(0);
//   }
//   //4、输出的时候只是输出文件名吗
// switch(st.type){
//   case T_FILE://如果是文件类型
//   //如果当前路径和文件名相同，则代表找到文件了
//     if(strcmp(fmtname(path),name)==0){
//         printf("%s\n", fmtname(buf));
//         break;
//         //exit(0);
//      }
//   case T_DIR://如果是目录类型
//     if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
//       printf("ls: path too long\n");
//       break;
//     }
//     strcpy(buf, path);////将路径放入buf中
//     p = buf+strlen(buf);
//     *p++ = '/';
//     while(read(fd, &de, sizeof(de)) == sizeof(de)){
//       if(de.inum == 0)
//         continue;
      
//       memmove(p, de.name, DIRSIZ);
//       p[DIRSIZ] = 0;
//     //不去递归处理.和..
//         if(!strcmp(de.name, ".") || !strcmp(de.name, ".."))
//         continue;
//       backtrack(buf,name);//传入的路径应该是buf
//     }
//     break;
//   }
//   close(fd);

// }

// int main(int argc,char*argv[]){
// if(argc!=3){
//     fprintf(2, "usage:find <path> <name>\n");
//     exit(1);
// }
// backtrack(argv[1],argv[2]);
// exit(0);
// }
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
//提取文件名函数
char* fmtname(char *path) //参考ls中的fmtname代码
{
	static char buf[DIRSIZ+1];
 	char *p;
  	// Find first character after last slash.
  	for(p=path+strlen(path); p >= path && *p != '/'; p--)
    	;
  	p++;
  	// Return blank-padded name.
  	if(strlen(p) >= DIRSIZ)
    	return p;
  	memmove(buf, p, strlen(p));//从存储区p复制strlen(p)个字节到存储区buf
	buf[strlen(p)] = 0;  //字符串结束符
  	return buf;
}

void
find(char *path, char *name)
{
 	char buf[512], *p;  //声明的这些结构体、变量等也和ls一样
 	int fd;
 	struct dirent de;
 	struct stat st;

	 if((fd = open(path, 0)) < 0){ //判断，也和ls一样
	   fprintf(2, "find open %s error\n", path);
	   exit(1);
	 }
	 if(fstat(fd, &st) < 0){
	   fprintf(2, "fstat %s error\n", path);
	   close(fd);
	   exit(1);
	 }

	 switch(st.type){
	 case T_FILE:  //如果是文件类型
	 	if(strcmp(fmtname(path), name) == 0) 
	     	printf("%s\n", path); //如果当前路径和文件名相同，则代表找到文件了,打印的是路径不是文件名
	  	break;
	 case T_DIR:  //如果是目录类型
	 	if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
	    	printf("find: path too long\n");
	       	break;
	   	}
	   	strcpy(buf, path); //将输入的目录字符串复制到buf中
	   	p = buf+strlen(buf);//指针p指向了buf的末尾位置
	   	*p++ = '/';  //将`/`拼接在后面
	   	//逐个读取目录项，直到读取失败或读取到的目录项为空。
	   	while(read(fd, &de, sizeof(de)) == sizeof(de)){
	      	if(de.inum == 0)//目录项为空则跳过
	          	continue;
	      	memmove(p, de.name, DIRSIZ);//将 de.name 的内容拷贝到 p 指向的位置开始的内存区域。
	       	p[DIRSIZ] = 0;//设置文件名结束符
	       //不去递归处理.和..
	       if(!strcmp(de.name, ".") || !strcmp(de.name, ".."))//.代表当前目录,..代表父目录
	           continue;
	       find(buf, name); //继续进入下一层目录递归处理
	   }
	   break;
	 }
	 close(fd);//将该文件描述符从进程的文件表中移除
}

int
main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "usage:find <path> <name>\n");
    exit(1);//1表示出现了一些一般性的错误。
  }
    find(argv[1], argv[2]);
    exit(0);//0表示程序正常退出。
}
