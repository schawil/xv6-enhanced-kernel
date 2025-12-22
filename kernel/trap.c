#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

// ====================================================================
// SECTION CONSTANTS
// ====================================================================
#define HEAT_INCREMENT 10

// ====================================================================
// SECTION EXTERNES
extern void uservec(void);   // Défini dans trampoline.S
extern void userret(void);   // Défini dans trampoline.S
extern void kernelvec(void); // Défini dans kernelvec.S
extern char trampoline[];    // Trampoline section in memory

// Prototype local pour calmer le compilateur
int devintr(void);

// ====================================================================
// SECTION STATS (Variables globales définies dans proc.c)
// ====================================================================
extern uint64 g_total_ticks;
extern uint64 g_cpu_active_ticks;
extern uint64 g_interrupt_count;
extern struct spinlock statslock;

struct spinlock tickslock;
uint64 ticks;

void trapinit(void)
{
  initlock(&tickslock, "time");
}

//
// set up to take exceptions and traps while in the kernel.
//
void trapinithart(void)
{
  w_stvec((uint64)kernelvec);
}

void usertrap(void)
{
  int which_dev = 0;
  uint64 scause = r_scause();
  if ((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();

  // save user program counter.
  p->trapframe->epc = r_sepc();

  if (r_scause() == 8)
  {
    // system call

    if (killed(p))
      exit(-1);

    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;

    // an interrupt will change sepc, scause, and sstatus,
    // so enable only now that we're done with those registers.
    intr_on();

    syscall();
  }
  else if ((which_dev = devintr()) != 0)
  {
    // ok
    // --- Activity Monitor (Stats Interruptions) ---
    acquire(&statslock);
    g_interrupt_count++;
    release(&statslock);
  }else if((scause & 0x8000000000000000L) &&
            (scause & 0xff) == 9){
    // timer interrupt
    which_dev = devintr();
    
    // ============================================================
    // HEAT-AWARE: Increment CPU heat on each tick
    // ============================================================
    if(which_dev == 2) {
    // ============================================================
    // HEAT-AWARE: Increment CPU heat on each timer tick
    // ============================================================
    struct proc *p = myproc();
    if(p != 0) {
      acquire(&p->lock);
      p->cpu_heat += 5;  // HEAT_INCREMENT (défini dans proc.c)
      p->total_ticks++;
      release(&p->lock);
    }
    
    yield();
  }
    
  } else if(scause == 13 || scause == 15)
  {
    // PAGE FAULT: scause 13 = load, 15 = store
    uint64 fault_va = r_stval(); // Adresse virtuelle qui a causé le fault

    // Log pour debugging et rapport
    printf("[PAGE FAULT] pid=%d va=%p scause=%p\n",
          p->pid, fault_va, scause);

    // Vérifier si c'est une adresse dans la zone lazy
    if (fault_va >= p->sz)
    {
      // Accès au-delà du heap → erreur légitime
      printf("  → Out of bounds (va >= sz)\n");
      setkilled(p);
    }
    else
    {
      // Adresse valide mais page non allouée → LAZY ALLOCATION
      if (lazy_alloc(p->pagetable, fault_va) < 0)
      {
        // Échec allocation (plus de mémoire)
        printf("  → lazy_alloc() failed - out of memory\n");
        setkilled(p);
      }
      else
      {
        // Succès ! Page allouée
        //printf("  → lazy_alloc() SUCCESS - page allocated at %p\n", fault_va);
      }
    }
  }
  else
  {
    printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
    printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
    setkilled(p);
  }

  if (killed(p))
    exit(-1);

  // give up the CPU if this is a timer interrupt.
  if (which_dev == 2)
  {

    // --- 1. Activity Monitor Stats ---
    acquire(&statslock);
    g_total_ticks++;
    if (p != 0 && p->state == RUNNING)
    {
      g_cpu_active_ticks++;
    }
    release(&statslock);

    // --- 2. Heat Scheduler (LE RADIATEUR) ---
    if (p != 0)
    {
      acquire(&p->lock);
      p->cpu_heat++;
      // MOUCHARD : Affiche quand ça chauffe (tous les 10 degres)
      if (p->cpu_heat % 10 == 0)
      printf("DEBUG TRAP: PID %d Heat monte a %d\n", p->pid, p->cpu_heat);
      release(&p->lock);
    }
    // ----------------------------------------

    yield();
  }

  usertrapret();
}

//
// return to user space
//
void usertrapret(void)
{
  struct proc *p = myproc();

  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(), so turn off interrupts until
  // we're back in user space, where usertrap() is correct.
  intr_off();

  // send syscalls, interrupts, and exceptions to uservec in trampoline.S
  // CORRECTION HERE: Cast en (uint64) pour éviter l'erreur "invalid operands"
  uint64 trampoline_uservec = TRAMPOLINE + ((uint64)uservec - (uint64)trampoline);
  w_stvec(trampoline_uservec);

  // set up trapframe values that uservec will need when
  // the process next traps into the kernel.
  p->trapframe->kernel_satp = r_satp();         // kernel page table
  p->trapframe->kernel_sp = p->kstack + PGSIZE; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp(); // hartid for cpuid()

  // set up the registers that trampoline.S's sret will use
  // to get to user space.

  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->trapframe->epc);

  // tell trampoline.S the user page table to switch to.
  uint64 satp = MAKE_SATP(p->pagetable);

  // jump to userret in trampoline.S at the top of memory, which
  // switches to the user page table, restores user registers,
  // and switches to user mode with sret.
  // CORRECTION HERE: Cast en (uint64) aussi
  uint64 trampoline_userret = TRAMPOLINE + ((uint64)userret - (uint64)trampoline);
  ((void (*)(uint64))trampoline_userret)(satp);
}

// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
void kerneltrap()
{
  int which_dev = 0;
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();

  if ((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");
  if (intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  if ((which_dev = devintr()) == 0)
  {
    printf("scause %p\n", scause);
    printf("sepc=%p stval=%p\n", r_sepc(), r_stval());
    panic("kerneltrap");
  }

  // --- SENIOR FIX: Comptage Kernel ---
  acquire(&statslock);
  g_interrupt_count++;

  if (which_dev == 2)
  {
    g_total_ticks++;
    if (myproc() != 0 && myproc()->state == RUNNING)
    {
      g_cpu_active_ticks++;
    }
  }
  release(&statslock);
  // -----------------------------------

  // give up the CPU if this is a timer interrupt.
  if (which_dev == 2 && myproc() != 0 && myproc()->state == RUNNING)
  {
    // --- AJOUT SCHEDULER ---
    myproc()->cpu_heat++; // Le processus chauffe aussi en mode noyau
    // -----------------------
    yield();
  }

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
  w_sepc(sepc);
  w_sstatus(sstatus);
}

void clockintr()
{
  acquire(&tickslock);
  ticks++;
  wakeup(&ticks);
  release(&tickslock);
}

// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.
int devintr()
{
  uint64 scause = r_scause();

  if ((scause & 0x8000000000000000L) &&
      (scause & 0xff) == 9)
  {
    // this is a supervisor external interrupt, via PLIC.

    // irq indicates which device interrupted.
    int irq = plic_claim();

    if (irq == UART0_IRQ)
    {
      uartintr();
    }
    else if (irq == VIRTIO0_IRQ)
    {
      virtio_disk_intr();
    }
    else if (irq)
    {
      printf("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    if (irq)
      plic_complete(irq);

    return 1;
  }
  else if (scause == 0x8000000000000001L)
  {
    // software interrupt from a machine-mode timer interrupt,
    // forwarded by timervec in kernelvec.S.

    if (cpuid() == 0)
    {
      clockintr();
    }

    // acknowledge the software interrupt by clearing
    // the SSIP bit in sip.
    w_sip(r_sip() & ~2);

    return 2;
  }
  else
  {
    return 0;
  }
}