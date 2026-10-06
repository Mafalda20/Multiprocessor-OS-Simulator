#include "somm25nm.h"
#include "exception.h"

#include <cstdio>
#include <cstdint>
#include <new>
#include <errno.h>

extern FeqNode *feqHead;

static void banner(const char *msg)
{
    std::printf("\n==================== %s ====================\n\n", msg);
}

/* ========================== */
/* Teste FEQ: feqOpen/feqClose */
/* ========================== */

static void testFeqOpenClose()
{
    banner("Testing FEQ (feqOpen / feqClose)");

    soBinSetIDs(0, 0);

    try {
        feqOpen();
        std::printf("FEQ opened OK.\n");
        std::printf("feqHead after open: %p (should not be FEQ_UNDEF_NODE)\n", feqHead);
        
        feqClose();
        std::printf("FEQ closed OK.\n");
        std::printf("feqHead after close: %p (should be FEQ_UNDEF_NODE)\n", feqHead);
    }
    catch (const Exception &e) {
        std::printf("Exception in testFeqOpenClose: %s (errno=%d)\n", e.what(), e.en);
    }
}

/* ========================== */
/* Teste FEQ: feqInsert e ordenação */
/* ========================== */

static void testFeqInsert()
{
    banner("Testing FEQ (feqInsert with sorting)");

    soBinSetIDs(0, 0);

    try {
        feqOpen();

        // Inserir eventos com diferentes tempos
        std::printf("Inserting events...\n");
        feqInsert(10.0, SUBMIT, 1);
        feqInsert(5.0, ADMIT, 2);
        feqInsert(15.0, EXIT, 3);
        feqInsert(5.0, DISPATCH, 4);  // Mesmo tempo que ADMIT, mas DISPATCH deve vir primeiro
        feqInsert(10.0, WAIT_EVENT, 5); // Mesmo tempo que SUBMIT, WAIT_EVENT deve vir primeiro
        feqInsert(10.0, TIMEOUT, 6);    // Mesmo tempo, TIMEOUT deve vir antes de SUBMIT
        feqInsert(12.0, PREEMPT, 7);
        
        std::printf("\nFEQ after insertions (normal format):\n");
        feqPrint(stdout, false);
        
        std::printf("\nFEQ after insertions (CSV format):\n");
        feqPrint(stdout, true);

        feqClose();
        std::printf("\nFEQ closed after test.\n");
    }
    catch (const Exception &e) {
        std::printf("Exception in testFeqInsert: %s (errno=%d)\n", e.what(), e.en);
    }
}

/* ========================== */
/* Teste FEQ: feqRetrieve */
/* ========================== */

static void testFeqRetrieve()
{
    banner("Testing FEQ (feqRetrieve)");

    soBinSetIDs(0, 0);

    try {
        feqOpen();

        // Inserir alguns eventos
        feqInsert(10.0, SUBMIT, 100);
        feqInsert(5.0, EXIT, 200);
        feqInsert(15.0, ADMIT, 300);
        feqInsert(5.0, DISPATCH, 400); // Deve vir antes de EXIT

        std::printf("FEQ before retrieval:\n");
        feqPrint(stdout, false);

        // Recuperar eventos um a um
        double time;
        FeqEventType type;
        uint32_t xid;

        std::printf("\nRetrieving events:\n");
        
        bool result = feqRetrieve(&time, &type, &xid, false);
        if (result) {
            std::printf("1st event: time=%.1f, type=%d (DISPATCH?), xid=%u (expected: 400)\n", 
                        time, (int)type, xid);
        }

        result = feqRetrieve(&time, &type, &xid, false);
        if (result) {
            std::printf("2nd event: time=%.1f, type=%d (EXIT?), xid=%u (expected: 200)\n", 
                        time, (int)type, xid);
        }

        result = feqRetrieve(&time, &type, &xid, false);
        if (result) {
            std::printf("3rd event: time=%.1f, type=%d (SUBMIT?), xid=%u (expected: 100)\n", 
                        time, (int)type, xid);
        }

        result = feqRetrieve(&time, &type, &xid, false);
        if (result) {
            std::printf("4th event: time=%.1f, type=%d (ADMIT?), xid=%u (expected: 300)\n", 
                        time, (int)type, xid);
        }

        // Tentar recuperar de fila vazia
        std::printf("\nTrying to retrieve from empty queue (non-blocking):\n");
        result = feqRetrieve(&time, &type, &xid, false);
        std::printf("Result: %s (expected: false)\n", result ? "true" : "false");

        feqClose();
        std::printf("\nFEQ closed after test.\n");
    }
    catch (const Exception &e) {
        std::printf("Exception in testFeqRetrieve: %s (errno=%d)\n", e.what(), e.en);
    }
}

/* ========================== */
/* Teste FEQ: Prioridades completas */
/* ========================== */

static void testFeqPriorities()
{
    banner("Testing FEQ (Event Priorities)");

    soBinSetIDs(0, 0);

    try {
        feqOpen();

        // Inserir vários eventos com mesmo tempo para testar prioridades
        double sameTime = 20.0;
        
        std::printf("Inserting events with same time (%.1f) in random order:\n", sameTime);
        feqInsert(sameTime, SUBMIT, 1);
        feqInsert(sameTime, DISPATCH, 2);
        feqInsert(sameTime, ADMIT, 3);
        feqInsert(sameTime, WAIT_EVENT, 4);
        feqInsert(sameTime, EXIT, 5);
        feqInsert(sameTime, TIMEOUT, 6);
        feqInsert(sameTime, PREEMPT, 7);
        feqInsert(sameTime, ACTIVATE, 8);
        
        std::printf("\nExpected order (for same time):\n");
        std::printf("1. DISPATCH (xid=2)\n");
        std::printf("2. WAIT_EVENT (xid=4)\n");
        std::printf("3. EXIT (xid=5)\n");
        std::printf("4. TIMEOUT (xid=6)\n");
        std::printf("5. PREEMPT (xid=7)\n");
        std::printf("6. SUBMIT (xid=1) - insertion order\n");
        std::printf("7. ADMIT (xid=3) - insertion order\n");
        std::printf("8. ACTIVATE (xid=8) - insertion order\n");
        
        std::printf("\nActual FEQ:\n");
        feqPrint(stdout, false);

        feqClose();
    }
    catch (const Exception &e) {
        std::printf("Exception in testFeqPriorities: %s (errno=%d)\n", e.what(), e.en);
    }
}

/* ========================== */
/* Main */
/* ========================== */

int main()
{
    soProbeOpen(stdout, 0, 0);

    testFeqOpenClose();
    testFeqInsert();
    testFeqRetrieve();
    testFeqPriorities();

    std::printf("\n==================== All FEQ tests finished ====================\n");
    return 0;
}