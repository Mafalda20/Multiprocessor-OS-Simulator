#include "somm25nm.h"
#include "exception.h"

#include <cstdio>
#include <cstdint>
#include <new>
#include <errno.h>

// Vamos aceder a algumas variáveis globais definidas nos frontends
// (apenas para testes, não é para entrega!)

extern PctNode **pctTable;
extern uint16_t pctPidBase;
extern uint16_t pctPidCount;

extern SwpNode *swpHead;
extern SwpNode *swpTail;
extern SwpSwappingPolicy swpPolicy;

extern double simTime;
extern uint32_t simProcessorCount;
extern SimProcessorState *simProcessorState;
extern uint16_t simIdleHead;
extern uint16_t simIdleTail;

static void banner(const char *msg)
{
    std::printf("\n==================== %s ====================\n\n", msg);
}

/* ========================== */
/* Teste RDY: rdyOpen/rdyClose */
/* ========================== */

static void testRdy()
{
    banner("Testing RDY (rdyOpen / rdyClose)");

    // garantir que usamos o código do grupo e não o binário
    soBinSetIDs(0, 0); // IDs fora disto usam sempre group

    try {
        rdyOpen(SPN);
        std::printf("RDY opened with SPN policy OK.\n");
        rdyClose();
        std::printf("RDY closed OK.\n");
    }
    catch (const Exception &e) {
        std::printf("Exception in testRdy: %s (errno=%d)\n", e.what(), e.en);
    }
}

/* ========================== */
/* Teste PCT: open/print/get/set */
/* ========================== */

static void testPct()
{
    banner("Testing PCT (pctOpen / pctPrint / pctGet / pctSet)");

    soBinSetIDs(0, 0);

    try {
        uint16_t base = 100;
        uint16_t cnt  = 4;
        pctOpen(base, cnt);

        // Em vez de usar pctNew (ainda por implementar),
        // criamos nós manualmente na tabela:
        uint16_t idx0 = 0; // pid = 100
        uint16_t idx2 = 2; // pid = 102

        pctTable[idx0] = new PctNode{ 0xAAAABBBB, 0x1000, READY };
        pctTable[idx2] = new PctNode{ 0xCCCCDDDD, 0x2000, BLOCKED };

        std::printf("PCT after manual insertion (normal):\n");
        pctPrint(stdout, false);

        std::printf("\nPCT after manual insertion (CSV):\n");
        pctPrint(stdout, true);

        // Testar pctGet
        uint32_t jid;
        uint32_t memAddr;
        PctProcessState state;

        pctGet(100, PctJid, &jid);
        pctGet(100, PctMemAddr, &memAddr);
        pctGet(100, PctState, &state);

        std::printf("\npctGet(100,*): jid=0x%08x, mem=0x%08x, state=%d\n",
                    jid, memAddr, (int)state);

        // Testar pctSet: mudar estado e memAddr do PID 100
        uint32_t newMem = 0xDEAD;
        PctProcessState newState = RUNNING;
        pctSet(100, PctMemAddr, &newMem);
        pctSet(100, PctState, &newState);

        pctGet(100, PctMemAddr, &memAddr);
        pctGet(100, PctState, &state);

        std::printf("After pctSet on PID 100: mem=0x%08x, state=%d\n",
                    memAddr, (int)state);

        // limpeza manual (já que pctClose ainda não está feito)
        delete pctTable[idx0];
        delete pctTable[idx2];
        delete [] pctTable;
        pctTable    = PCT_UNDEF_TABLE;
        pctPidBase  = 0;
        pctPidCount = 0;
    }
    catch (const Exception &e) {
        std::printf("Exception in testPct: %s (errno=%d)\n", e.what(), e.en);
    }
}

/* ========================== */
/* Teste SWP: swpRetrieve */
/* ========================== */

static void testSwp()
{
    banner("Testing SWP (swpRetrieve)");

    soBinSetIDs(0, 0);

    try {
        // Vamos montar o módulo SWP manualmente:
        swpPolicy = FirstFit;
        swpHead = nullptr;
        swpTail = nullptr;

        // Criar lista: [ (1,100,false) -> (2,200,true) -> (3,150,false) ]
        SwpNode *n1 = new SwpNode{1, 100, false, nullptr};
        SwpNode *n2 = new SwpNode{2, 200, true, nullptr};
        SwpNode *n3 = new SwpNode{3, 150, false, nullptr};

        swpHead = n1;
        n1->next = n2;
        n2->next = n3;
        n3->next = nullptr;
        swpTail = n3;

        std::printf("FirstFit, canBeBlocked = false, sizeAvailable = 160\n");
        uint16_t pid = swpRetrieve(160, false);
        std::printf("swpRetrieve -> pid = %hu (expected: 1)\n", pid);

        std::printf("FirstFit, again, canBeBlocked = false, sizeAvailable = 160\n");
        pid = swpRetrieve(160, false);
        std::printf("swpRetrieve -> pid = %hu (expected: 3)\n", pid);

        std::printf("FirstFit, again, canBeBlocked = false, sizeAvailable = 160\n");
        pid = swpRetrieve(160, false);
        std::printf("swpRetrieve -> pid = %hu (expected: 0)\n", pid);

        // limpeza
        SwpNode *cur = swpHead;
        while (cur != nullptr)
        {
            SwpNode *next = cur->next;
            delete cur;
            cur = next;
        }
        swpHead = swpTail = nullptr;
    }
    catch (const Exception &e) {
        std::printf("Exception in testSwp: %s (errno=%d)\n", e.what(), e.en);
    }
}

/* ========================== */
/* Teste SIM: simPrint */
/* ========================== */

static void testSim()
{
    banner("Testing SIM (simPrint)");

    soBinSetIDs(0, 0);

    try {
        // Montar estado mínimo coerente
        simTime = 123.456;

        simProcessorCount = 3;
        simProcessorState = new SimProcessorState[simProcessorCount];

        // CPU 0: IDLE -> next = 2
        // CPU 1: RUNNING pid=42
        // CPU 2: IDLE -> next = simProcessorCount (fim da lista)
        simProcessorState[0].next = 2;
        simProcessorState[1].pid  = 42;
        simProcessorState[2].next = simProcessorCount;

        simIdleHead = 0;
        simIdleTail = 2;

        std::printf("SIM print (normal):\n");
        simPrint(stdout, SimPrintNone, false);

        std::printf("\nSIM print (CSV):\n");
        simPrint(stdout, SimPrintNone, true);

        // limpeza
        delete [] simProcessorState;
        simProcessorState = SIM_UNDEF_POINTER;
        simProcessorCount = 0;
        simTime = SIM_UNDEF_TIME;
        simIdleHead = simIdleTail = SIM_UNDEF_INDEX;
    }
    catch (const Exception &e) {
        std::printf("Exception in testSim: %s (errno=%d)\n", e.what(), e.en);
    }
}

int main()
{
    // abrir sistema de probing (como o prof faz)
    soProbeOpen(stdout, 0, 0);

    testRdy();
    testPct();
    testSwp();
    testSim();

    std::printf("\nAll tests finished.\n");
    return 0;
}
