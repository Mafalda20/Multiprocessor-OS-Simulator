/*
 *  \author João Pinto
 */

#include "somm25nm.h"

namespace group
{
    void simStepPreempt(uint16_t cid)
    {
        uint16_t pid = simProcessorState[cid].pid;

        PctProcessState state = READY;
        pctSet(pid, PctState, &state);

        uint32_t jid;
        pctGet(pid, PctJid, &jid);

        double runTime;
        jobGet(jid, JobNextBurstDuration, &runTime);

        if (runTime < 0.0)
        {
            runTime = -runTime;
        }

        if (runTime <= 0.0)
        {
            runTime = 1.0;  
        }

        rdyInsert(pid, simTime, runTime);

        simProcessorState[cid].next = simProcessorCount;

        if (simIdleTail == SIM_UNDEF_INDEX)
        {
            simIdleHead = cid;
            simIdleTail = cid;
        }
        else
        {
            simProcessorState[simIdleTail].next = cid;
            simIdleTail = cid;
        }

        feqInsert(simTime, DISPATCH, 0);
    }
} // end of namespace group