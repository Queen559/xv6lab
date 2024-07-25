//FD_NONE（未使用）、FD_PIPE（管道文件）、FD_INODE（普通文件或目录）、FD_DEVICE（设备文件）
//表示文件描述符的状态，管理打开的文件的具体信息
struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  int ref; // reference count，表示多少进程共享这个文件结构
  char readable;
  char writable;
  struct pipe *pipe; // FD_PIPE，指向管道结构的指针，用于 FD_PIPE 类型
  struct inode *ip;  // FD_INODE and FD_DEVICE，指向 inode 结构的指针，用于 FD_INODE 和 FD_DEVICE 类型
  uint off;          // FD_INODE，文件偏移量
  short major;       // FD_DEVICE，设备号
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define	mkdev(m,n)  ((uint)((m)<<16| (n)))

// in-memory copy of an inode，表示文件系统中的 inode，是文件和目录在磁盘上的表示。
struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count，引用计数。
  struct sleeplock lock; // protects everything below here，保护 inode 的睡眠锁
  int valid;          // inode has been read from disk?inode 是否已从磁盘读取的标志

  short type;         // copy of disk inode
  short major;        //设备号的主要和次要部分。
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT+1];//数据块地址数组，用于存储文件内容的磁盘块地址
};

// map major device number to device functions.
struct devsw {
  int (*read)(int, uint64, int);
  int (*write)(int, uint64, int);
};

extern struct devsw devsw[];

#define CONSOLE 1
//struct file 用于内核内部管理打开的文件及其状态。
//struct stat 用于通过系统调用将文件的状态信息传递给用户空间程序。
//struct inode 用于文件系统内部管理文件和目录，是文件的底层表示