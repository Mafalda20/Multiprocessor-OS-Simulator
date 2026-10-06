/*
José Francisco Teixeira Mota - 113985
 */

#include "somm25nm.h"
#include <errno.h>
#include <new>

namespace group
{
    void simStepActivate()
    {
        uint32_t sizeAvail = memBiggestFreeBlock();
        if (sizeAvail == 0)
            return; 

        // tentar obter processo swp
        uint16_t pid = swpRetrieve(sizeAvail, false);
        if (pid == 0)
            pid = swpRetrieve(sizeAvail, true);
        if (pid == 0)
            return; 

 
        uint32_t jid;
        pctGet(pid, PctJid, &jid);

        uint32_t memSize;
        jobGet(jid, JobMemSize, &memSize);

        // alocar memória para este processo
        uint32_t addr = memAlloc(pid, memSize);
        if (addr == 0) {
            throw Exception(ENOMEM, __func__);
        }

        // atualizar PCT 
        pctSet(pid, PctMemAddr, &addr);

        PctProcessState state;
        pctGet(pid, PctState, &state);

        if (state == S_READY)
        {
            PctProcessState newState = READY;
            pctSet(pid, PctState, &newState);

            double burst;
            jobGet(jid, JobNextBurstDuration, &burst);

            if (burst < 0.0)
                burst = -burst;         

            if (burst <= 0.0)
                burst = 1.0;             

            rdyInsert(pid, simTime, burst);

            if (simIdleHead != SIM_UNDEF_INDEX && !rdyIsEmpty())
                feqInsert(simTime, DISPATCH, 0);
        }
        else if (state == S_BLOCKED)
        {
            PctProcessState newState = BLOCKED;
            pctSet(pid, PctState, &newState);
        }
        else
        {
            throw Exception(EPERM, __func__);
        }
    }
} // end of namespace group

