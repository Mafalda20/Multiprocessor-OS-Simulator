/*
 *  \author Rodrigo Lopes, nmec: 113811
 */

#include "somm25nm.h"

namespace group
{
     void simStepAdmit(uint16_t pid)
    {
        // obter informações do job 
        uint32_t jid;
        pctGet(pid, PctJid, &jid);

        uint32_t memSize;
        jobGet(jid, JobMemSize, &memSize);

        // alocar memória para o processo
        uint32_t memAddr = memAlloc(pid, memSize);

        if (memAddr != PCT_UNDEF_ADDRESS)
        {

            pctSet(pid, PctMemAddr, &memAddr);

            PctProcessState state = READY;
            pctSet(pid, PctState, &state);

            double runTime;
            jobGet(jid, JobNextBurstDuration, &runTime);

            if (runTime < 0.0)
                runTime = -runTime;    

            if (runTime <= 0.0)
                runTime = 1.0;         

            rdyInsert(pid, simTime, runTime);

            if (simIdleHead != simProcessorCount)
            {
                feqInsert(simTime, DISPATCH, 0);
            }
        }
        else
        {
            // inserir na fila SWP
            PctProcessState newState = S_READY;
            pctSet(pid, PctState, &newState);

            swpInsert(pid, memSize, false);
        }
    }
} // end of namespace group

