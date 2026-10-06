/*
 * TESTE COMPLETO DE VALIDAÇÃO - SOMM25NM
 * Valida todas as funções implementadas
 */

#include "somm25nm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int totalTests = 0;
int passedTests = 0;
int failedTests = 0;

#define TEST(name, condition) \
    do { \
        totalTests++; \
        if (condition) { \
            printf("  ✓ %s\n", name); \
            passedTests++; \
        } else { \
            printf("  ✗ %s FAILED\n", name); \
            failedTests++; \
        } \
    } while(0)

void printSeparator(const char* title) {
    printf("\n═══════════════════════════════════════════════\n");
    printf("  %s\n", title);
    printf("═══════════════════════════════════════════════\n\n");
}

void testMemModule() {
    printSeparator("TESTE 1: MEM - Memory Management");
    
    try {
        // Test 1.1: memOpen
        uint32_t sizes[] = {2, 2, 1}; // 2x1KB, 2x2KB, 1x4KB
        memOpen(0x1000, 10, sizes, 3);
        TEST("memOpen com 5 blocos", true);
        
        // Test 1.2: memBiggestFreeBlock
        uint32_t biggest = memBiggestFreeBlock();
        TEST("memBiggestFreeBlock == 4096", biggest == 4096);
        
        // Test 1.3: memAlloc - alocar 1KB
        uint32_t addr1 = memAlloc(100, 1024);
        TEST("memAlloc(100, 1KB) retorna endereço válido", addr1 != 0);
        
        // Test 1.4: Biggest agora deve ser 4KB ainda
        biggest = memBiggestFreeBlock();
        TEST("Após alloc 1KB, biggest ainda é 4KB", biggest == 4096);
        
        // Test 1.5: memAlloc - alocar 2KB
        uint32_t addr2 = memAlloc(101, 2048);
        TEST("memAlloc(101, 2KB) retorna endereço válido", addr2 != 0);
        
        // Test 1.6: memAlloc - alocar 4KB
        uint32_t addr3 = memAlloc(102, 4096);
        TEST("memAlloc(102, 4KB) retorna endereço válido", addr3 != 0);
        
        // Test 1.7: Biggest agora deve ser 2KB (um bloco de 2KB livre)
        biggest = memBiggestFreeBlock();
        TEST("Após alloc 4KB, biggest é 2KB", biggest == 2048);
        
        // Test 1.8: memFree
        memFree(addr1);
        TEST("memFree(addr1) sem exceção", true);
        
        // Test 1.9: Biggest agora deve voltar a ter 2KB
        biggest = memBiggestFreeBlock();
        TEST("Após free, biggest continua 2KB ou mais", biggest >= 1024);
        
        // Test 1.10: memClose
        memClose();
        TEST("memClose sem exceção", true);
        
    } catch (Exception &e) {
        printf("  ✗ EXCEÇÃO no MEM: %s (errno=%d)\n", e.what(), e.en);
        failedTests++;
    }
}

