#include "kernel/types.h"
#include "user/user.h"

void
work_hard(int id)
{
  double x = 1.0;
  int i = 0;
  printf("Process %d (CHAUFFAGE) demarre...\n", id);
  while(1) {
      x = x * 1.000001;
      i++;
      // On n'affiche rien pour ne pas être ralenti par l'écran.
      // On chauffe en silence.
  }
}

void
work_light(int id)
{
  printf("Process %d (FRAIS) demarre...\n", id);
  int i;
  for(i = 0; i < 20; i++) {
      // Le processus frais travaille un peu, mais dort aussi.
      // Cela maintient sa température basse (car il quitte le CPU souvent).
      sleep(10); 
      printf("Process %d (FRAIS) : Je passe devant car je suis frais !\n", id);
  }
}

int
main(int argc, char *argv[])
{
  int pid1, pid2;

  printf("=== DUEL : CHAUD vs FRAIS ===\n");

  // Lance le Bad Guy (Chauffage)
  pid1 = fork();
  if(pid1 == 0) {
      work_hard(getpid());
      exit(0);
  }

  // Lance le Good Guy (Frais)
  pid2 = fork();
  if(pid2 == 0) {
      work_light(getpid());
      exit(0);
  }

  // Le parent attend
  wait(0);
  wait(0);
  exit(0);
}