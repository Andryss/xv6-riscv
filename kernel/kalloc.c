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
int pagenum(void *pa);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

static struct spinlock refslock;
static int refs[64000]; // count usage of each page
                        // refslock must be held

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&refslock, "refslock");
  freerange(end, (void*)PHYSTOP);
}

void freelist_add(void *pa) {
  struct run *r;

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  printf("amount of pages: %d\n", (pa_end - pa_start) / PGSIZE);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    freelist_add(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  if(pagerefsdec(pa) > 0)
    return;

  freelist_add(pa);
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

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  if(r)
    refs[pagenum(r)] = 1;
  return (void*)r;
}

// return number of the page
int
pagenum(void *pa)
{
  return (int) (pa - (void*)end) / PGSIZE;
}

// atomically increase number of page references
int
pagerefsinc(void *pa)
{
  int newrefs;

  acquire(&refslock);
  newrefs = ++refs[pagenum(pa)];
  release(&refslock);

  return newrefs;
}

// atomically decrease number of page references
int
pagerefsdec(void *pa)
{
  int newrefs;

  acquire(&refslock);
  newrefs = --refs[pagenum(pa)];
  release(&refslock);

  return newrefs;
}