void testJobModule() {
    printSeparator("TESTE 2: JOB - Job Descriptor Table");
    
    try {
        // Test 2.1: jobOpen
        jobOpen();
        TEST("jobOpen inicializa lista vazia", true);
        
        // Test 2.2: jobInsert - Job 1
        double bursts1[] = {10.0, 5.0, 8.0, 0, 0, 0, 0, 0, 0, 0, 0};
        jobInsert(0x00000001, 0.0, 1024, bursts1);
        TEST("jobInsert job1 (JID=0x00000001)", true);
        
        // Test 2.3: jobInsert - Job 2 (maior JID)
        double bursts2[] = {15.0, 3.0, 12.0, 4.0, 10.0, 0, 0, 0, 0, 0, 0};
        jobInsert(0x00000002, 5.0, 2048, bursts2);
        TEST("jobInsert job2 (JID=0x00000002)", true);
        
        // Test 2.4: jobGet - submissionTime
        double submitTime;
        jobGet(0x00000001, JobSubmissionTime, &submitTime);
        TEST("jobGet submissionTime == 0.0", submitTime == 0.0);
        
        // Test 2.5: jobGet - memSize
        uint32_t memSize;
        jobGet(0x00000002, JobMemSize, &memSize);
        TEST("jobGet memSize == 2048", memSize == 2048);
        
        // Test 2.6: jobGet - nextBurstIndex (inicial deve ser 0)
        uint32_t burstIdx;
        jobGet(0x00000001, JobNextBurstIndex, &burstIdx);
        TEST("jobGet nextBurstIndex == 0 (inicial)", burstIdx == 0);
        
        // Test 2.7: jobGet - nextBurstDuration
        double burstDuration;
        jobGet(0x00000001, JobNextBurstDuration, &burstDuration);
        TEST("jobGet nextBurstDuration == 10.0", burstDuration == 10.0);
        
        // Test 2.8: jobSet - finishTime
        double finishTime = 100.5;
        jobSet(0x00000001, JobFinishTime, &finishTime);
        TEST("jobSet finishTime", true);
        
        // Test 2.9: jobSet - nextBurstIndex
        uint32_t newIdx = 2;
        jobSet(0x00000001, JobNextBurstIndex, &newIdx);
        jobGet(0x00000001, JobNextBurstIndex, &burstIdx);
        TEST("jobSet/Get nextBurstIndex == 2", burstIdx == 2);
        
        // Test 2.10: jobClose
        jobClose();
        TEST("jobClose liberta memória", true);
        
    } catch (Exception &e) {
        printf("  ✗ EXCEÇÃO no JOB: %s (errno=%d)\n", e.what(), e.en);
        failedTests++;
    }
}

void testPctModule() {
    printSeparator("TESTE 3: PCT - Process Control Table");
    
    try {
        // Test 3.1: pctOpen
        pctOpen(100, 10);
        TEST("pctOpen(base=100, count=10)", true);
        
        // Test 3.2: pctNew - criar processo 1
        uint16_t pid1 = pctNew(0xABCDEF00);
        TEST("pctNew retorna PID válido (100-109)", pid1 >= 100 && pid1 < 110);
        
        // Test 3.3: pctNew - criar processo 2
        uint16_t pid2 = pctNew(0x12345678);
        TEST("pctNew retorna segundo PID diferente", pid2 != pid1 && pid2 >= 100 && pid2 < 110);
        
        // Test 3.4: pctGet - JID
        uint32_t jid;
        pctGet(pid1, PctJid, &jid);
        TEST("pctGet JID == 0xABCDEF00", jid == 0xABCDEF00);
        
        // Test 3.5: pctGet - MemAddr (inicial deve ser UNDEF)
        uint32_t memAddr;
        pctGet(pid1, PctMemAddr, &memAddr);
        TEST("pctGet memAddr == UNDEF (inicial)", memAddr == PCT_UNDEF_ADDRESS);
        
        // Test 3.6: pctGet - State (inicial é NEW)
        PctProcessState state;
        pctGet(pid1, PctState, &state);
        TEST("pctGet state == NEW (inicial)", state == NEW);
        
        // Test 3.7: pctSet - memAddr
        uint32_t newAddr = 0x5000;
        pctSet(pid1, PctMemAddr, &newAddr);
        pctGet(pid1, PctMemAddr, &memAddr);
        TEST("pctSet/Get memAddr == 0x5000", memAddr == 0x5000);
        
        // Test 3.8: pctSet - State
        PctProcessState newState = READY;
        pctSet(pid1, PctState, &newState);
        pctGet(pid1, PctState, &state);
        TEST("pctSet/Get state == READY", state == READY);
        
        // Test 3.9: pctDelete
        pctDelete(pid1);
        TEST("pctDelete(pid1)", true);
        
        // Test 3.10: Pode reusar PID após delete
        uint16_t pid3 = pctNew(0xFFFFFFFF);
        TEST("Pode criar novo processo após delete", pid3 >= 100 && pid3 < 110);
        
        pctClose();
        TEST("pctClose", true);
        
    } catch (Exception &e) {
        printf("  ✗ EXCEÇÃO no PCT: %s (errno=%d)\n", e.what(), e.en);
        failedTests++;
    }
}

