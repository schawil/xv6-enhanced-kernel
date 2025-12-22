#include "kernel/types.h"
#include "user/user.h"

void burn_cpu(int id)
{
    double x = 1.0;
    // Boucle infinie pour chauffer
    while (1)
    {
        x = x * 1.000001;
    }
}

int main(int argc, char *argv[])
{
    int i;
    int n_process = 5; // Valeur par défaut
    int pids[30];      // Tableau pour stocker les IDs (Max 30)

    // Gestion de l'argument (nombre de processus)
    if (argc > 1)
    {
        n_process = atoi(argv[1]);
    }

    // Sécurité (xv6 a une limite de processus, on reste raisonnable)
    if (n_process < 1 || n_process > 20)
    {
        printf("Erreur : Choisissez entre 1 et 20 processus.\n");
        exit(1);
    }

    printf("=== TEST DYNAMIQUE EN CASCADE (%d Processus) ===\n", n_process);
    printf("Intervalle : ~3 secondes entre chaque lancement.\n");

    for (i = 0; i < n_process; i++)
    {
        int pid = fork();

        if (pid == 0)
        {
            // ENFANT
            int my_id = getpid();
            // On affiche clairement l'arrivée du nouveau
            printf("\n>>> [NOUVEAU] Process %d arrive (FRAIS) !\n", my_id);
            burn_cpu(my_id);
            exit(0);
        }
        else
        {
            // PARENT
            pids[i] = pid;

            // On attend 30 ticks (environ 3 secondes) avant de lancer le suivant
            // C'est ce délai qui permet de voir le "Relais" Frais vs Chaud
            sleep(30);
        }
    }

    printf("\n=== TOUT LE MONDE EST LA (%d). OBSERVATION 5s ===\n", n_process);
    sleep(50); // On laisse tourner un peu tout le monde ensemble

    // Nettoyage propre
    printf("Fin du test. Nettoyage...\n");
    for (i = 0; i < n_process; i++)
        kill(pids[i]);
    for (i = 0; i < n_process; i++)
        wait(0);

    exit(0);
}