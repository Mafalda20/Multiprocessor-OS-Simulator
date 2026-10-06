/*
 *  \author Mafalda
 */

 #include "somm25nm.h"

 namespace group
 {
     void simStepWaitEvent(uint16_t cid)
    {
        uint16_t pid = simProcessorState[cid].pid;

        uint32_t jid;
        pctGet(pid, PctJid, &jid);

        double ioDuration;
        jobGet(jid, JobNextBurstDuration, &ioDuration);

        uint32_t burstIndex;
        jobGet(jid, JobNextBurstIndex, &burstIndex);
        burstIndex++;
        jobSet(jid, JobNextBurstIndex, &burstIndex);

        PctProcessState state = BLOCKED;
        pctSet(pid, PctState, &state);

        feqInsert(simTime + ioDuration, EVENT_OCCURS, pid);

        simProcessorState[cid].idle = true;
        simProcessorState[cid].next = simProcessorCount; 

        if (simIdleHead == simProcessorCount)
        {
            simIdleHead = cid;
            simIdleTail = cid;
        }
        else
        {
            simProcessorState[simIdleTail].next = cid;
            simIdleTail = cid;
        }
    }
 }
 