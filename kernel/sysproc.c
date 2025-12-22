#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "activity.h" // 

extern uint64 g_total_ticks;
extern uint64 g_cpu_active_ticks;
extern uint64 g_interrupt_count;
extern uint64 g_last_keypress_tick;
extern struct spinlock statslock;
extern uint64 ticks;
extern struct spinlock tickslock;

static uint64 last_total_ticks = 0;
static uint64 last_cpu_active = 0;
static uint64 last_int_count = 0;

// Helper: Convertit un entier < 100 en chaine 2 chars
void fmt_int(char *b, int n)
{
  if (n < 10)
  {
    b[0] = '0';
    b[1] = n + '0';
  }
  else
  {
    b[0] = (n / 10) + '0';
    b[1] = (n % 10) + '0';
  }
}

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
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

/* uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

// DANS kernel/sysproc.c
 */

/*



*/

/*

========  Fonction originale (avant modification) ========


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

*/

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);

  struct proc *p = myproc();
  addr = p->sz; // On sauvegarde l'ancienne taille (adresse de retour)

  // --- LE BLUFF (Lazy Allocation) ---
  if (n > 0)
  {
    // On augmente juste la taille "sur le papier" (p->sz).
    // On n'appelle PAS growproc(). On ne donne pas de RAM physique.
    p->sz += n;
  }
  else if (n < 0)
  {
    // Si n est négatif, on réduit la mémoire pour de vrai.
    // Là, on doit appeler growproc car on libère de la place.
    if (growproc(n) < 0)
      return -1;
  }
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (killed(myproc()))
    {
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

uint64
sys_getactivity(void)
{
  uint64 addr;
  struct activity act;
  uint64 free_mem, total_mem;
  uint64 current_total, current_active, current_int;

  argaddr(0, &addr);

  acquire(&statslock);
  current_total = g_total_ticks;
  current_active = g_cpu_active_ticks;
  current_int = g_interrupt_count;
  release(&statslock);

  uint64 delta_total = current_total - last_total_ticks;
  uint64 delta_active = current_active - last_cpu_active;
  uint64 delta_int = current_int - last_int_count;

  last_total_ticks = current_total;
  last_cpu_active = current_active;
  last_int_count = current_int;

  // CPU
  if (delta_total == 0)
    act.cpu_percent = 0;
  else
    act.cpu_percent = (delta_active * 100) / delta_total;

  // MEM
  get_memory_stats(&free_mem, &total_mem);
  if (total_mem == 0)
    act.mem_percent = 0;
  else
    act.mem_percent = ((total_mem - free_mem) * 100) / total_mem;

  // USER ACTIVITY (Timeout ~10s)
  act.user_activity = ((ticks - g_last_keypress_tick) < 100) ? 1 : 0;

  // INTERRUPTS
  act.interrupts = delta_int;

  // STATE
  if (act.cpu_percent > 20 || act.user_activity == 1 || act.mem_percent > 30)
    act.state = 1;
  else
    act.state = 0;

  // TIMESTAMP CORRIGÉ (YYYY-MM-DD en dur pour éviter le bug d'affichage)
  // Heure dynamique basée sur le temps depuis le boot + heure de départ
  int START_HOUR = 19; // <--- Mets ton heure ici
  int START_MIN = 30;

  // 10 ticks = 1 seconde approx sur QEMU virt
  uint64 seconds_running = ticks / 10;

  uint64 total_min = START_MIN + (seconds_running / 60);
  uint64 total_hour = START_HOUR + (total_min / 60);

  int s = seconds_running % 60;
  int m = total_min % 60;
  int h = total_hour % 24;

  char *t = act.timestamp;
  // On écrit la date en dur pour éviter le problème d'entier > 99
  safestrcpy(t, "2025-12-22 ", 32);

  // On remplit seulement l'heure (offset 11)
  fmt_int(t + 11, h);
  t[13] = ':';
  fmt_int(t + 14, m);
  t[16] = ':';
  fmt_int(t + 17, s);
  t[19] = '\0';

  if (copyout(myproc()->pagetable, addr, (char *)&act, sizeof(act)) < 0)
    return -1;
  return 0;
}
