#include "kernel/types.h"
#include "user/user.h"

void
burn_cpu(int id)
{
  double x = 1.0;
  // Boucle infinie pour chauffer
  while(1) {
      x = x * 1.000001;
  }
}

int
main(int argc, char *argv[])
{
  int i;
  int pids[10];

  printf("=== TEST DYNAMIQUE (CASCADE) ===\n");
  printf("Lancement de 5 processus les uns apres les autres...\n");
  printf("Regardez comment le NOUVEAU (Frais) vole la vedette aux ANCIENS (Chauds).\n\n");

  for(i = 0; i < 5; i++) {
      int pid = fork();
      
      if(pid == 0) {
          // ENFANT
          int my_id = getpid();
          printf("\n>>> [NOUVEAU] Process %d arrive sur la piste (FRAIS) !\n", my_id);
          burn_cpu(my_id);
          exit(0);
      } else {
          // PARENT
          pids[i] = pid;
          // On attend 20 ticks (environ 2 secondes) avant de lancer le suivant
          // C'est ça qui crée l'effet de "Relais"
          sleep(30); 
      }
  }

  printf("\n=== TOUT LE MONDE EST LA. FIN DU SPECTACLE DANS 5s ===\n");
  sleep(50);

  // Nettoyage
  for(i = 0; i < 5; i++) kill(pids[i]);
  for(i = 0; i < 5; i++) wait(0);
  
  exit(0);
}