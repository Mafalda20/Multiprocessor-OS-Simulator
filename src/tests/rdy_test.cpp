#include "rdy.h"
#include "exception.h"
#include "dbc.h"

#include <cstdio>
#include <errno.h>


static void testSPN()
{
    printf("===== TESTE SPN =====\n");
    // Esperado: 2, 1, 3 (runTime 3, 5, 7)

    rdyOpen(SPN);

    rdyInsert(1, 0.0, 5.0);
    rdyInsert(2, 0.0, 3.0);
    rdyInsert(3, 0.0, 7.0);

    printf("Conteúdo da fila (SPN):\n");
    rdyPrint(stdout, false);

    printf("Retirados (esperado: 2 1 3):\n");
    printf("%u\n", rdyRetrieve(0.0));
    printf("%u\n", rdyRetrieve(0.0));
    printf("%u\n", rdyRetrieve(0.0));

    rdyClose();
}

static void testSRT()
{
    printf("\n===== TESTE SRT =====\n");
    // Esperado: 20, 30, 10 (runTime 1, 2, 4)

    rdyOpen(SRT);

    rdyInsert(10, 0.0, 4.0);
    rdyInsert(20, 0.0, 1.0);
    rdyInsert(30, 0.0, 2.0);

    printf("Conteúdo da fila (SRT):\n");
    rdyPrint(stdout, false);

    printf("Retirados (esperado: 20 30 10):\n");
    printf("%u\n", rdyRetrieve(0.0));
    printf("%u\n", rdyRetrieve(0.0));
    printf("%u\n", rdyRetrieve(0.0));

    rdyClose();
}

static void testHRRN()
{
    printf("\n===== TESTE HRRN =====\n");

    /*
        curTime = 10.0

        P1: queueTime=0,  runTime=4 -> W=10 -> R=(10+4)/4 = 3.5
        P2: queueTime=5,  runTime=3 -> W=5  -> R=(5+3)/3 ≈ 2.67
        P3: queueTime=9,  runTime=1 -> W=1  -> R=(1+1)/1 = 2

        Primeiro a sair: PID 1
    */

    rdyOpen(HRRN);

    rdyInsert(1, 0.0, 4.0);
    rdyInsert(2, 5.0, 3.0);
    rdyInsert(3, 9.0, 1.0);

    printf("Conteúdo da fila (HRRN):\n");
    rdyPrint(stdout, false);

    printf("Retirado (esperado: 1):\n");
    printf("%u\n", rdyRetrieve(10.0));

    printf("Seguinte (restantes 2 e 3, ordem depende dos R):\n");
    printf("%u\n", rdyRetrieve(10.0));
    printf("%u\n", rdyRetrieve(10.0));

    rdyClose();
}

int main()
{
    try {
        testSPN();
        testSRT();
        testHRRN();
    }
    catch (const Exception &e) {
        fprintf(stderr, "Exception: %s (en=%d, func=%s)\n",
                e.what(), e.en, e.func);
        return 1;
    }

    return 0;
}
