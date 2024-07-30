// PhyFical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.
//这段代码实现了一个简单的内核内存分配器，负责初始化、分配和释放内存页
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);
//当操作系统启动时，内核加载到内存中，包含内核代码、全局变量和静态数据。end 符号标志着这些部分的结束位置,即内核可以开始分配内存的位置。
extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

//用于构建空闲内存页链表。每个 run 结构体表示一个空闲内存页，并包含一个指向下一个空闲内存页的指针 next。
struct run {
  struct run *next;
};

//其中包含一个自旋锁 lock 和一个空闲内存页链表 freelist。kmem 是内存分配器的全局变量。
struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

//kinit 函数初始化内存分配器。
void
kinit()
{
  //初始化自旋锁 kmem.lock
  initlock(&kmem.lock, "kmem");
  //释放从 end 到 PHYSTOP 范围内的所有内存页。
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  //将给定的大小参数向上舍入到最近的页边界，确保内存分配和管理操作以页为单位进行，从而避免内存碎片和不对齐问题。
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
//kfree 函数释放一个物理内存页
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  //用于初始化或填充内存区域。
  memset(pa, 1, PGSIZE);
//将内存页转换为 run 结构体
  r = (struct run*)pa;
//获得自旋锁 kmem.lock 以保证线程安全
  acquire(&kmem.lock);
  //用于将一个空闲的内存页块（r）插入到空闲链表的开头
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
//函数分配一个 4096 字节（一个页）的物理内存
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  //如果取到了内存页，则将链表头移动到下一个节点
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

//计算可用内存量，即先要计算空闲内存页的页数，而空闲内存页都存在空闲内存页链表中
uint64 freemem(void){
  struct run *r;
  uint64 num=0;
  acquire(&kmem.lock);
  r = kmem.freelist;
  while(r){
    num+=1;
    r=r->next;
  }
  release(&kmem.lock);
  return num*PGSIZE;
}