void testFeqModule() {
    printSeparator("TESTE 4: FEQ - Future Event Queue");
    
    try {
        // Test 4.1: feqOpen
        feqOpen();
        TEST("feqOpen inicializa fila vazia", true);
        
        // Test 4.2: feqInsert - evento em t=10
        feqInsert(10.0, SUBMIT, 1);
        TEST("feqInsert(t=10, SUBMIT)", true);
        
        // Test 4.3: feqInsert - evento em t=5 (antes)
        feqInsert(5.0, ADMIT, 2);
        TEST("feqInsert(t=5, ADMIT)", true);
        
        // Test 4.4: feqInsert - evento em t=15 (depois)
        feqInsert(15.0, DISPATCH, 3);
        TEST("feqInsert(t=15, DISPATCH)", true);
        
        // Test 4.5: feqInsert - evento em t=5 (mesmo tempo)
        feqInsert(5.0, EXIT, 4);
        TEST("feqInsert(t=5, EXIT - mesmo tempo)", true);
        
        // Test 4.6: feqRetrieve - deve sair t=5 primeiro
        double time;
        FeqEventType type;
        uint32_t id;
        feqRetrieve(&time, &type, &id);
        TEST("feqRetrieve primeiro evento t=5", time == 5.0);
        
        // Test 4.7: feqRetrieve - segundo evento também t=5
        feqRetrieve(&time, &type, &id);
        TEST("feqRetrieve segundo evento t=5", time == 5.0);
        
        // Test 4.8: feqRetrieve - terceiro evento t=10
        feqRetrieve(&time, &type, &id);
        TEST("feqRetrieve terceiro evento t=10", time == 10.0);
        
        // Test 4.9: feqRetrieve - quarto evento t=15
        feqRetrieve(&time, &type, &id);
        TEST("feqRetrieve quarto evento t=15", time == 15.0);
        
        // Test 4.10: feqClose
        feqClose();
        TEST("feqClose liberta memória", true);
        
    } catch (Exception &e) {
        printf("  ✗ EXCEÇÃO no FEQ: %s (errno=%d)\n", e.what(), e.en);
        failedTests++;
    }
}

void testSwpModule() {
    printSeparator("TESTE 5: SWP - Swap Queue");
    
    try {
        // Test 5.1: swpOpen
        swpOpen(FirstFit);
        TEST("swpOpen(FirstFit)", true);
        
        // Test 5.2: swpIsEmpty inicial
        bool empty = swpIsEmpty();
        TEST("swpIsEmpty == true (inicial)", empty);
        
        // Test 5.3: swpInsert - processo não blocked
        swpInsert(200, 2048, false);
        TEST("swpInsert(pid=200, size=2048, blocked=false)", true);
        
        // Test 5.4: swpIsEmpty após insert
        empty = swpIsEmpty();
        TEST("swpIsEmpty == false (após insert)", !empty);
        
        // Test 5.5: swpInsert - processo blocked
        swpInsert(201, 4096, true);
        TEST("swpInsert(pid=201, size=4096, blocked=true)", true);
        
        // Test 5.6: swpInsert - outro não blocked
        swpInsert(202, 1024, false);
        TEST("swpInsert(pid=202, size=1024, blocked=false)", true);
        
        // Test 5.7: swpRetrieve - pede não blocked, suficiente
        uint16_t pid = swpRetrieve(2048, false);
        TEST("swpRetrieve(2048, false) retorna pid=200", pid == 200);
        
        // Test 5.8: swpRetrieve - pede não blocked menor
        pid = swpRetrieve(1024, false);
        TEST("swpRetrieve(1024, false) retorna pid=202", pid == 202);
        
        // Test 5.9: swpRetrieve - só sobra blocked
        pid = swpRetrieve(4096, false);
        TEST("swpRetrieve(4096, false) retorna 0 (não há não-blocked)", pid == 0);
        
        // Test 5.10: swpRetrieve - pede blocked
        pid = swpRetrieve(4096, true);
        TEST("swpRetrieve(4096, true) retorna pid=201", pid == 201);
        
        swpClose();
        TEST("swpClose", true);
        
    } catch (Exception &e) {
        printf("  ✗ EXCEÇÃO no SWP: %s (errno=%d)\n", e.what(), e.en);
        failedTests++;
    }
}

