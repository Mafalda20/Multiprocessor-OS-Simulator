/**
José Francisco Teixeira Mota - 113985
 */

#include <cstdio>
#include <cstdint>
#include <iostream>

#include "somm25nm.h"

using namespace std;

// Pequeno helper para banners bonitos
static void banner(const char *msg)
{
    fprintf(stdout,
            "\n════════════════════════════════════════════\n"
            "  %s\n"
            "════════════════════════════════════════════\n\n",
            msg);
}

/**
 * Cria um batch simples de jobs em memória (tmpfile) e chama simLoadBatch.
 * Formato das linhas:
 *   JID(hex);submitTime;memSize;burst1,burst2,...
 *
 * Bursts têm número ímpar para garantir CPU/IO/CPU, etc.
 */
static void createAndLoadBasicBatch()
{
    banner("TESTE 1: simLoadBatch + simRun (cenário simples)");

    FILE *fin = tmpfile();
    if (fin == nullptr)
    {
        perror("tmpfile");
        return;
    }

    // 3 jobs:
    // jid; submit; memSize; bursts (CPU,IO,CPU,...)
    // -> estes bursts vão exercitar SUBMIT, ADMIT, DISPATCH, WAIT_EVENT,
    //    EVENT_OCCURS, EXIT e DELETE durante o simRun.

    fprintf(fin, "11111111;0.0;1024;20,10,30\n");   // CPU 20, IO 10, CPU 30
    fprintf(fin, "22222222;5.0;2048;15,5,25\n");    // CPU 15, IO 5, CPU 25
    fprintf(fin, "33333333;10.0;1024;10,5,10\n");   // CPU 10, IO 5, CPU 10

    rewind(fin);

    // maxMemSize grande o suficiente para não falhar aqui
    simLoadBatch(fin, 4096);    // cada job tem memSize <= 2048

    fclose(fin);

    // Ver JOB + FEQ depois de carregar o batch
    simPrint(stdout, SimPrintJob | SimPrintFeq, false);

    banner("A correr simRun() até FEQ ficar vazia");

    // cnt = 0  -> correr até acabar
    // blocking = false -> não bloquear se FEQ ficar vazia
    simRun(0, false);

    // Estado após a simulação completa
    simPrint(stdout,
             SimPrintJob |
             SimPrintPct |
             SimPrintMemGlobal |
             SimPrintRdy |
             SimPrintSwp,
             false);
}

/**
 * Teste dirigido para PREEMPT / SUSPEND / ACTIVATE.
 *
 * Aqui não usamos simLoadBatch: criamos 1 job “à mão” e
 * construímos os eventos na FEQ para controlar o cenário.
 */
