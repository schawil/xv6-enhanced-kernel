#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  struct activity act;

  printf("=== SCENARIO 4 : PRESSOIN MEMOIRE ===\n");
  printf("Ne touchez a rien pendant 15 secondes.\n");
  printf("Observez UserAct passer de 1 a 0.\n\n");

  while (1)
  {
    // 1. Appel au système
    if (getactivity(&act) < 0)
    {
      printf("Erreur getactivity\n");
      exit(1);
    }

    // 2. Affichage des données brutes
    printf("[%s] Etat: %s | CPU=%d%% | MEM=%d%% | UserAct=%d | Intr=%d\n",
           act.timestamp,
           (act.state == 1 ? "OCCUPEE" : "LIBRE"),
           act.cpu_percent,
           act.mem_percent,
           act.user_activity,
           act.interrupts);

    // 3. Pause de 5 secondes (environ 50 ticks)
    sleep(50);
  }
  exit(0);
}