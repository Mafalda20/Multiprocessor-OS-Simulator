/*
 *  \author Mafalda
 */

 #include "somm25nm.h"

 namespace group
 {
     void simStepExit(uint16_t cid)
    {
        uint16_t pid = simProcessorState[cid].pid;

        uint32_t jid;
        pctGet(pid, PctJid, &jid);

        double finishTime = simTime;
        jobSet(jid, JobFinishTime, &finishTime);

        PctProcessState state = ENDED;
        pctSet(pid, PctState, &state);

        feqInsert(simTime, DELETE, pid);

        simProcessorState[cid].idle = true;
        simProcessorState[cid].next = simProcessorCount; // último

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
 