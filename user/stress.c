#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int target_mb = 0; // Par défaut 0 MB (CPU only)

    // Si on tape "stress 50", target_mb vaut 50
    if (argc > 1)
    {
        target_mb = atoi(argv[1]);
    }

    printf("=== STRESS TEST (%d MB + CPU) ===\n", target_mb);

    if (target_mb > 0)
    {
        // 1. Allocation (Seulement si demandé)
        uint64 size = (uint64)target_mb * 1024 * 1024;
        printf("Allocation de %d MB...\n", target_mb);

        char *start = sbrk(size);
        if (start == (char *)-1)
        {
            printf("Erreur sbrk\n");
            exit(1);
        }

        // Touch (Ecriture pour forcer l'allocation physique)
        for (int i = 0; i < size; i += 4096)
            start[i] = 'X';
    }
    else
    {
        printf("Aucune RAM allouee (Test CPU pur).\n");
    }

    // 2. Boucle CPU Infinie
    printf("Lancement boucle CPU (CTRL+C pour stopper)...\n");
    double x = 10.0;
    while (1)
    {
        x = x * 1.00001; // Calcul flottant lourd
    }
}