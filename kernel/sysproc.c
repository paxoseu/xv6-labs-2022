#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


pte_t *
walkcpy(pagetable_t pagetable, uint64 va, int alloc)
{
    if(va >= MAXVA)
        panic("walk");

    for(int level = 2; level > 0; level--) {
        pte_t *pte = &pagetable[PX(level, va)];
        if(*pte & PTE_V) {
            pagetable = (pagetable_t)PTE2PA(*pte);
        } else {
            if(!alloc || (pagetable = (pde_t*)kalloc()) == 0)
                return 0;
            memset(pagetable, 0, PGSIZE);
            *pte = PA2PTE(pagetable) | PTE_V;
        }
    }
    return &pagetable[PX(0, va)];
}

#ifdef LAB_PGTBL
// detect pagetable if it's accessed;
uint64
sys_pgaccess(void)
{
  struct proc *p = myproc();
  pagetable_t ptb = p -> pagetable;
  vmprint(ptb);
  uint64 sva;
  int page_cnt;
  uint64 bit_mask_addr;
  argaddr(0, &sva);
  argint(1, &page_cnt);
  argaddr(2, &bit_mask_addr);

  uint64 bit = 0;

  for (int idx = 0; idx < page_cnt; idx ++ )
  {
      uint64 addr = sva + idx * (1<<12);
      pte_t *pte = walkcpy(ptb, addr, 0);
      if (*pte & PTE_A && *pte & PTE_V) {
        *pte ^= PTE_A; //reset
        bit |= (1L << idx);
      }
  }
  if (copyout(ptb, bit_mask_addr, (char *)&bit, sizeof(bit)) < 0) {
      return -1;
  }
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
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
