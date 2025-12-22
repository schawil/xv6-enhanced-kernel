#include "kernel/types.h"
#include "user/user.h"

// --- FONCTION CHAUFFAGE (BAD GUY) ---
void
work_hard(int id)
{
  double x = 1.0;
  // On ne print pas dans la boucle pour ne pas ralentir le chauffage
  // et pour ne pas spammer la console (le kernel le fera via les logs PENALISE)
  while(1) {
      x = x * 1.000001;
  }
}

// --- FONCTION FRAICHEUR (GOOD GUY) ---
void
work_light(int id)
{
  //int i;
  while(1) {
      // Dort 1 seconde (10 ticks)
      sleep(10); 
      // Si j'arrive à imprimer ça, c'est que le scheduler m'a donné la main !
      printf("   [P%d] Je suis FRAIS et je passe !\n", id);
  }
}

int
main(int argc, char *argv[])
{
  int n_hot = 1;
  int n_fresh = 1;
  int pids[30]; // On limite à 30 processus max pour le test
  int count = 0;

  // Lecture des arguments: multitest <nb_hot> <nb_fresh>
  if(argc >= 2) n_hot = atoi(argv[1]);
  if(argc >= 3) n_fresh = atoi(argv[2]);

  if(n_hot + n_fresh > 28) {
      printf("Erreur: Trop de processus demandes (Max ~28)\n");
      exit(1);
  }

  printf("=== MULTITEST: %d Hot vs %d Fresh ===\n", n_hot, n_fresh);
  printf("Test pendant 10 secondes...\n\n");

  // 1. Lancement des BAD GUYS (Chauffage)
  for(int i=0; i<n_hot; i++){
      int pid = fork();
      if(pid == 0){
          work_hard(getpid());
          exit(0);
      }
      pids[count++] = pid;
  }

  // 2. Lancement des GOOD GUYS (Frais)
  for(int i=0; i<n_fresh; i++){
      int pid = fork();
      if(pid == 0){
          work_light(getpid());
          exit(0);
      }
      pids[count++] = pid;
  }

  // 3. SUPERVISEUR (On laisse tourner 10 secondes)
  sleep(100); // 100 ticks = ~10s

  // 4. NETTOYAGE (On tue tout le monde)
  printf("\n=== FIN DU TEST - NETTOYAGE ===\n");
  for(int i=0; i<count; i++){
      kill(pids[i]);
  }
  
  // On attend que les cadavres disparaissent
  for(int i=0; i<count; i++){
      wait(0);
  }

  printf("Test termine proprement.\n");
  exit(0);
}