static void testPreemptSuspendActivate(const SimParameters &params)
{
    banner("TESTE 2: simStepPreempt / simStepSuspend / simStepActivate");

    // Fechar SIM (se estiver aberto) e reabrir limpo com os mesmos parâmetros
    simClose(true);
    simOpen(const_cast<SimParameters *>(&params));

    // --------------------------------------------------------------------
    // 1) Criar um job simples e evento SUBMIT
    // --------------------------------------------------------------------
    uint32_t jid = 0x12345678;
    uint32_t memSize = 1024;

    double bursts[JOB_MAX_BURSTS] = { 20.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    // Nota: só 1 burst CPU (20). Número ímpar = 1 -> ok.

    double submitTime = 0.0;
    jobInsert(jid, submitTime, memSize, bursts);

    // Inserir evento SUBMIT na FEQ
    feqInsert(submitTime, SUBMIT, jid);

    // --------------------------------------------------------------------
    // 2) simStep() 3 vezes: SUBMIT -> ADMIT -> DISPATCH
    // --------------------------------------------------------------------
    // 1º step: SUBMIT → simStepSubmit(jid) → cria PID basePid
    simStep(false);

    // 2º step: ADMIT → simStepAdmit(pid) → memAlloc + READY + RDY + DISPATCH
    simStep(false);

    // 3º step: DISPATCH → simStepDispatch() → mete processo a correr em cid=0
    simStep(false);

    // Primeiro PID criado é igual ao basePid (tabela vazia)
    uint16_t pid = params.basePid;
    uint16_t cid = 0; // primeiro dispatcher usa sempre o primeiro idle (cid=0)

    printf("Antes do PREEMPT: cid=%hu, pid=%hu deve estar em RUNNING\n", cid, pid);
    simPrint(stdout, SimPrintPct | SimPrintRdy, false);

    // --------------------------------------------------------------------
    // 3) Testar simStepPreempt(cid)
    // --------------------------------------------------------------------
    banner("Chamar simStepPreempt(cid)");

    simStepPreempt(cid);

    printf("Depois de PREEMPT: processo deve voltar a READY e ir para RDY\n");
    simPrint(stdout, SimPrintPct | SimPrintRdy, false);

    // --------------------------------------------------------------------
    // 4) Testar simStepSuspend(pid)
    // --------------------------------------------------------------------
    banner("Chamar simStepSuspend(pid)");

    simStepSuspend(pid);

    printf("Depois de SUSPEND: processo deve estar em SWP (S_READY) e memória libertada\n");
    simPrint(stdout,
             SimPrintPct |
             SimPrintSwp |
             SimPrintMemGlobal,
             false);

    // --------------------------------------------------------------------
    // 5) Forçar ACTIVATE com um evento na FEQ
    // --------------------------------------------------------------------
    banner("Forçar ACTIVATE via FEQ (simStepActivate)");

    // Se a fila SWP não estiver vazia, agendar ACTIVATE "à mão"
    if (!swpIsEmpty())
    {
        feqInsert(simTime, ACTIVATE, 0);
        simStep(false); // vai chamar simStepActivate()
    }

    printf("Depois de ACTIVATE: processo deve voltar a memória (READY ou BLOCKED)\n");
    simPrint(stdout,
             SimPrintPct |
             SimPrintRdy |
             SimPrintSwp |
             SimPrintMemGlobal,
             false);
}

int main()
{
    cout << "\n";
    cout << "========================================\n";
    cout << "   SIM MODULE - COMPREHENSIVE TEST\n";
    cout << "========================================\n";

    // Abrir probing (opcional mas consistente com resto do projeto)
    soProbeOpen(stdout, 0, 0);

    // --------------------------------------------------------------------
    // Configuração base do simulador (parametros do simOpen)
    // --------------------------------------------------------------------
    uint32_t memSizesArray[] = { 2, 1 }; 
    // Isto significa (típico do memOpen):
    //   2 blocos de tamanho 2^(memMinLogSize)   -> 2 x 1024
    //   1 bloco de tamanho 2^(memMinLogSize+1) -> 1 x 2048
    // Total = 4096 bytes

    SimParameters params;
    params.processorCount   = 2;
    params.basePid          = 100;
    params.maxPids          = 16;
    params.swappingPolicy   = FirstFit;
    params.schedulingPolicy = SPN;
    params.memInitAddr      = 0x10000;
    params.memMinLogSize    = 10;   // 2^10 = 1024 bytes
    params.memSizesCount    = 2;
    params.memSizes         = memSizesArray;

    banner("Abrir SIM (simOpen)");

    simOpen(&params);
    simPrint(stdout, SimPrintNone, false);

    // --------------------------------------------------------------------
    // TESTE 1: simLoadBatch + simRun (usa internamente simStep + step*...)
    // --------------------------------------------------------------------
    createAndLoadBasicBatch();

    // --------------------------------------------------------------------
    // TESTE 2: PREEMPT / SUSPEND / ACTIVATE (chamadas explícitas)
    // --------------------------------------------------------------------
    testPreemptSuspendActivate(params);

    // --------------------------------------------------------------------
    // Fechar tudo no fim
    // --------------------------------------------------------------------
    banner("Fechar SIM (simClose) + módulos satélite");
    simClose(true);

    cout << "\n========================================\n";
    cout << "   TODOS OS TESTES SIM TERMINADOS\n";
    cout << "========================================\n\n";

    return 0;
}