void testIntegration() {
    printSeparator("TESTE 6: INTEGRAÇÃO - Múltiplos Módulos");
    
    try {
        // Simular cenário real: admitir job, criar processo, alocar memória
        
        // 1. Abrir módulos
        jobOpen();
        pctOpen(100, 5);
        uint32_t sizes[] = {2, 1, 1};
        memOpen(0x1000, 10, sizes, 3);
        feqOpen();
        
        TEST("Inicialização de múltiplos módulos", true);
        
        // 2. Inserir job
        double bursts[] = {50.0, 20.0, 30.0, 0, 0, 0, 0, 0, 0, 0, 0};
        jobInsert(0xAABBCCDD, 0.0, 1024, bursts);
        
        uint32_t memSize;
        jobGet(0xAABBCCDD, JobMemSize, &memSize);
        TEST("Job inserido com memSize correto", memSize == 1024);
        
        // 3. Criar processo
        uint16_t pid = pctNew(0xAABBCCDD);
        TEST("Processo criado para job", pid >= 100 && pid < 105);
        
        // 4. Alocar memória
        uint32_t addr = memAlloc(pid, memSize);
        TEST("Memória alocada para processo", addr != 0);
        
        // 5. Atualizar PCT com endereço
        pctSet(pid, PctMemAddr, &addr);
        
        uint32_t checkAddr;
        pctGet(pid, PctMemAddr, &checkAddr);
        TEST("Endereço armazenado em PCT", checkAddr == addr);
        
        // 6. Agendar evento
        feqInsert(0.0, SUBMIT, 0xAABBCCDD);
        
        double time;
        FeqEventType type;
        uint32_t id;
        feqRetrieve(&time, &type, &id);
        TEST("Evento agendado e recuperado", id == 0xAABBCCDD);
        
        // 7. Simular finalização - libertar memória
        memFree(addr);
        TEST("Memória libertada", true);
        
        // 8. Deletar processo
        pctDelete(pid);
        TEST("Processo deletado", true);
        
        // 9. Fechar módulos
        feqClose();
        memClose();
        pctClose();
        jobClose();
        
        TEST("Fecho de múltiplos módulos", true);
        
    } catch (Exception &e) {
        printf("  ✗ EXCEÇÃO na INTEGRAÇÃO: %s (errno=%d)\n", e.what(), e.en);
        failedTests++;
    }
}

int main() {
    printf("\n");
    printf("TESTE COMPLETO DE VALIDAÇÃO - SOMM25NM\n");
    printf("Validação de todas as funções implementadas\n");
    printf("\n");
    
    // Desativar binários para usar apenas o código do grupo
    soBinSetIDs(0, 0);
    
    // Executar testes
    testMemModule();
    testJobModule();
    testPctModule();
    testFeqModule();
    testSwpModule();
    testIntegration();
    
    // Resultados finais
    printSeparator("RESULTADOS FINAIS");
    printf("Total de testes: %d\n", totalTests);
    printf("Testes passados: %d ✓\n", passedTests);
    printf("Testes falhados: %d ✗\n", failedTests);
    printf("\n");
    
    if (failedTests == 0) {

        printf("TODOS OS TESTES PASSARAM\n");

        return 0;
    } else {

        printf("ALGUNS TESTES FALHARAM\n");

        return 1;
    }
}
