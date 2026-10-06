/*
 *  \author João Pinto
 */

#include "somm25nm.h"

namespace group
{
    void simStepSubmit(uint32_t jid)
    {
        // Try to create a new process
        uint16_t pid;
        
        try
        {
            pid = pctNew(jid);
        }
        catch (const Exception &e)
        {
            // No process available - reject the job
            double finishTime = simTime;
            jobSet(jid, JobFinishTime, &finishTime);
            return;
        }
        
        // Process created successfully - schedule ADMIT event
        feqInsert(simTime, ADMIT, pid);
    }
} // end of namespace group

