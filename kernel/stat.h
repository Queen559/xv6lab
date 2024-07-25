#define T_DIR     1   // Directory
#define T_FILE    2   // File
#define T_DEVICE  3   // Device
//存储文件的元数据信息，用于系统调用（如 fstat）返回文件的状态信息。
struct stat {
  int dev;     // File system's disk device，文件所在的磁盘设备号。
  uint ino;    // Inode number
  short type;  // Type of file
  short nlink; // Number of links to file，指向这个文件的目录项数量。
  uint64 size; // Size of file in bytes
};
