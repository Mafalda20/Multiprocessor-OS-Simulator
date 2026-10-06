/*
 *  \author Rodrigo Lopes, nmec: 113811
 */

#include "somm25nm.h"

namespace group
{
    void simStepDispatch()
    {
        // Recuperar processo da fila RDY
        uint16_t pid = rdyRetrieve(simTime);
        
        if (pid == 0)
        {
            // Fila RDY vazia, não fazer nada
            return;
        }
        
        // Recuperar o ID do processador idle mais antigo
        uint16_t cid = simIdleHead;
        
        // Atualizar lista de processadores idle
        simIdleHead = simProcessorState[cid].next;
        if (simIdleHead == SIM_UNDEF_INDEX)
        {
            // Era o último idle
            simIdleTail = SIM_UNDEF_INDEX;
        }
        
        // Atualizar estado do processador
        simProcessorState[cid].idle = false;
        simProcessorState[cid].pid = pid;
        
        // Atualizar estado do processo para RUNNING
        PctProcessState state = RUNNING;
        pctSet(pid, PctState, &state);
        
        // Obter JID do processo
        uint32_t jid;
        pctGet(pid, PctJid, &jid);
        
        // Obter duração do próximo burst CPU e avançar índice
        double burstDuration;
        jobGet(jid, JobNextBurstDuration, &burstDuration);

        // garantir que vamos avançar o tempo, nunca recuar
        if (burstDuration < 0.0)
            burstDuration = -burstDuration;
        if (burstDuration <= 0.0)
            burstDuration = 1.0;

        uint32_t burstIndex;
        jobGet(jid, JobNextBurstIndex, &burstIndex);
        burstIndex++;
        jobSet(jid, JobNextBurstIndex, &burstIndex);

        // Verificar próximo burst
        double nextBurst;
        jobGet(jid, JobNextBurstDuration, &nextBurst);

        if (nextBurst == 0.0)
        {
            feqInsert(simTime + burstDuration, EXIT, cid);
        }
        else
        {
            feqInsert(simTime + burstDuration, WAIT_EVENT, cid);
        }
    }
} // end of namespace group

