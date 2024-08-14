// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

//定义引用计数数组结构
#define MAXCOUNT PHYSTOP/PGSIZE
struct{
struct spinlock lock;
int count[MAXCOUNT];
}ref;

//增加引用计数
void incref(void*pa){
   acquire(&ref.lock);
   ref.count[(uint64)pa/PGSIZE]++;
   release(&ref.lock);
}

//减少引用计数
void decref(void*pa){
  acquire(&ref.lock);
   ref.count[(uint64)pa/PGSIZE]--;
   release(&ref.lock);
}

void
kinit()
{
  //初始化引用计数数组
  initlock(&ref.lock,"ref");
  for(int i=0;i<MAXCOUNT;i++){
    ref.count[i]=0;
  }
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
  //在这里要将相应的物理页面加1，否则会变为-1
  ref.count[(uint64)p/PGSIZE]=1;
    kfree(p);  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");
  decref(pa);
  if(ref.count[(uint64)pa/PGSIZE]>0) return;
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r){
    memset((char*)r, 5, PGSIZE); // fill with junk
    incref((void*)r);}
  return (void*)r;
}
