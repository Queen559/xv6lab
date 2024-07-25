#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  // 用于保存用户空间传入的掩码值
  int mask;

  // 从用户空间获取第一个系统调用参数并存储到 mask 中
  if(argint(0, &mask) < 0)
    return -1;

  // 打印调试信息，显示掩码值
  //printf("sys_trace: hi! n is %d\n", mask);

  // 将掩码值保存到当前进程的 mask 字段中
  myproc()->mask = mask;
  //printf("sys_trace: hi! n is %d\n", myproc()->mask);
  return 0;
}

//
uint64
sys_sysinfo(void){
  struct proc *p = myproc();
  struct sysinfo info;
  uint64 addr;//  user pointer to struct sysinfo
  if(argaddr(0, &addr) < 0)
  return -1;
  info.freemem=freemem();
  info.nproc=nproc();
  //将 st 中的状态信息拷贝到用户空间地址 addr 指向的内存中，(char *)&st 表示将结构体指针转换为字符指针
  if(copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0)
    return -1;
  return 0;
}
