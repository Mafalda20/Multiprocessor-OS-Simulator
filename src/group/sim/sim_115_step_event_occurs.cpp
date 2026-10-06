/*
 *  \author Antonio Caetano
 */

#include "somm25nm.h"

namespace group
{
    void simStepEventOccurs(uint16_t pid)
    {
        PctProcessState state;
        pctGet(pid, PctState, &state);

        if (state == S_BLOCKED)
        {
            PctProcessState newState = S_READY;
            pctSet(pid, PctState, &newState);
        }
        else if (state == BLOCKED)
        {
            PctProcessState newState = READY;
            pctSet(pid, PctState, &newState);
            
            uint32_t jid;
            pctGet(pid, PctJid, &jid);

            double burstDuration;
            jobGet(jid, JobNextBurstDuration, &burstDuration);

            if (burstDuration < 0.0)
                burstDuration = -burstDuration;

            if (burstDuration <= 0.0)
                burstDuration = 1.0;   
            
            rdyInsert(pid, simTime, burstDuration);
            
            if (simIdleHead != SIM_UNDEF_INDEX)
            {
                feqInsert(simTime, DISPATCH, 0);
            }
        }
        else
        {
            throw Exception(EPERM, __func__);
        }
    }
} // end of namespace group

