#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid_monitor;
  int pid_stress;

  printf("\n=== SCENARIO 4 PRESSION MEMOIRE ===\n");
  
  // 1. Lancer le Moniteur
  printf("[1/3] Lancement du Moniteur (Background)...\n");
  pid_monitor = fork();
  if(pid_monitor == 0){
      char *args[] = {"activitymon", 0};
      exec("activitymon", args);
      exit(1);
  }
  
  // Attendre 6 secondes (pour voir au moins une ligne "LIBRE")
  sleep(60); 

  // 2. Lancer le Stress
  printf("\n[2/3] Injection de charge (50MB + CPU)...\n");
  pid_stress = fork();
  if(pid_stress == 0){
      // On passe "50" en argument, sur une ligne séparée du commentaire !
      char *args[] = {"stress", "50", 0}; 
      exec("stress", args);
      exit(1);
  }

  // Attendre 15 secondes
  sleep(150);

  // 3. Kill
  printf("\n[3/3] Fin du test. Nettoyage.\n");
  //kill(pid_stress);   // Le stress meurt
  //kill(pid_monitor);  // Le moniteur meurt
  
  wait(0);
  wait(0);

  printf("=== DEMO TERMINEE ===\n");
  exit(0);
}