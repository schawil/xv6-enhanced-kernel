#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PGSIZE 4096

int main(int argc, char *argv[])
{
    char *buf;
    int pages = 4; // Nombre de pages à allouer

    printf("=== LAZY ALLOCATION TEST ===\n\n");

    // TEST 1: sbrk() sans accès (pas de page fault attendu)
    printf("TEST 1: Calling sbrk(%d) to reserve %d pages\n",
           pages * PGSIZE, pages);
    printf("        (NO physical allocation yet - LAZY)\n");

    buf = sbrk(pages * PGSIZE);
    if (buf == (char *)-1)
    {
        printf("ERROR: sbrk() failed\n");
        exit(1);
    }

    printf("        → sbrk() returned address: %p\n", buf);
    printf("        → Virtual memory reserved, waiting for access...\n\n");

    sleep(100); // Pause 1 seconde pour voir les logs

    // TEST 2: Accès à la PREMIÈRE page (devrait causer page fault)
    printf("TEST 2: Writing to FIRST page at %p\n", buf);
    printf("        → Expecting PAGE FAULT then allocation...\n");

    buf[0] = 'A';

    printf("        → Write SUCCESS! Page was allocated on-demand.\n");
    printf("        → Read back: buf[0] = '%c'\n\n", buf[0]);

    sleep(100);

    // TEST 3: Accès à la DERNIÈRE page (page fault aussi)
    char *last_page = buf + ((pages - 1) * PGSIZE);
    printf("TEST 3: Writing to LAST page at %p\n", last_page);
    printf("        → Expecting another PAGE FAULT...\n");

    last_page[0] = 'Z';
    last_page[PGSIZE - 1] = '!';

    printf("        → Write SUCCESS! Last page allocated.\n");
    printf("        → Read back: last[0]='%c', last[4095]='%c'\n\n",
            last_page[0], last_page[PGSIZE - 1]);

    sleep(100);

    // TEST 4: Accès à une page INTERMÉDIAIRE (page fault aussi)
    char *mid_page = buf + (2 * PGSIZE);
    printf("TEST 4: Writing to MIDDLE page at %p\n", mid_page);
    printf("        → Expecting PAGE FAULT...\n");

    mid_page[100] = 'M';

    printf("        → Write SUCCESS!\n");
    printf("        → Read back: mid[100] = '%c'\n\n", mid_page[100]);

    sleep(100);

    // TEST 5: Remplir toutes les pages (peut causer plusieurs faults)
    printf("TEST 5: Writing pattern to ALL pages\n");
    printf("        → May trigger multiple page faults...\n");

    for (int i = 0; i < pages * PGSIZE; i += PGSIZE)
    {
        buf[i] = 'X';
    }

    printf("        → ALL pages accessed and allocated!\n\n");

    printf("=== LAZY ALLOCATION TEST COMPLETE ===\n");
    printf("Check kernel logs above for [PAGE FAULT] messages.\n");
    printf("Each write to a new page should show:\n");
    printf("  [PAGE FAULT] → lazy_alloc() SUCCESS\n");

    exit(0);
}